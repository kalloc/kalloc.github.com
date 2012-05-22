#include "include.h"
#include <ctype.h>
struct timeval tv;
FILE * fp = 0;


//Authorization

// DNS
static const struct nv classes[] = {
    { "IN", C_IN},
    { "CHAOS", C_CHAOS},
    { "HS", C_HS},
    { "ANY", C_ANY},
};
static const int nclasses = sizeof (classes) / sizeof (classes[0]);

static const struct nv types[] = {
    { "A", T_A},
    { "NS", T_NS},
    { "MD", T_MD},
    { "MF", T_MF},
    { "CNAME", T_CNAME},
    { "SOA", T_SOA},
    { "MB", T_MB},
    { "MG", T_MG},
    { "MR", T_MR},
    { "NULL", T_NULL},
    { "WKS", T_WKS},
    { "PTR", T_PTR},
    { "HINFO", T_HINFO},
    { "MINFO", T_MINFO},
    { "MX", T_MX},
    { "TXT", T_TXT},
    { "RP", T_RP},
    { "AFSDB", T_AFSDB},
    { "X25", T_X25},
    { "ISDN", T_ISDN},
    { "RT", T_RT},
    { "NSAP", T_NSAP},
    { "NSAP_PTR", T_NSAP_PTR},
    { "SIG", T_SIG},
    { "KEY", T_KEY},
    { "PX", T_PX},
    { "GPOS", T_GPOS},
    { "AAAA", T_AAAA},
    { "LOC", T_LOC},
    { "SRV", T_SRV},
    { "AXFR", T_AXFR},
    { "MAILB", T_MAILB},
    { "MAILA", T_MAILA},
    { "NAPTR", T_NAPTR},
    { "ANY", T_ANY}
};
static const int ntypes = sizeof (types) / sizeof (types[0]);

const char *type_name(int type) {
    int i;

    for (i = 0; i < ntypes; i++) {
        if (types[i].value == type)
            return types[i].name;
    }
    return "(unknown)";
}

const char *class_name(int dnsclass) {
    int i;

    for (i = 0; i < nclasses; i++) {
        if (classes[i].value == dnsclass)
            return classes[i].name;
    }
    return "(unknown)";
}

inline const unsigned char *skip_question(const unsigned char *aptr, const unsigned char *abuf, int alen) {
    char *name;
    int status;
    long len;
    status = ares_expand_name(aptr, abuf, alen, &name, &len);

    if (status != ARES_SUCCESS) return NULL;
    aptr += len;
    if (aptr + QFIXEDSZ > abuf + alen) {
        ares_free_string(name);
        return NULL;
    }
    aptr += QFIXEDSZ;
    ares_free_string(name);
    return aptr;
}

inline int getDNSTypeAfterParseAnswer(const unsigned char *aptr, const unsigned char *abuf, int alen) {
    int type, dnsclass, ttl, dlen, status;
    long len;

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
    return type;
}

inline const u32 getResolvedIPAfterParseAnswer(const unsigned char *aptr, const unsigned char *abuf, int alen) {
    int type, dnsclass, ttl, dlen, status;
    long len;

    union {
        unsigned char * as_uchar;
        char * as_char;
    } name;

    /* Parse the RR name. */
    status = ares_expand_name(aptr, abuf, alen, &name.as_char, &len);
    if (status != ARES_SUCCESS)
        return 0;
    aptr += len;

    /* Make sure there is enough data after the RR name for the fixed
     * part of the RR.
     */
    if (aptr + RRFIXEDSZ > abuf + alen) {
        ares_free_string(name.as_char);
        return 0;
    }

    /* Parse the fixed part of the RR, and advance to the RR data
     * field. */
    type = DNS_RR_TYPE(aptr);
    dnsclass = DNS_RR_CLASS(aptr);
    ttl = DNS_RR_TTL(aptr);
    dlen = DNS_RR_LEN(aptr);
    aptr += RRFIXEDSZ;
    if (aptr + dlen > abuf + alen) {
        ares_free_string(name.as_char);
        return 0;
    }
    hexPrint(abuf, alen);
    ares_free_string(name.as_char);

    if (type != T_A and type != T_NS) return 0;
    hexPrint(aptr, dlen);
    if (dlen != 4)
        return 0;
    return *(const u32 *) aptr;
    /*
        printf("\t%s\n", inet_ntop(AF_INET, aptr, addr, sizeof (addr)));
        hexPrint(addr, sizeof (addr));
        return addr;
     */
}


unsigned StringToDNSType(char **str) {
	//	NS -> 0x534e
	if(*(u16 *) *str == 0x534e) {
		*str+=1;
		return T_NS;
	}
	//	SOA -> 0x414f53
	else if(*(u32 *) *str == 0x414f53) {
		*str+=2;
		return T_SOA;
	}
	//	SRV -> 0x565253
	else if(*(u32 *) *str == 0x565253) {
		*str+=2;
		return T_SRV;
	}
//	//	WHOIS -> 494f4857
//	else if(*(u32 *) *str == 0x494f4857) {
//		*str+=4;
//		return T_WHOIS;
//	}
	//	A -> 0x41
	else if(*(u8 *) *str == 0x41) {
		return T_A;
	}
	//	TXT -> 00545854
	else if(*(u32 *) *str == 0x545854) {
		*str+=2;
		return T_TXT;
	}
	//	CNAME -> 4d414e43
	else if(*(u32 *) *str == 0x4d414e43) {
		*str+=4;
		return T_CNAME;
	} else {
		return 0;
	}
}




//Time

int timeDiffMS(struct timeval end, struct timeval start) {
    int time = (int) ((((double) end.tv_sec + (double) end.tv_usec / 1000000) - ((double) start.tv_sec + (double) start.tv_usec / 1000000))*1000);
    return time > 0x7ffe ? -1 : time;
}

