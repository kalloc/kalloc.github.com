#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <sys/time.h>
#include <sys/resource.h>


#define MAXBUF 1024
#define MAXEPOLLSIZE 10000


int listener, new_fd, kdpfd, nfds, n, ret, curfds;

socklen_t len;
struct sockaddr_in my_addr, their_addr;
unsigned int myport, lisnum;
struct rlimit rt;

struct epoll_event ev;
struct epoll_event events[MAXEPOLLSIZE];

char out[]=	"HTTP/1.1 200 OK\r\n"
		"Server: nginx/0.5.35\r\n"
    "Content-Type: text/html\r\n"
    "Content-Length: 151\r\n"
    "Connection: keep-alive\r\n"
    "Accept-Ranges: bytes\r\n"
    "\r\n"
    "<html>"
    "<head>"
    "<title>Welcome to nginx!</title>"
    "</head>"
    "<body bgcolor=\"white\" text=\"black\">"
    "<center><h1>Welcome to nginx!</h1></center>"
    "</body>"
    "</html>\r\n\r\n";



int setnonblocking(int sockfd)
{
    if (fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0)|O_NONBLOCK) == -1) {
        return -1;
    }
    return 0;
}

int handle_message(struct epoll_event evh)
{
    char static buf[MAXBUF + 1];
    int len;
    bzero(buf, MAXBUF + 1);
    if(evh.events == EPOLLIN) {
	len = recv(evh.data.fd, buf, MAXBUF, 0);
	if(len < 1) {
	    return 0;
	}
//	printf("read: %d:%s\n",len,buf);
        evh.events = EPOLLOUT | EPOLLERR | EPOLLHUP | EPOLLET;
        epoll_ctl(kdpfd, EPOLL_CTL_MOD, evh.data.fd, &evh);
    } else if(evh.events == EPOLLOUT)  {
        send(evh.data.fd, out, sizeof(out), 0);
	return 0;
        epoll_ctl(kdpfd, EPOLL_CTL_MOD, evh.data.fd, &evh);
    } else {
        printf("in h: %d\n",evh.events);
	return 0;
    }


    return 1;
}

int main(int argc, char **argv)
{

    if (argv[1])
        myport = atoi(argv[1]);
    else
        myport = 7838;

    if (argv[2])
        lisnum = atoi(argv[2]);
    else
        lisnum = 1;


    listener = socket(AF_INET, SOCK_STREAM, 0);
    setnonblocking(listener);

    bzero(&my_addr, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(myport);
    my_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind (listener, (struct sockaddr *) &my_addr, sizeof(struct sockaddr)) == -1) {
        perror("bind");
        exit(1);
    }

    if (listen(listener, lisnum) == -1) {
        perror("listen");
        exit(1);
    }
    int one = 10;
    setsockopt(listener, IPPROTO_TCP, 9,&one, sizeof(one));
    one = 1;
    setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &one,sizeof(one));
    kdpfd = epoll_create(MAXEPOLLSIZE);
    len = sizeof(struct sockaddr_in);
    ev.events = EPOLLIN | EPOLLERR | EPOLLHUP | EPOLLET;
    ev.data.fd = listener;
    
    if (epoll_ctl(kdpfd, EPOLL_CTL_ADD, listener, &ev) < 0) {
        fprintf(stderr, "epoll set insertion error: fd=%d\n", listener);
        return -1;
    }

    curfds = 1;
    while (1) {
        nfds = epoll_wait(kdpfd, events, curfds, -1);
        if (nfds == -1) {
            perror("epoll_wait");
            break;
        }

//	printf("nfds: %d\n",nfds);
        for (n = 0; n < nfds; ++n) {
	
	    if (events[n].events & (EPOLLHUP | EPOLLERR)) {
		    fputs("epoll: EPOLLERR", stderr);
		    close(events[n].data.fd);
		    continue;
	    } 

            if (events[n].data.fd == listener) {
                new_fd = accept(listener, (struct sockaddr *) &their_addr,
                                &len);
                if (new_fd < 0) {
                    perror("accept");
                    continue;
                }

                setnonblocking(new_fd);
                ev.events = EPOLLIN| EPOLLERR | EPOLLHUP | EPOLLET;
                ev.data.fd = new_fd;
                if (epoll_ctl(kdpfd, EPOLL_CTL_ADD, new_fd, &ev) < 0) {
                    return -1;
                }
                curfds++;
            } else {

                if(handle_message(events[n]) == 0) {
		    epoll_ctl(kdpfd, EPOLL_CTL_DEL, events[n].data.fd, 0);
		    close(events[n].data.fd);
		    curfds--;
		}
		continue;
            }


        }
    }
    close(listener);
    return 0;
}

			