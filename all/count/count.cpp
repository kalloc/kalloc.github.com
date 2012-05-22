#include <Magick++.h>
#include <stdio.h> //printf
#include <mysql/mysql.h>
#include <stdlib.h>
#include <stdarg.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <fastcgi.h>
#include <fcgi_stdio.h>



#include <iostream>
#include <list>
#include <vector>
#include <map>
#include <string>

using namespace std;
using namespace Magick;


#include "./sis_var.inc.cpp"

int random_gen();
vector<std::string> ParseQueryString(std::string);
map<std::string,std::string> ParseCookieString(std::string);
char * genSQL(char *,...);
char * get_current_time();

MYSQL mysql;
MYSQL_RES *result;
MYSQL_ROW row;

#define NOT_FOUND -1 
#define INVALID_NUMBER -2
#define PNG_HEADER "Content-type: image/png\nPragma: no-cache\nCache-control: no-cache\n\n"

#define SQL_SITE "SELECT SQL_NO_CACHE hits,hosts,hosts_y,users,hits_all,hosts_all,users_all,num_count FROM top_main WHERE id=?"
#define SQL_TOP_USER_TODAY "SELECT SQL_NO_CACHE * FROM top_users_today WHERE id=? AND us_id=?"
#define SQL_IP_TODAY "SELECT SQL_NO_CACHE * FROM top_ip_today WHERE id=? AND ip=?"
#define SQL_INSERT_USER_TODAY "INSERT INTO top_users_today(id,us_id) VALUES(?, ?)"
#define SQL_INSERT_IP_TODAY "INSERT INTO top_ip_today(id,ip) VALUES(?, ?)"
#define SQL_UPDATE_SITE "UPDATE top_main SET hits = ?, hosts = ?, users = ?, hits_all = ?, hosts_all = ?, users_all = ? WHERE id=?"

// Аварийное завершение программы по тайм-ауту
void timeOut(int nsig) {
    mysql_close(&mysql);
    printf("Content-type: text/html\n\n");
    printf("Server overloaded...");
    exit(0);
};


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
    Blob blob;

    map<string,string> cookieMap;
    map<string,Image> ImageMap;
    map<string,Image>::iterator it;
    
    

    // Установка таймаута на завершение программы, сек
    alarm(256);
    signal(SIGALRM, timeOut);
    int mysql_inited = 0;
    int new_user;
    int new_hit;
    int users;
    int users_all;
    int hosts;
    int hosts_all;
#ifdef DEBUG        
    int counter = 0;
    char ccounter[80]; 
