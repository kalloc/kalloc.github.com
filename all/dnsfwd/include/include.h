#include <time.h>
#include <search.h>
#include <string.h>
#include <signal.h>
#include <sys/queue.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/resource.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/queue.h>
#include <unistd.h>
#include <sys/time.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <netdb.h>
#include <memory.h>
#include <math.h>
#include <getopt.h>

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

int verbose_flag,id;





//main.c
void net_loop(int i,short sh,void *vd);
//network.c
int net_init_server();
int net_init_client();
//EOF

