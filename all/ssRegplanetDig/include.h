/*
 * include.h
 *
 *  Created on: 26.01.2010
 *      Author: nick
 */

#ifndef INCLUDE_H_
#define INCLUDE_H_

#define _GNU_SOURCE     /* Expose declaration of tdestroy() */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <signal.h>
#include <search.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <errno.h>
#include <err.h>
#include <time.h>
#include <ctype.h>
#include <getopt.h>

#include <ares.h>
#include <ares_dns.h>
#include <ares_version.h>

#include <sys/queue.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/resource.h>

#include <arpa/inet.h>
#include <arpa/nameser.h>

#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <netinet/ip.h>

#include <event2/event.h>
#include <event2/thread.h>
#include <event2/event_struct.h>
#include <event2/http.h>
#include <event2/http_struct.h>
#include <event2/http_compat.h>
#include <event2/bufferevent.h>
#include <event2/bufferevent_compat.h>
#include <event2/buffer.h>
#include <event2/buffer_compat.h>
#include "json.h"

//some define
#define THREAD
//#define LIST
#define help() terminate(0)
#define die(err) terminate(err)
#define DEBUG
#ifdef DEBUG
#define debug(...) loger(__FILE__,__FUNCTION__,LOG_DEBUG, __VA_ARGS__);
#define warn(...) loger(__FILE__,__FUNCTION__,LOG_WARN, __VA_ARGS__);
#define notice(...) loger(__FILE__,__FUNCTION__,LOG_NOTICE, __VA_ARGS__);
#else
#define debug(...) {}
#define warn(...) {}
#define notice(...) {}

#endif

#define info(...) loger(NULL,NULL,LOG_INFO, __VA_ARGS__);

//log level
#define LOG_NONE            0
#define LOG_NOTICE          1<<0
#define LOG_INFO            1<<1
#define LOG_WARN            1<<2
#define LOG_DEBUG           1<<3

#define STATE_DISCONNECTED  0
#define STATE_CONNECTING    1<<0
#define STATE_CONNECTED     1<<1
#define STATE_DONE          1<<2
#define STATE_ERROR         1<<3
#define STATE_SESSION       1<<4
#define STATE_QUIT          1<<5
#define STATE_TIMEOUT       1<<6
#define STATE_WRITE         1<<7
#define STATE_READ          1<<8

#include <pthread.h>

/*
 *
 *		misc define
 *
 */

#pragma pack(1)

#endif /* INCLUDE_H_ */

//отправлять одни пакетом
#define ONEPACKET

#ifndef DEBUG
#define DEBUG
#define HEXPRINT
#endif

#define MODULE_COUNT        1024
#define OR                  ||
#define AND                 &&
#define or                  OR
#define and                 AND
#define cBLUE               "\e[1;34m"
#define cYELLOW             "\e[1;33m"
#define cGREEN              "\e[1;32m"
#define cRED                "\e[1;31m"
#define cEND                "\e[0m"
#define BUFLEN              1024
#define TTL                 3600
#define TRUE                1
#define FALSE               0
#define KEYLEN              128
#define MAX_UNCMPR          1024*1024*5
#define MAX_CMPR            1024*512
//hack (:
#define u8                  u_int8_t
#define u16                 u_int16_t
#define u32                 u_int32_t
#define u64                 u_int64_t
#ifdef __x86_64__
#define PLATFORM            "64"
#else
#define PLATFORM            "32"
#endif


enum {
	RESULT_NOTFOUND,RESULT_FOUND,RESULT_ERROR
};
enum {
	DNS_SUBTASK, DNS_RESOLV, DNS_TASK, DNS_GETNS
};
enum {
	TASK_DNS
};

//89261500929
#define MSToTimeval(ms,tv)         timerclear(&tv);\
        if (ms > 1000) {\
            tv.tv_sec = ms / 1000 / 2;\
            tv.tv_usec = (ms % 1000)*1000 / 2;\
        } else {\
            tv.tv_sec = 0;\
            tv.tv_usec = ms * 1000 / 2;\
        }

struct nv {
	const char *name;
	int value;
};

struct cond_wait {
	pthread_mutex_t lock;
	pthread_cond_t cond;
};

union {
	unsigned char * as_uchar;
	char * as_char;
} name;


struct Task {
	void (*callback)(struct Task *);
	struct Request *Req;
	int returnCode;
	ares_channel channel;
	int taskType;
	struct event ev;
	int fd;
	json *child;
};

struct Request {
	struct evhttp_request *evreq;
	int countTask;
	void (*callback)(struct Request *);
	char *taskDomain;
        struct event ev;
	json *root;
};


