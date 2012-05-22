/* Generated from [templates/foto_index.qtp] by QTP (version 1.4.4 Linux 2.6.21-2-686-bigmem) */

#include <string.h>
#include <stdio.h>
#include <cgix.h>
#include <qcm_internal.h>
extern FILE *cachef;

void foto_index_header_do ();
void foto_index_header (char *title) {
	fwrite ("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\">\r\n<html>\r\n<HEAD>\r\n<TITLE>фото альбом сайта n1ck.name ",155,1,stdout);
	if(strlen(title)>0) {
		fprintf(stdout," :: %s",title);
	}
	fwrite ("</TITLE>\r\n<script language=\"javascript\" type=\"text/javascript\" src=\"/js/script.js\"></script>\r\n<script language=\"javascript\" type=\"text/javascript\" src=\"/js/jquery.js\"></script>\r\n<script language=\"javascript\" type=\"text/javascript\" src=\"/js/interface.js\"></script>\r\n<link href=\"/js/css.css\" rel=\"stylesheet\" type=\"text/css\" />\r\n</HEAD>\r\n<BODY>\r\n",344,1,stdout);
	if (!cachef) return;
	fwrite ("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\">\r\n<html>\r\n<HEAD>\r\n<TITLE>фото альбом сайта n1ck.name ",155,1,cachef);
	if(strlen(title)>0) {
		fprintf(cachef," :: %s",title);
	}
	fwrite ("</TITLE>\r\n<script language=\"javascript\" type=\"text/javascript\" src=\"/js/script.js\"></script>\r\n<script language=\"javascript\" type=\"text/javascript\" src=\"/js/jquery.js\"></script>\r\n<script language=\"javascript\" type=\"text/javascript\" src=\"/js/interface.js\"></script>\r\n<link href=\"/js/css.css\" rel=\"stylesheet\" type=\"text/css\" />\r\n</HEAD>\r\n<BODY>\r\n",344,1,cachef);
}
void foto_index_body_do ();
void foto_index_body (char *url, char *text, char *pic) {
	fwrite ("<div style=\"text-align:center;width:170px;height:170px;float:left\">\r\n	",70,1,stdout);
	if(strlen(pic)>0) {
	fwrite ("		",2,1,stdout);
	if(strstr(url,"foto_view")) {
	fwrite ("			<a target=\"_new\" href=\"",26,1,stdout);
	fprintf(stdout,"http://xxl/foto_content/2007/new/2007-06-13/catz.jpg?%s",url);
	fwrite ("\" rel=\"imagebox-view\" title=\"",29,1,stdout);
	fprintf(stdout,"%s",pic);
	fwrite ("\"><img src=\"",12,1,stdout);
	fprintf(stdout,"%s",pic);
	fwrite ("\" border=\"0\" /></a>\r\n		",23,1,stdout);
	} else {
	fwrite ("			<a href=\"",12,1,stdout);
	fprintf(stdout,"%s",url);
	fwrite ("\" rel=\"imagebox-view\" title=\"",29,1,stdout);
	fprintf(stdout,"%s",pic);
	fwrite ("\"><img src=\"",12,1,stdout);
	fprintf(stdout,"%s",pic);
	fwrite ("\" border=\"0\" /></a>\r\n		",23,1,stdout);
	}
	fwrite ("</a>\r\n	",7,1,stdout);
	} else {
	fwrite ("		<a href=\"",11,1,stdout);
	fprintf(stdout,"%s",url);
	fwrite ("\"  rel=\"imagebox-view\" title=\"",30,1,stdout);
	fprintf(stdout,"%s",pic);
	fwrite ("\">",2,1,stdout);
	fprintf(stdout,"%s",text);
	fwrite ("</a>\r\n	",7,1,stdout);
	}
	fwrite ("	<p style=\"text-align:center;font-family:Verdana\"><sup>",55,1,stdout);
	fprintf(stdout,"%s",text);
	fwrite ("</sup></p>\r\n</div>\r\n\r\n\r\n",24,1,stdout);
	if (!cachef) return;
	fwrite ("<div style=\"text-align:center;width:170px;height:170px;float:left\">\r\n	",70,1,cachef);
	if(strlen(pic)>0) {
	fwrite ("		",2,1,cachef);
	if(strstr(url,"foto_view")) {
	fwrite ("			<a target=\"_new\" href=\"",26,1,cachef);
	fprintf(cachef,"http://xxl/foto_content/2007/new/2007-06-13/catz.jpg?%s",url);
	fwrite ("\" rel=\"imagebox-view\" title=\"",29,1,cachef);
	fprintf(cachef,"%s",pic);
	fwrite ("\"><img src=\"",12,1,cachef);
	fprintf(cachef,"%s",pic);
	fwrite ("\" border=\"0\" /></a>\r\n		",23,1,cachef);
	} else {
	fwrite ("			<a href=\"",12,1,cachef);
	fprintf(cachef,"%s",url);
	fwrite ("\" rel=\"imagebox-view\" title=\"",29,1,cachef);
	fprintf(cachef,"%s",pic);
	fwrite ("\"><img src=\"",12,1,cachef);
	fprintf(cachef,"%s",pic);
	fwrite ("\" border=\"0\" /></a>\r\n		",23,1,cachef);
	}
	fwrite ("</a>\r\n	",7,1,cachef);
	} else {
	fwrite ("		<a href=\"",11,1,cachef);
	fprintf(cachef,"%s",url);
	fwrite ("\"  rel=\"imagebox-view\" title=\"",30,1,cachef);
	fprintf(cachef,"%s",pic);
	fwrite ("\">",2,1,cachef);
	fprintf(cachef,"%s",text);
	fwrite ("</a>\r\n	",7,1,cachef);
	}
	fwrite ("	<p style=\"text-align:center;font-family:Verdana\"><sup>",55,1,cachef);
	fprintf(cachef,"%s",text);
	fwrite ("</sup></p>\r\n</div>\r\n\r\n\r\n",24,1,cachef);
}
void foto_index_footer () {
	fwrite ("</BODY>\r\n</HTML>\r\n",18,1,stdout);
	if (!cachef) return;
	fwrite ("</BODY>\r\n</HTML>\r\n",18,1,cachef);
}
void foto_index_footer_do() {
	foto_index_footer();
}


void foto_index() {
	printf ("\r\n");
	foto_index_header_do();
	foto_index_body_do();
	foto_index_footer_do();
}
