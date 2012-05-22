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


#define prce(check)       pcre *f;int errchar;const char *errstr;char *pattern=check; \
    if((f=pcre_compile(pattern,PCRE_CASELESS|PCRE_MULTILINE,&errstr,&errchar,NULL))==NULL)    return 0; \
    if((pcre_exec(f,NULL,str,strlen(str),0,PCRE_NOTEMPTY,0,0))<0)  return 0;	\
    return 1

char buf[BLOCK+1];
int buflen;



int rx_check_login(char *str) {
    strip(str);    
    if(strlen(str) > 0) return TRUE; 
    return FALSE;
}

int rx_check_password(char *str) {
    prce("^[0-9-_a-z]{8,}$"); 
}

int rx_check_mail(char *str) {
    prce("^[0-9\\-_\\.a-z]+@[0-9\\-_\\.a-z]+\\.[a-z]{2,6}$"); 
}


int rx_check_name(char *str) {
    if(escape(str,128) && strlen(str) > 0) return TRUE; 
    return FALSE;
}

int rx_check_country(char *str) {
    if(escape(str,64) && strlen(str) > 0) return TRUE; 
    return FALSE;
}

int rx_check_city(char *str) {
    if(escape(str,64) && strlen(str) > 0) return TRUE; 
    return FALSE;
}

int rx_check_gender(char *str) {
    if(!isdig(str))  { return FALSE; }
    int gender=atoi(str);
    if(gender == 0 || gender > 3) return FALSE;
    return TRUE;
}

int rx_check_byear(char *str) {
    int year;
													  
    if(!isdig(str))  { return FALSE; }
    year=atoi(str);
    if(year < 1900  || year > get_Year()) return FALSE;
    
    return TRUE;
}

int rx_check_bmonth(char *str) {
    int month;

    if(!isdig(str))  { return FALSE; }
    month=atoi(str);
    if(month < 1  || month > 12) return FALSE;

    return TRUE;
}

int rx_check_bday(char *str) {
    int day;
    if(!isdig(str))  { return FALSE; }
    day=atoi(str);
    if(day < 1  || day > 31) return FALSE;
    return TRUE;
}

int rx_check_race(char *str) {
    if(!isdig(str)) return FALSE;
    return check_kingdom(atoi(str));
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

