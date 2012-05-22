#include <stdio.h> //printf
#include <mysql/mysql.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <fastcgi.h>

#include <iostream>
#include <list>
#include <map>
#include <vector>
#include <string>

using namespace std;

vector<std::string> ParseQueryString(std::string);

// Функция подсчета символов в строке
int countSymbol(char* string, char symbol) {
    int count = 0;
    while(*string) {
        if(*string++ == symbol) count++;
    };
    return count;
};
#define BUFFER_LENGTH 4095
#define BLANK ""
char * genSQL(char * fmt,...) {
    static char buf [BUFFER_LENGTH+1];
    char *name, *value;
    char * ptr = fmt;
    int buflen = 0,len=0;
    va_list ap;
    va_start(ap, fmt);
    while( ( ptr = strchr(ptr,'?')) != NULL) {
        name = va_arg(ap, char *);
        if(2+(ptr-fmt)+buflen >= BUFFER_LENGTH) {
            return BLANK;
        }
        memcpy(buf+buflen,fmt,ptr-fmt);
        buflen+=ptr-fmt;
        fmt=++ptr;
        buf[buflen++]='\'';
        for(;*name;name++) {
            if(buflen >= BUFFER_LENGTH) {
                return BLANK;
            }
            if(*name == '\'' || *name == '\\') {
                buf[buflen++]='\\';
            }
            buf[buflen++]=*name;

        }
        buf[buflen++]='\'';
    }
    for(;*fmt;fmt++) {
        if(buflen >= BUFFER_LENGTH) {
            return BLANK;
        }
        buf[buflen++]=*fmt;
    }

    buf[buflen] = 0;
    va_end(ap);
    return buf;
}

map<std::string,std::string> ParseCookieString(std::string In) {
    map<std::string, std::string> ret;
    string strTmp = In;
    string amp ("&");
    string eq ("=");
    vector<std::string>::iterator it;
    vector<std::string> kv = ParseQueryString(In);
    for ( it=kv.begin() ; it != kv.end(); it++ ) {
        size_t found;
        string cookie = *it;
        found=cookie.find_first_not_of(" ");
        cookie = cookie.substr (found);
        found = cookie.find("=");
        string key = cookie.substr(0,found);
        string value = cookie.substr(found+1);
        ret[key]=value;
    }
    return ret;
}

//Функция генерации случайного числа
int random_gen() {
    int rnd;
    srand(time(NULL));
    rnd = rand();
    return rnd;
}

char * get_current_time() {
    struct tm *when;
    time_t now;
    static char str[80];
    time( &now );
    when = localtime( &now );
    when->tm_mday = when->tm_mday + 1;
    mktime( when );

    strftime(str, 100, "%a, %d %b %Y 00:00:01 GMT", when);
    return str;
}

vector<std::string> ParseQueryString(std::string In) {
    vector<std::string> ret;
    string strTmp = In;
    string Key (";");

    size_t i = 0;
    while (i < strTmp.length()) {
        int old_i = i;
        i = strTmp.find(Key, i);
        if(i != string::npos) {
            ret.push_back(strTmp.substr(old_i, i - old_i));
        } else {
            ret.push_back(strTmp.substr(old_i));
            break;
        }
        i += Key.length();
    }
    return ret;
}
