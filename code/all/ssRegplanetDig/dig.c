/*
 * dig.c
 *
 *  Created on: 26.01.2010
 *      Author: nick
 */
#include "include.h"

struct timeval tv;
static struct event_base *base;
struct event_base inline  *getDNSBase() {
#ifdef THREAD
    return base;
#else
    return getBase();
#endif
}

static inline char * getPTR(char *host) {
    char arpa[100];
    u_char *ip;
    in_addr_t in = inet_addr((const char *) host);
    if (in == -1)
        return host;
    ip = (char *) & in;
    snprintf(arpa, 100, "%d.%d.%d.%d.in-addr.arpa", ip[3], ip[2], ip[1], ip[0]);
    return arpa;
}

inline static const unsigned char *CheckPatternAfterParseAnswer(
        struct Task *task, const unsigned char *aptr,
        const unsigned char *abuf, int alen) {

    const unsigned char *p;
    int type, dnsclass, ttl, dlen, status;
    long len;
    char addr[46];

    /*
     if (task->LObjId == 1148) raise(SIGTRAP);
     */

    union {
        unsigned char * as_uchar;
        char * as_char;
    } name;

    status = ares_expand_name(aptr, abuf, alen, &name.as_char, &len);
    if (status != ARES_SUCCESS)
        return NULL;
    aptr += len;

    if (aptr + RRFIXEDSZ > abuf + alen) {
        ares_free_string(name.as_char);
        return NULL;
    }
    //	hexPrint(aptr, alen);
    type = DNS_RR_TYPE(aptr);
    dnsclass = DNS_RR_CLASS(aptr);
    ttl = DNS_RR_TTL(aptr);
    dlen = DNS_RR_LEN(aptr);
    aptr += RRFIXEDSZ;
    if (aptr + dlen > abuf + alen) {
        ares_free_string(name.as_char);
        return NULL;
    }
    ares_free_string(name.as_char);
    switch (type) {
        case T_CNAME:
        case T_MB:
        case T_MD:
        case T_MF:
        case T_MG:
        case T_MR:
        case T_NS:
        case T_PTR:

            status = ares_expand_name(aptr, abuf, alen, &name.as_char, &len);
            if (status != ARES_SUCCESS) {
                debug("task %p %d", task, status);
                return NULL;
            }
            debug("task %p %s", task, name.as_char);

            ares_free_string(name.as_char);
            break;

        case T_HINFO:
            /* The RR data is two length-counted character strings. */
            p = aptr;
            len = *p;
            if (p + len + 1 > aptr + dlen)
                return NULL;
            debug("\t%.*s", (int) len, p + 1)
                    ;
            p += len + 1;
            len = *p;
            if (p + len + 1 > aptr + dlen)
                return NULL;
            debug("\t%.*s", (int) len, p + 1)
                    ;
            break;

        case T_MINFO:
            /* The RR data is two domain names. */
            p = aptr;
            status = ares_expand_name(p, abuf, alen, &name.as_char, &len);
            if (status != ARES_SUCCESS)
                return NULL;
            debug("\t%s.", name.as_char)
                    ;
            ares_free_string(name.as_char);
            p += len;
            status = ares_expand_name(p, abuf, alen, &name.as_char, &len);
            if (status != ARES_SUCCESS)
                return NULL;
            debug("\t%s.", name.as_char)
                    ;
            ares_free_string(name.as_char);
            break;

        case T_MX:
            if (dlen < 2) {
                return NULL;
            }
            status = ares_expand_name(aptr + 2, abuf, alen, &name.as_char, &len);
            if (status != ARES_SUCCESS) {
                return NULL;
            }

            debug("task %p", task)
                    ;
            ;
            ares_free_string(name.as_char);

            break;

        case T_SOA:
            /* The RR data is two domain names and then five four-byte
             * numbers giving the serial number and some timeouts.
             */
            p = aptr;
            status = ares_expand_name(p, abuf, alen, &name.as_char, &len);
            if (status != ARES_SUCCESS)
                return NULL;
            debug("\t%s.", name.as_char)
                    ;
            ares_free_string(name.as_char);
            p += len;
            status = ares_expand_name(p, abuf, alen, &name.as_char, &len);
            if (status != ARES_SUCCESS)
                return NULL;
            debug("\t\t\t\t\t\t%s.", name.as_char)
                    ;
            ares_free_string(name.as_char);
            p += len;
            if (p + 20 > aptr + dlen)
                return NULL;
            debug("\t\t\t\t\t\t( %lu %lu %lu %lu %lu )",
                    (unsigned long) DNS__32BIT(p), (unsigned long) DNS__32BIT(p + 4),
                    (unsigned long) DNS__32BIT(p + 8), (unsigned long) DNS__32BIT(p + 12),
                    (unsigned long) DNS__32BIT(p + 16))
                    ;
            break;

        case T_TXT:
            /* The RR data is one or more length-counted character
             * strings. */
            p = aptr;
            while (p < aptr + dlen) {
                len = *p;
                if (p + len + 1 > aptr + dlen)
                    return NULL;

                //printf("\t%.*s", (int) len, p + 1);
                debug("task %p", task);
                p += len + 1;
            }
            break;

        case T_A:
            /* The RR data is a four-byte Internet address. */
            debug("task %p %08x %08x", task, *(u32*) aptr, *(u32*) addr);
            json_AddItemToArray(task->child, json_CreateString(inet_ntop(AF_INET, aptr, addr, sizeof (addr))));
            /*
             if (task->task->LObjId == 1056) raise(SIGSEGV);
             */

            break;

        case T_AAAA:
            /* The RR data is a 16-byte IPv6 address. */
            if (dlen != 16)
                return NULL;
            debug("\t%s", inet_ntop(AF_INET6, aptr, addr, sizeof (addr)))
                    ;
            break;

        case T_WKS:
            /* Not implemented yet */
            break;

        case T_SRV:
            /* The RR data is three two-byte numbers representing the
             * priority, weight, and port, followed by a domain name.
             */

            printf("\t%d", DNS__16BIT(aptr));
            printf(" %d", DNS__16BIT(aptr + 2));
            printf(" %d", DNS__16BIT(aptr + 4));

            status = ares_expand_name(aptr + 6, abuf, alen, &name.as_char, &len);
            if (status != ARES_SUCCESS)
                return NULL;
            printf("\t%s.", name.as_char);
            ares_free_string(name.as_char);
            break;

        case T_NAPTR:

            printf("\t%d", DNS__16BIT(aptr)); /* order */
            printf(" %d\n", DNS__16BIT(aptr + 2)); /* preference */

            p = aptr + 4;
            status = ares_expand_string(p, abuf, alen, &name.as_uchar, &len);
            if (status != ARES_SUCCESS)
                return NULL;
            printf("\t\t\t\t\t\t%s\n", name.as_char);
            ares_free_string(name.as_char);
            p += len;

            status = ares_expand_string(p, abuf, alen, &name.as_uchar, &len);
            if (status != ARES_SUCCESS)
                return NULL;
            printf("\t\t\t\t\t\t%s\n", name.as_char);
            ares_free_string(name.as_char);
            p += len;

            status = ares_expand_string(p, abuf, alen, &name.as_uchar, &len);
            if (status != ARES_SUCCESS)
                return NULL;
            printf("\t\t\t\t\t\t%s\n", name.as_char);
            ares_free_string(name.as_char);
            p += len;

            status = ares_expand_string(p, abuf, alen, &name.as_uchar, &len);
            if (status != ARES_SUCCESS)
                return NULL;
            printf("\t\t\t\t\t\t%s", name.as_char);
            ares_free_string(name.as_char);
            break;

        default:
            printf("\t[Unknown RR; cannot parse]");
            break;
    }
    if (task->returnCode != STATE_DONE) {
        return aptr + dlen;
    } else {
        return NULL;
    }
}

