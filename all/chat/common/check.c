#ifdef BUILD
#define NETH
#define DEFINE
#include "var.h"
#else
    #define TRUE 1
    #define FALSE 0
    #define BLOCK 1024+1
#endif
#include <iconv.h>
#include <stdio.h>
#include <time.h>
#include <pcre.h>

#define memscpy(static)	 memcpy(buf+buflen,static,sizeof(static)-1);buflen+=sizeof(static)-1
#define prce(check)       pcre *f;int errchar;const char *errstr;char *pattern=check; \
    if((f=pcre_compile(pattern,PCRE_CASELESS|PCRE_MULTILINE,&errstr,&errchar,NULL))==NULL)    return 0; \
    if((pcre_exec(f,NULL,str,strlen(str),0,PCRE_NOTEMPTY,0,0))<0)  return 0;	\
    return 1

char buf[BLOCK+1];
int buflen;

time_t time_cur;
struct tm *tm_cur;


void strip(char *str) {
    buflen=0;
    char *ptr;
    ptr=str;

    while(*(ptr) != 0) {
        switch (*ptr) {
            case '"':
                break;
            case '\'':
                break;
            case '<':
                break;
            case '>':
                break;
            default:
                buf[buflen++]=*(ptr);
                break;
        }
	ptr++;
    }
    memcpy(str,buf,buflen);
    str[buflen]=0;
}

int escape(char *str,int maxline) {
    buflen=0;
    char *ptr;
    ptr=str;

    while(*(ptr) != 0 && buflen < maxline) {
        switch (*ptr) {
            case '"':
		memscpy("&quot;");
                break;
            case '\'':
		memscpy("&quot;");
                break;
            case '<':
		memscpy("&lt;");
                break;
            case '>':
		memscpy("&gt;");
                break;
            default:
                buf[buflen++]=*(ptr);
                break;
        }
	ptr++;
    }
    if(buflen >= maxline) return FALSE;
    memcpy(str,buf,buflen);
    str[buflen]=0;
    return TRUE;
}

int isdig(char *str) {

    while(*(str) != 0) {
	if(*(str) < '0' || *(str) > '9') return FALSE;
	str++;
    }
    
    return TRUE;


}


void convert(char *str) {


  iconv_t cd;
  size_t k, f, t;
  char* in = str;
  char* out = buf;

  cd = iconv_open("cp1251", "utf-8");
  f = strlen(str);
  t = sizeof buf;
  memset( &buf, 0, sizeof buf );
  k = iconv(cd, &in, &f, &out, &t);
  iconv_close(cd);
  bzero(str,BLOCK);
  strcpy(str,buf);


}

int get_Year() {

    time_cur=time(NULL);
    tm_cur = localtime(&time_cur);
    return tm_cur->tm_year+1900;
}




#ifndef BUILD
int main() {

    int i=0;
    char str[]="aba\"'<>babba";
    escape(str,sizeof(str));

//    printf("%s\n",str);

    return 0;
}
#endif

//EOF

