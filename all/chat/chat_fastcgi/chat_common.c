#include <stdio.h>
#include <stdlib.h>
#define NETH
#define DEFINE
#include "var.h"
#include <qtp.h>
#include <qtl_tailq.h>
#include <ctype.h>
#include <iconv.h>


//
static char top[]="Content-Type:text/xml; charset=windows-1251\n"
		  "A-Powered-Via:QTP 1.4.4\n"
		  "\n"
		  "<?xml version=\"1.0\" encoding=\"windows-1251\" ?>\r\n<response>\r\n";
static char bottom[]="</response>";

static char error[]="<error>server_no_reply</server><reload><![CDATA[/]]></reload>\r\n";
static char reerror[]="<reload><![CDATA[%s]]></reload>\r\n";

void error_404() {

		char *query;

		 printf("A-Powered-Via: QTP 1.4.4\n");
	         printf("Content-Type: text/html; charset=windows-1251\n");
		 cgiHeaderStatus(404,"Not Found");
		 if((query=getenv("REQUEST_URI")) != NULL)  {

		 printf("<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\n"
			"<html><head>\n"
			"<title>404 Not Found</title>\n"
			"</head><body>\n"
			"<h1>Not Found</h1>\n"
			"<p>The requested URL ");
		cgiHtmlEscape(query);
		printf(" was not found on this server.</p>\n"
			"<hr>\n"
			"<address>%s Server at %s Port %s</address>\n"
			"</body></html>\n",cgiServerSoftware,cgiServerName,cgiServerPort);

		}
}
void error_xml() {
		 cgiTop();
		 
		 cgiDown();
}
void error_500() {
  		cgiHeaderStatus(500,"Internal server error");
		cgiTop();
    		cgiWriteStr(error);
		cgiDown();
}





void cgiTop() {
	cgiWriteStr(top);

}

void cgiDown() {
	cgiWriteStr(bottom);
}





void reload(char *str) {
	cgiTop();
	cgiprintf(reerror,str);
	cgiDown();
}





//EOF

