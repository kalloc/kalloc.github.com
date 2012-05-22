/*
 * http.c
 *
 *  Created on: 23.01.2010
 *      Author: nick
 */

#include "include.h"

struct event_base *base;
struct evhttp *http;
struct event alarmEv;
#ifdef THREAD
static pthread_mutex_t *send_mutex = NULL;
#endif

struct timeval tv;

char *getHostString();
int getPort();

inline struct event_base * getBase() {
    return base;
}

inline void replySend(struct Request *Req, struct evbuffer *out) {
    struct evkeyvalq *headOut;
    headOut = evhttp_request_get_output_headers(Req->evreq);
    evhttp_add_header(headOut, "Content-Type", "text/javascript; charset=utf8");
    evhttp_add_header(headOut, "Server", "Regplanet.JSON");
    evhttp_send_reply(Req->evreq, HTTP_OK, "OK", out);
    free(Req->taskDomain);
    free(Req);
}

inline void errorSend(struct evhttp_request *evreq) {
    struct evbuffer *out = evbuffer_new();
    struct evkeyvalq *headOut;
    headOut = evhttp_request_get_output_headers(evreq);
    evhttp_add_header(headOut, "Server", "Regplanet.JSON");
    evhttp_send_reply(evreq, HTTP_BADREQUEST, "BADBOY", out);
    evbuffer_free(out);
}


#ifdef LIST

struct ReportListEntry {
    struct Request *Req;
    LIST_ENTRY(ReportListEntry) entries; /* List. */
} *ReportEntryPtr;

LIST_HEAD(ReportListHead, ReportListEntry) ReportHead;

void alarmReport(int fd, short action, void *arg) {
    struct event *ev = (struct event *) arg;
#ifdef THREAD
    pthread_mutex_lock(send_mutex);
#endif

    LIST_FOREACH(ReportEntryPtr, &ReportHead, entries) {
        if (ReportEntryPtr->Req and ReportEntryPtr->Req->callback) {
            ReportEntryPtr->Req->callback(ReportEntryPtr->Req);
        }
        LIST_REMOVE(ReportEntryPtr, entries);
        free(ReportEntryPtr);
    }
#ifdef THREAD
    pthread_mutex_unlock(send_mutex);
#endif
    tv.tv_sec = 0;
    tv.tv_usec = 50;
    evtimer_add(ev, &tv);
}
#endif

#ifndef LIST

void http_Reply_timer(int fd, short action, void *arg) {
    struct Request *Req = (struct Request *) arg;
    event_del(&Req->ev);
    //printf("%p->ev = %p\n",Req,Req->ev.ev_base);
    Req->callback(Req);
}
#endif

void addDigReport_cb(struct Task * task) {
    struct timeval tv;
    struct Request *Req = task->Req;
    debug("task %p %p", task, Req);
#ifdef LIST
#ifdef THREAD
    static pthread_mutex_t *mutex = 0;
    if (!mutex) {
        mutex = calloc(1, sizeof (*mutex));
        pthread_mutex_init(mutex, NULL);
    }
#endif
#endif

    if (task->returnCode == RESULT_NOTFOUND) {
        json_AddItemToObject(Req->root, type_name(task->taskType), json_CreateNull());
    } else if (task->returnCode == RESULT_ERROR) {
        json_AddItemToObject(Req->root, type_name(task->taskType), json_CreateFalse());
    } else {
        json_AddItemToObject(Req->root, type_name(task->taskType), task->child);
    }
    debug("%s\n", type_name(task->taskType));

    Req->countTask -= 1;
    free(task);

    if (Req->countTask > 0) {
        return;
    }

#ifdef LIST
#ifdef THREAD
    pthread_mutex_lock(mutex);
    pthread_mutex_lock(send_mutex);
#endif

    ReportEntryPtr = getNulledMemory(sizeof (struct ReportListEntry)); /* Insert at the head. */
    ReportEntryPtr->Req = Req;
    debug("Req %p\n", ReportEntryPtr->Req);
    LIST_INSERT_HEAD(&ReportHead, ReportEntryPtr, entries);

#ifdef THREAD
    pthread_mutex_unlock(mutex);
    pthread_mutex_unlock(send_mutex);
#endif


#else

#ifdef THREAD
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    event_assign(&Req->ev, base, -1, 0, http_Reply_timer, Req);
    assert(Req->ev.ev_base == base);
    //printf("%p %p\n",base,Req->ev.ev_base);
    evtimer_add(&Req->ev, &tv);
#else
    Req->callback(Req);
#endif
#endif
}

inline void http_dig_reply_cb(struct Request *Req) {

    json *root;
    struct evbuffer *out = evbuffer_new();
    root = json_CreateObject();
    json_AddItemToObject(root, "reply", Req->root);
    json_AddItemToObject(root, "query", json_CreateString(Req->taskDomain));
    evbuffer_add_printf(out, json_Print(root));
    replySend(Req, out);
    evbuffer_free(out);
    json_Delete(root);
    return;
}

static void http_dig(struct evhttp_request *evreq, void *arg) {
    struct evkeyvalq query;
    struct evbuffer *out;
    struct Task *task;
    json *root;
    struct Request *Req;
    short flag = 0;
    unsigned int dnsType;


    debug("connect from %s:%d try %s", evreq->remote_host, evreq->remote_port, evhttp_request_get_uri(evreq));
    evhttp_parse_query(evhttp_request_get_uri(evreq), &query);

    char *domain, *query_type, *ns;

    domain = evhttp_find_header(&query, "dig");
    query_type = evhttp_find_header(&query, "query");
    //printf("=> %s %s\n",domain,query_type);
    if (domain and query_type) {
        Req = getNulledMemory(sizeof (*Req));
        Req->root = json_CreateObject();
        Req->callback = http_dig_reply_cb;
        Req->taskDomain = strdup(domain);

        Req->evreq = evreq;
        for (; *query_type != 0; query_type++) {

            if (flag == 0 and(dnsType = StringToDNSType(&query_type))) {

                task = getNulledMemory(sizeof (*task));
                task->child = json_CreateArray();
                task->callback = addDigReport_cb;
                task->Req = Req;
                task->taskType = dnsType;
                Req->countTask += 1;
                makeDNSTask(task);
                flag = 1;
            } else if (query_type[0] == '|') {
                flag = 0;
            }
        }
        if (Req->countTask == 0) {
            out = evbuffer_new();
            query_type = evhttp_encode_uri(evreq->uri);
            evbuffer_add_printf(out, "{\n\t\"error\":\t\"param\",\n\t\"query\":\t\"%s\"\n}", query_type);
            free(query_type);
            replySend(Req, out);
        }
    } else {
        errorSend(evreq);
    }
    evhttp_clear_headers(&query);
}

void initHttp() {
    base = event_base_new();
    http = evhttp_new(base);
#ifdef THREAD
    evthread_use_pthreads();
    if (!send_mutex) {
        send_mutex = calloc(1, sizeof (*send_mutex));
        pthread_mutex_init(send_mutex, NULL);
    }
    evthread_make_base_notifiable(base);
#endif
#ifdef LIST
    LIST_INIT(&ReportHead);
    tv.tv_sec = 0;
    tv.tv_usec = 50;
    event_assign(&alarmEv, base, -1, 0, alarmReport, &alarmEv);
    evtimer_add(&alarmEv, &tv);
#endif
    evhttp_set_cb(http, "/dig", http_dig, NULL);
    evhttp_bind_socket(http, getHostString(), getPort());


    event_base_dispatch(base);
}

void destroyHttp() {

}
