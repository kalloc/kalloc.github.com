#include <stdio.h>
#include <mysql/mysql.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <signal.h>
#include <unistd.h>
#include <fastcgi.h>
#include <fcgi_stdio.h>

#include <iostream>
#include <list>
#include <vector>
#include <map>
#include <string>

using namespace std;

#include "./sis_var.inc.cpp"

vector<std::string> ParseQueryString(std::string);
char * genSQL(char *,...);

MYSQL mysql;
MYSQL_RES *result;
MYSQL_ROW row;

#define NOT_FOUND -1 
#define INVALID_NUMBER -2

#define SQL_CAT "SELECT cat FROM top_main WHERE id=?"

// Аварийное завершение программы по тайм-ауту
void timeOut(int nsig) {
	mysql_close(&mysql);
	printf("Content-type: text/html\n\n");
	printf("Server overloaded...");
	exit(0);
}

// Макрос обработки ошибок
#define exiterr(errcode) {                                  \
    printf("Location: /\nContent-type: text/html\n\n");                  \
    switch(errcode) {                                       \
        case -1: printf("Count not found."); break;         \
        case -2: printf("Invalid number."); break;          \
        default: printf("System error, code #%d.",errcode); \
    };                                                      \
}                                                           \
continue 

int main(int argc,char **argv) 
{
    vector <string> query;
    
    // Установка таймаута на завершение программы, сек
    alarm(256);
    signal(SIGALRM, timeOut);
    
    int mysql_inited = 0;
    const char *id;
    
    while(FCGI_Accept() >= 0) {
        if(!mysql_inited) {
            mysql_init(&mysql);
            if (!(mysql_real_connect(&mysql, mysqlhost , mysqluser , mysqlpassword, mysqldb, mysqlport, NULL, 0))) {
                exiterr(1);
            }
            mysql_inited = 1; 
        }
        // Получение данных из строки запроса
        string queryString = getenv("QUERY_STRING");
        if (queryString.empty()) {
            exiterr(-2);
        }
        query = ParseQueryString(queryString);
        if (query.empty()) {
            exiterr(-2);
        }

        id = query[0].c_str();

        if (mysql_query(&mysql, genSQL(SQL_CAT,id))) {
            exiterr(2);
        }
        if (!(result = mysql_store_result(&mysql))) {
            exiterr(3);
        }
        if (mysql_num_rows(result) == 0) {
            exiterr(-1);
        }
        if (!(row = mysql_fetch_row(result))) {
            exiterr(4);
        }

        printf("Location: %scat_%s\nContent-type: text/html\n\n", siteurl, row[0]);
        mysql_free_result(result);

    }
    if (mysql_inited) {
        mysql_close(&mysql);
    }
    return 0; 
}