int timeDiffUS(struct timeval end, struct timeval start) {
    return (int) ((((double) end.tv_sec + (double) end.tv_usec / 1000000) - ((double) start.tv_sec + (double) start.tv_usec / 1000000))*1000000);
}

double get_timeval_delta(struct timeval *after, struct timeval *before) {
    return ((double) after ->tv_usec - (double) before->tv_usec);
}


//Memory

inline void *getNulledMemory(int size) {
    /*
        void *ptr;
        ptr = malloc(size);
        bzero(ptr, size);
        return ptr;
     */
    return calloc(1, size);
}


//Loger

void loger(char *codefile, char *codefunction, int level, const char *fmt, ...) {
//#define STDOUT
    static pthread_mutex_t *mutex = NULL;
    if (getLogLevel() & (level)) {
        if (!mutex) {
            mutex = calloc(1, sizeof (*mutex));
            pthread_mutex_init(mutex, NULL);
        }
        pthread_mutex_lock(mutex);

        int len, second, hour;
        char *buf, *file, *date;
        struct timeval tv;
        gettimeofday(&tv, NULL);
        buf = getNulledMemory(4096);
        if (fp == 0) {
            struct tm *tm;
            time_t result;
            result = time(NULL);
            file = getNulledMemory(4096);
            tm = localtime(&result);
            snprintf(file, 4096, getLogPath(), 1900 + tm->tm_year, tm->tm_mon, tm->tm_mday, tm->tm_hour, tm->tm_min);
            fp = fopen(file, "a");
            free(file);
        }
        if (!fp) return;
        second = tv.tv_sec - tv.tv_sec / 3600 * 3600;
        hour = tv.tv_sec / 3600 - (tv.tv_sec / 3600 / 24)*24;
        len = snprintf(buf, 4096, "[%02d:%02d:%02d %d] [%s%s%s%s] %s%s%s%s",
                hour, second / 60, second % 60, tv.tv_sec,
                level == LOG_DEBUG ? "DEBUG" : "",
                level == LOG_NOTICE ? "NOTICE" : "",
                level == LOG_INFO ? "INFO" : "",
                level == LOG_WARN ? "WARN" : "",
                codefile ? codefile : "",
                codefunction ? ":" : "",
                codefunction ? codefunction : "",
                codefunction ? "() " : ""
                );
        fwrite(buf, 1, len, fp);
#ifdef STDOUT
        printf(buf);
#endif
        va_list ap;
        va_start(ap, fmt);
        len = vsnprintf(buf, 4094, fmt, ap);
        buf[len] = '\n';
        buf[len + 1] = 0;
        fwrite(buf, 1, len + 1, fp);
        fflush(fp);
        va_end(ap);
#ifdef STDOUT
        printf(buf);
#endif
        free(buf);
        pthread_mutex_unlock(mutex);
    }
    
}


//Reload

void restart_handler(int signum) {
    if (fp != 0) fclose(fp);
    fp = 0;
}


//Network
void strtolower(char *str) {
    for (; *str; str++) {
        if (*str >= 'A' && *str <= 'Z') {
            *str += 32;
        } else if (*str >= 192 && *str <= 223) {
            *str += 32;
        }
    }
}

char * ipString(u32 net) {
    char *ptr;
    struct in_addr in;
    in.s_addr = (in_addr_t) net;
    ptr = inet_ntoa(in);
    return ptr ? ptr : "UNKNOW";
}

char * getActionText(int code) {
    static char buf[100];
    snprintf(buf, 100, "%s%s%s [%x]",
            code & EV_READ ? "EV_READ " : "",
            code & EV_WRITE ? "EV_WRITE " : "",
            code & EV_TIMEOUT ? "EV_TIMEOUT " : "", code
            );
    return buf;
}

char * getRoleText(int code) {
    switch (code) {
        case DNS_SUBTASK:
            return "DNS_SUBTASK";
        case DNS_RESOLV:
            return "DNS_RESOLV";
        case DNS_TASK:
            return "DNS_TASK";
        case DNS_GETNS:
            return "DNS_GETNS";
        default:
            return "UNKNOW";
    }
}

unsigned char * bin2hex(unsigned char *bin, int len) {
    static u_char hex[BUFLEN];
    bzero(&hex, BUFLEN);
    len -= 1;
    u_char left = 0, right = 0;
    hex[len * 2 + 2] = 0;
    for (; len >= 0; len--) {
        left = bin[len] >> 4;
        right = bin[len]-(bin[len] >> 4 << 4);

        if (right >= 0 and right <= 9) {
            hex[len * 2 + 1] = right + 48;
        } else {
            hex[len * 2 + 1] = right + 87;
        }
        if (left >= 0 and left <= 9) {
            hex[len * 2] = left + 48;
        } else {
            hex[len * 2] = left + 87;
        }
    }
    return hex;

}

void hexPrint(char *inPtr, int inLen) {
#define LINE_LEN 16
    int sOffset = 0;
    u16 Index = 0;
    for (Index = 0; inLen > 0; inLen -= LINE_LEN, sOffset += LINE_LEN) {

        printf("  %05d: ", sOffset);

        for (Index = 0; Index < LINE_LEN; Index++) {
            if (Index < inLen) {
                printf("%02x ", (unsigned char) inPtr[Index + sOffset]);
            } else {
                printf("   ");
            }
        }

        printf(" : ");

        for (Index = 0; Index < LINE_LEN; Index++) {
            char byte = ' ';

            if (Index < inLen) {
                byte = inPtr[Index + sOffset];
            }
            printf("%c", (((byte & 0x80) == 0) && isprint(byte)) ? byte : '.');
        }

        printf("\n");

    }
}
