#include "var.h"

#define HAVE_LOG_TIME

static void err_doit(int, const char *, va_list);
char *pname=NULL;
time_t t;


void err_sys(const char *fmt, ...)
{
    va_list ap;
    va_start(ap,fmt);
    err_doit(1,fmt,ap);
    va_end(ap);
    exit(1);
}


void log_sys(const char *fmt, ...)
{
     char buf[MAXLINE];
    va_list ap;
    va_start(ap,fmt);
    vsnprintf(buf,MAXLINE-1,fmt,ap);
    fflush(stdout);
    
#ifdef HAVE_LOG_TIME
        char buf_end[MAXLINE];
	char * log_string;
     	log_string=malloc(MAXLINE);
	time(&t);
	strftime(log_string,100,"[%Y/%m/%d %H:%M] ",localtime(&t));
	strcpy(buf_end,buf);
	snprintf(buf,MAXLINE-1,"%s%s",log_string,buf_end);
	free(log_string);
#endif

    fputs(buf,stdout);
    fflush(NULL);
    va_end(ap);
    return;
}

void err_quit(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    err_doit(0,fmt,ap);
    va_end(ap);
    exit(1);
}

static void err_doit(int errnoflag, const char *fmt, va_list ap)
{
    int errno_save;
    char buf[MAXLINE];
    errno_save=errno;
    vsnprintf(buf,MAXLINE-1,fmt,ap);
    if(errnoflag)
    snprintf(buf+strlen(buf),MAXLINE-1, ": %s", strerror(errno_save));
    strcat(buf, "\nexit....\n");
    fflush(stdout);
    fputs(buf,stderr);
    fflush(NULL);
    return;
}



