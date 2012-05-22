#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <netdb.h>
#include <signal.h>
#include <fcntl.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <memory.h>
#include <math.h>
#include <getopt.h>
#include <search.h>

#include <event.h>


#define FALSE 0
#define TRUE 1

#define error(...) printf ("!! func: %-20s (%10s:%d): ",__FUNCTION__,__FILE__,__LINE__); \
		   printf(__VA_ARGS__); \
	           printf("\n"); exit(0)

#ifdef DEBUG

#ifdef DEBUG_EXT
#define print(...) printf ("func: %-20s (%10s:%d): ",__FUNCTION__,__FILE__,__LINE__); \
		   printf(__VA_ARGS__); \
	           printf("\n")
#else
#define print(...) printf ("func: %-20s: ",__FUNCTION__); \
		   printf(__VA_ARGS__); \
	           printf("\n")
#endif
#else
#define print(...)  
#endif


#define mt_rand(s1,s2) (s1 + rand() % (s2+1-s1))




//global   
int verbose_flag;
unsigned int uid;
unsigned int sid;
int  id;
unsigned int pid;




//debug.c
void err_sys(const char *,...);
void log_sys(const char *,...);
void err_quit(const char *, ...);


//hash.c

struct nlist{struct nlist *next;char *name;char *value;};
int install(struct nlist *hashtab[],char *name,char *value);
struct nlist *lookup(struct nlist *hashtab[],char *s);
unsigned hash (char *namearg);

//read_conf.c
void print_config(int depth,struct nlist *np);
void init_config(int argc , char **argv);
void show_top();
void show_end();
char * get_config(char *);


//chatd/chatd_users.c
void get_user_list();
int add_users(char *,char *);
void init_users();
int del_users();
int check_access(unsigned int);


//chatd/chatd_main.c
void net_loop();

//network.c
int net_init_server();
int net_init_client();
int join_users(int);
//int user_status(int);
int check_place();
void get_place_list();
int chat_add_msg(char *);
int chat_get_msg();


//chat_mysql.c
void db_init();
void db_reinit();
int db_check_account (char *);
int db_check_access (char *,char *);
int db_reg_user (char *,char *,char *,char *,char *,char *,char *,char *,char *,char *,char *);
int user_status(unsigned int);
int chat_add_pvt(char *);
int escape(char *,int);

//define db account
typedef struct {
        unsigned int aID;
        char aLogin[NAME_SIZE+1];
        char aPassword[NAME_SIZE+1];
        char aEmail[100+1];
        char aName[128+1];
        char aCountry[64+1];
        char aCity[64+1];
        short int aGender;
        char  aHB[10+1];
        short int aStatus;
        short int aRace;
        char aCode[64+1];
} db_account_struct_def;

//define db characters
typedef struct {
        unsigned int aID;
        unsigned int cID;
	unsigned int cProf;
        char cLogin[NAME_SIZE+1];
} db_char_struct_def;

db_char_struct_def db_char;
db_account_struct_def db_account;


//chatd_main.c
int uptime();

//chat_common.c:
void error_404();
void error_500();
void cgiWrite(char *,int);
void cgiTop();
void cgiDown();

//verify_link.c
char * getMail ();
char * getHash ();
char * genLink (char *,char *);
char * getLink ();
char * getLogin ();

//mail.c
void mail_pre_reg(char *,char *,char *);
void mail_reg(char *,char *,char *);

//chat_check.c
int rx_check_login(char *);
int rx_check_password(char *);
int rx_check_mail(char *);
int rx_check_name(char *);
int rx_check_country(char *);
int rx_check_city(char *);
int rx_check_gender(char *);
int rx_check_byear(char *);
int rx_check_bmonth(char *);
int rx_check_bday(char *);
int rx_check_race(char *);


//check.c
void strip(char *);
int escape(char *,int);
int isdig(char *);
void convert(char *);
int get_Year();


//chat_classes.c
int check_kingdom(char);

//EOF

