#define BLOCK 1024
#define NAME_SIZE 30
#define MAX_MSG_SIZE 150


#define CHATD_VERSION "0.01 alpha"

#define dPORT "31337"
#define dIP "0.0.0.0"
#define dFILE "./chatd.conf"
#define dKEY "123456789123456"

#define dDB_HOST "localhost"
#define dDB_USER "root"
#define dDB_PASS "teamsql"
#define dDB_NAME "game"

#define BUFFSIZE 1024*4
#define MAXLINE 1024
#define NAME_SIZE 30

int MASTER_KEY;
char * LISTEN_IP;
int LISTEN_PORT;

char *DB_USER;
char *DB_HOST;
char *DB_NAME;
char *DB_PASS;


#include "include.h"
#if defined  NETH
#include "net.h"
#endif

#if defined  DEFINE
#include "define.h"
#endif



//EOF