void OnEventDNSTask(int fd, short event, void *arg) {
    struct Task *task = (struct Task *) arg;
    debug("task %p", task);
    if (event & EV_READ) {
        ares_process_fd(task->channel, fd, ARES_SOCKET_BAD);
    } else if (event & EV_TIMEOUT) {
        task->returnCode = STATE_TIMEOUT;
    }
    ares_destroy(task->channel);
    event_del(&task->ev);
    debug("callback %p", task);
    task->callback(task);
}

void DNSTaskResolvCallback(void *arg, int status, int timeouts,
        unsigned char *abuf, int alen) {
    struct Task *task = (struct Task *) arg;
    char *query;

    int id, qr, opcode, aa, tc, rd, ra, rcode, len;
    u32 ip;
    unsigned int qdcount, ancount, nscount, arcount, i;
    const unsigned char *aptr;
    debug("task %p %d", task, status);
    if (status != ARES_SUCCESS and status != ARES_ENODATA) {
        if (status == ARES_ENOTFOUND) {
            task->returnCode = RESULT_NOTFOUND;
        }
        return status;
    }

    /* Parse the answer header. */
    id = DNS_HEADER_QID(abuf);
    qr = DNS_HEADER_QR(abuf);
    opcode = DNS_HEADER_OPCODE(abuf);
    aa = DNS_HEADER_AA(abuf);
    tc = DNS_HEADER_TC(abuf);
    rd = DNS_HEADER_RD(abuf);
    ra = DNS_HEADER_RA(abuf);
    rcode = DNS_HEADER_RCODE(abuf);
    qdcount = DNS_HEADER_QDCOUNT(abuf);
    ancount = DNS_HEADER_ANCOUNT(abuf);
    nscount = DNS_HEADER_NSCOUNT(abuf);
    arcount = DNS_HEADER_ARCOUNT(abuf);

    aptr = abuf + HFIXEDSZ;

    ares_expand_name(aptr, abuf, alen, &query, &len);

    for (i = 0; i < qdcount; i++) {
        aptr = skip_question(aptr, abuf, alen);
        if (aptr == NULL)
            return;
    }
    struct hostent *he;
    struct addrttl addrttls;
    char *ptr2 = NULL, *ptr = NULL;

    task->returnCode = STATE_ERROR;
    debug("task %p %d", task, ancount);

    for (i = 0; i < ancount; i++) {
        debug("try %d ", ancount);
        aptr = CheckPatternAfterParseAnswer(task, aptr, abuf, alen);
        if (aptr == NULL)
            break;
    }

}



