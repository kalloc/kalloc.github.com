#include <stdio.h>
#include <stdlib.h>
#ifdef BUILD

#define NETH
#define DEFINE
#include "var.h"
#include <ctype.h>
#include <iconv.h>

#endif

#define EXECPATH "/usr/sbin/sendmail"
#define MAILFROM "nikes99@mail.ru"
#define MAILFROMNAME "nikita"

#define SENDMAIL_START	if(fork()>0) return ; \
		        if(ex[0] != '/')    snprintf(ex,255,"%s -t -f %s",EXECPATH,MAILFROM);\
		        if (!(pp=popen(ex,"w")))  exit(0);					\
		        fprintf(pp,"From: \"%s\" <%s>\n",MAILFROMNAME,MAILFROM);


#define SENDMAIL_END	fputs("\n\n",pp);	\
		        fputs(".\n",pp);	\
		        pclose(pp);		\
		        exit(0);

char ex[256];
FILE *pp;

void mail_pre_reg(char *to,char *user,char *ip) {

    SENDMAIL_START

    fprintf(pp,"to: \"%s\" <%s>\n",user,to);
    genLink(to,user);
    fprintf(pp,"subject: registration confirm on untitled.ainmarh.com\n\n");
    fprintf(pp,"ÂÛ (èëè êòî-òî äğóãîé ñ àäğåñà - %s) ÇÀÏĞÎÑÈË  ĞÅÃÈÑÒĞÀÖÈŞ Â ÏĞÎÅÊÒÅ untitled game\r\n\r\n"
	    "ÄËß ÏÎÄÒÂÅĞÆÄÅÍÈß ĞÅÃÈÑÒĞÀÖÈÈ ÎÒÊĞÎÉÒÅ ÑÑÛËÊÓ:\r\n"
	    "\r\n"
	    "http://untitled.ainmarh.com/registration_verify?Verify=%s&Hash=%s\r\n\r\n"
	    "\r\n"
	    "Ñ ÓÂÀÆÅÍÈÅÌ, ÀÄÌÈÍÈÑÒĞÀÖÈß untitled.ainmarh.com\r\n",ip,getLink(),getHash());


    SENDMAIL_END
}


void mail_reg(char *to,char *user,char * pass) {

    SENDMAIL_START
    
    fprintf(pp,"to: \"%s\" <%s>\n",user,to);
    fprintf(pp,"subject: registration on untitled.ainmarh.com\n\n");
    fprintf(pp,"ÁËÀÃÎÄÀĞÈÌ ÂÀÑ ÇÀ ĞÅÃÈÑÒĞÀÖÈŞ Â ÏĞÎÅÊÒÅ untitled game\r\n"
	    "ÂÀØÈ ÄÀÍÍÛÅ ÄËß ÂÕÎÄÀ Â ÈÃĞÓ:\r\n"
	    "\r\n"
	    "ËÎÃÈÍ: %s\r\n"
	    "ÏÀĞÎËÜ: %s\r\n"
	    "\r\n"
	    "Ñ ÓÂÀÆÅÍÈÅÌ, ÀÄÌÈÍÈÑÒĞÀÖÈß untitled.ainmarh.com\r\n",user,pass);

    SENDMAIL_END

}



#ifndef BUILD

int main() {

    sendmail("me@n1ck.name","TesT","TTTTTTTTTTTTTTTTEEEEEEEEEEEEEEEESSSSSSSSSSSSSSSST");
    return 0;

}


#endif
//EOF
