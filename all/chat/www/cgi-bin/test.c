#include <stdio.h>
#include <stdlib.h>
#include <qtp.h>
#include <qtl_tailq.h>

int cgiInit() {
cgiGetAfterPost(1);
return 0;

}



int cgiMain () {
    char ptr[1024];
    printf("Content-Type:text/plain\r\n\r\nlalalaal\r\n");
    if(!cgiFormString("l",ptr,10000)) printf("GET: %s\r\n",ptr);
    if(!cgiFormString("d",ptr,10000)) printf("POST: %s\r\n",ptr);
    if(!cgiFormString("t",ptr,10000)) printf("POST: %s\r\n",ptr);
    return 0;

}