void runDNSTask_timer(int fd, short action, void *arg) {
    struct Task *task = (struct Task *) arg;
    int i;
    struct ares_options options = {0};
    int optmask;
    optmask = ARES_OPT_FLAGS;
    tv.tv_sec = 1;
    tv.tv_usec = 0;

    i=ares_init_options(&task->channel, &options, optmask);
    if(i != ARES_SUCCESS) {
        task->returnCode=RESULT_ERROR;
        task->callback(task);
        return;
    }
    ares_query(task->channel, getPTR(task->Req->taskDomain), C_IN, task->taskType, DNSTaskResolvCallback, task);
    ares_getsock(task->channel, &task->fd, 1);
    event_assign(&task->ev, getDNSBase(), task->fd, EV_READ | EV_TIMEOUT, OnEventDNSTask, task);
    event_add(&task->ev, &tv);
}

void makeDNSTask(struct Task * task) {
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    event_assign(&task->ev, getDNSBase(), -1, 0, runDNSTask_timer, task);
    evtimer_add(&task->ev, &tv);
}



static void timerMain(int fd, short action, void *arg) {
    struct event *ev = (struct event *) arg;
    tv.tv_sec = 60;
    tv.tv_usec = 0;
    evtimer_add(ev, &tv);
}

static void initThread() {
    base = event_base_new();

    struct event ev;

    tv.tv_sec = 60;
    tv.tv_usec = 0;
    event_assign(&ev, base, -1, 0, timerMain, &ev);
    evtimer_add(&ev, &tv);
    event_base_dispatch(base);
}

void initDig() {
#ifdef THREAD
    pthread_t threads;
    pthread_create(&threads, NULL, (void*) initThread, NULL);
#endif
}