#endif
    char chits[20];
    char chits_all[20];
    char cusers[20];
    char cusers_all[20];
    char chosts[20];
    char chosts_all[20];
    char cookie_id[32];
        
    const char * VID, *id, *ctype;
    char * time, *ip_address;


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
        ctype = query[1].c_str();
        
        if (mysql_query(&mysql, genSQL(SQL_SITE,id))) {
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


        if (query[1].empty()) {
            ctype = "0";
        }        
        char filename[10];
        sprintf(filename, "%s_%s", ctype, row[7]);

        string pngTemplate = filename;
        //string pngTemplate = row[7];
        users = atoi(row[3]);
        users_all = atoi(row[6]);
        hosts = atoi(row[1]);
        hosts_all = atoi(row[5]);

        snprintf(chits, sizeof(chits), "%d", atoi(row[0])+1);
        snprintf(chits_all, sizeof(chits_all), "%d", atoi(row[4])+1);
        
    
        mysql_free_result(result);

        new_user = 0;

        if(getenv("HTTP_COOKIE")) {
            string cookieString = getenv("HTTP_COOKIE");
            cookieMap = ParseCookieString(cookieString);
        }
        
        if(cookieMap.find("VID") != cookieMap.end()){
            VID = cookieMap.find("VID")->second.c_str();
            if (mysql_query(&mysql, genSQL(SQL_TOP_USER_TODAY,id,VID))) {
                exiterr(6);
            }
        
            if (!(result = mysql_store_result(&mysql))) {
                exiterr(7);
            }
        
            if (mysql_num_rows(result) == 0) {
                if (mysql_query(&mysql, genSQL(SQL_INSERT_USER_TODAY, id, VID))) {
                    exiterr(8);
                }
                new_user = 1;
            }
        
        } else {
            // Установка куки пользователю
            snprintf(cookie_id,32,"%d",random_gen());
            time = get_current_time();
            printf("Set-Cookie:VID=%s; expires=%s\n", cookie_id, time);
            if (mysql_query(&mysql, genSQL(SQL_INSERT_USER_TODAY,id, cookie_id))) {
                exiterr(5);
            }
            new_user = 1;
        }

        ip_address = getenv("REMOTE_ADDR");

  		  if (mysql_query(&mysql, genSQL(SQL_IP_TODAY, id, ip_address))) {
            exiterr(10);
        }

        if (!(result = mysql_store_result(&mysql))) {
            exiterr(11);
        }
  
        if (mysql_num_rows(result) == 0) {
            if (mysql_query(&mysql, genSQL(SQL_INSERT_IP_TODAY, id, ip_address))) {
                exiterr(12);
            }        
            hosts++;
            hosts_all++;
        }
        else {
            new_user = 0;
        }

        if (new_user == 1) {
            users++;
            users_all++;
        }

        snprintf(cusers, sizeof(users), "%d", users);
        snprintf(cusers_all, sizeof(users_all), "%d", users_all);
        snprintf(chosts, sizeof(hosts), "%d", hosts);
        snprintf(chosts_all, sizeof(hosts_all), "%d", hosts_all);

        if(ImageMap.end() == ImageMap.find(pngTemplate)) {
            try {
                Image newImage( imgpach + pngTemplate + ".png"); 
                newImage.label( "Annotate" );
                newImage.fontPointsize( 10 );
                newImage.font( "@01.ttf" );
                newImage.strokeColor( Color() );
                newImage.fillColor( "black" );
                ImageMap.insert(pair<string,Image>(pngTemplate,newImage));
            }  catch( ... )  {
                exiterr(13);
            }
#ifdef DEBUG        
            printf ("pngTemplate: INIT %s\n",pngTemplate.c_str());
        } else {
            printf ("pngTemplate: FOUND %s\n",pngTemplate.c_str());
#endif
        }

        if (mysql_query(&mysql, genSQL(SQL_UPDATE_SITE, chits, chosts, cusers, chits_all, chosts_all, cusers_all, id))) {
            exiterr(5);
        }

        Image image = ImageMap.find(pngTemplate)->second;

        switch (atoi(ctype)) {
            case 0:
                image.annotate( chits_all, "+1+1", NorthEastGravity );
                image.annotate( chits, "+1+10", NorthEastGravity );
                image.annotate( cusers, "+1+19", NorthEastGravity );        
            break;
            case 1:
                image.annotate( chits_all, "+1+2", NorthEastGravity );
            break;
            case 2:
                image.annotate( chits, "+5+33", NorthEastGravity );
                image.annotate( chits_all, "+5+42", NorthEastGravity );
                image.annotate( chosts, "+5+54", NorthEastGravity );
                image.annotate( chosts_all, "+5+63", NorthEastGravity );
                image.annotate( cusers, "+5+76", NorthEastGravity );
                image.annotate( cusers_all, "+5+84", NorthEastGravity );
            break;
            default: exiterr(9);
        }
        
#ifdef DEBUG        
        snprintf(ccounter, sizeof(ccounter), "%d",counter++);
        image.annotate( ccounter, "+40+1", NorthEastGravity );
#endif
        image.write(&blob,"png");
        printf(PNG_HEADER);
        fwrite(( void *)blob.data(), blob.length(), 1, FCGI_stdout);
        
    }
    if (mysql_inited) {
        mysql_close(&mysql);
    }
    return 0; 
}
