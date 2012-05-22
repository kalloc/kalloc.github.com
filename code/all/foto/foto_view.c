/* Generated from [templates/foto_view.qtp] by QTP (version 1.4.4 Linux 2.6.21-2-686-bigmem) */

#include <string.h>
#include <stdio.h>
#include <cgix.h>
#include <qcm_internal.h>
extern FILE *cachef;

void foto_view_header_do ();
void foto_view_header (char *title) {
	fwrite ("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\">\r\n<html>\r\n<HEAD>\r\n<TITLE>фото альбом сайта n1ck.name ",155,1,stdout);
	if(strlen(title)>0) {
		fprintf(stdout," :: %s",title);
	}
	fwrite (" :: просмотр/комментирование</TITLE>\r\n<script language=\"javascript\" type=\"text/javascript\" src=\"/js/script.js\"></script>\r\n<HTML>\r\n<HEAD>\r\n</HEAD>\r\n<BODY>\r\n",155,1,stdout);
	if (!cachef) return;
	fwrite ("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\">\r\n<html>\r\n<HEAD>\r\n<TITLE>фото альбом сайта n1ck.name ",155,1,cachef);
	if(strlen(title)>0) {
		fprintf(cachef," :: %s",title);
	}
	fwrite (" :: просмотр/комментирование</TITLE>\r\n<script language=\"javascript\" type=\"text/javascript\" src=\"/js/script.js\"></script>\r\n<HTML>\r\n<HEAD>\r\n</HEAD>\r\n<BODY>\r\n",155,1,cachef);
}
void foto_view_body_do ();
void foto_view_body (char *url, char *text, char *pic) {
	fwrite ("<div style=\"text-align:center;width:100%;height:160px;\">\r\n		<img src=\"",70,1,stdout);
	fprintf(stdout,"%s",pic);
	fwrite ("\" border=\"0\" >\r\n	<p style=\"text-align:center;font-family:Verdana\"><sup>",71,1,stdout);
	fprintf(stdout,"%s",text);
	fwrite ("</sup></p>\r\n	<div align=\"left\" id=\"commentus\" style=\"margin-left:35%;padding-left:10px;margin-right:35%;width:360px;border:1px dashed black;text-align:center;font-family:Verdana\">\r\n	&nbsp;		\r\n\r\n	</div>\r\n	<div align=\"center\"  id=\"formus\" style=\"margin-left:35%;margin-right:35%;width:370px;border:1px dashed black;text-align:center;font-family:Verdana\">\r\n		who are you <br/>(кто тут )? <br/><input onkeyup=\"checkenter();\" onchange=\"checkenter();\" style=\"border:1 px solid black\"  type=\"text\" id=\"gb_name\"><br/><br/>\r\n		what do yo want <br/>(умный молодец, сказать что желаешь?)\r\n		<textarea  onkeyup=\"checkenter();\" onchange=\"checkenter();\" style=\"border:1 px solid black\" id=\"gb_text\" rows=5 cols=25></textarea><br/>\r\n	      <input id=\"butt\" style=\"border:1 px solid black\"  onclick=\"addAnswer()\" type=\"button\" value=\"заслать (send)\"><br/><br/>\r\n\r\n	</div>\r\n\r\n</div>\r\n\r\n\r\n\r\n",873,1,stdout);
	if (!cachef) return;
	fwrite ("<div style=\"text-align:center;width:100%;height:160px;\">\r\n		<img src=\"",70,1,cachef);
	fprintf(cachef,"%s",pic);
	fwrite ("\" border=\"0\" >\r\n	<p style=\"text-align:center;font-family:Verdana\"><sup>",71,1,cachef);
	fprintf(cachef,"%s",text);
	fwrite ("</sup></p>\r\n	<div align=\"left\" id=\"commentus\" style=\"margin-left:35%;padding-left:10px;margin-right:35%;width:360px;border:1px dashed black;text-align:center;font-family:Verdana\">\r\n	&nbsp;		\r\n\r\n	</div>\r\n	<div align=\"center\"  id=\"formus\" style=\"margin-left:35%;margin-right:35%;width:370px;border:1px dashed black;text-align:center;font-family:Verdana\">\r\n		who are you <br/>(кто тут )? <br/><input onkeyup=\"checkenter();\" onchange=\"checkenter();\" style=\"border:1 px solid black\"  type=\"text\" id=\"gb_name\"><br/><br/>\r\n		what do yo want <br/>(умный молодец, сказать что желаешь?)\r\n		<textarea  onkeyup=\"checkenter();\" onchange=\"checkenter();\" style=\"border:1 px solid black\" id=\"gb_text\" rows=5 cols=25></textarea><br/>\r\n	      <input id=\"butt\" style=\"border:1 px solid black\"  onclick=\"addAnswer()\" type=\"button\" value=\"заслать (send)\"><br/><br/>\r\n\r\n	</div>\r\n\r\n</div>\r\n\r\n\r\n\r\n",873,1,cachef);
}
void foto_view_footer () {
	fwrite ("<script>\r\n\r\nfunction addAnswer(){\r\n\r\n	ajax_query('?ajax',{ 'call':'add','gb_name':document.getElementById('gb_name').value,'gb_text':document.getElementById('gb_text').value});\r\n//	loadXML('?ajax',{ 'call':'add','gb_name':gb_name,'gb_text':gb_text},AnswerReply);\r\n	return false;\r\n}\r\nfunction printAnswer(){\r\n\r\n	checkenter();\r\n	ajax_query('?ajax',{'call':'print'});\r\n//	loadXML('?ajax',{'call':'print'},AnswerReply);\r\n\r\n	return false;\r\n}\r\n\r\n\r\nvar AnswerReply=function() {\r\n		eval(req.responseText);\r\n}\r\n\r\n\r\n\r\nfunction checkenter() {\r\n\r\n\r\n	var gbname=document.getElementById('gb_name').value;\r\n	var gbtext=document.getElementById('gb_text').value;\r\n	var bt=document.getElementById('butt');\r\n\r\n\r\n	if(gbname == \"\" || gbtext== \"\")\r\n	{\r\n		bt.disabled=true;\r\n	} else {\r\n		bt.disabled=false;\r\n		\r\n	}\r\n\r\n\r\n}\r\n\r\n\r\nprintAnswer();\r\n</script>\r\n</BODY>\r\n</HTML>\r\n",849,1,stdout);
	if (!cachef) return;
	fwrite ("<script>\r\n\r\nfunction addAnswer(){\r\n\r\n	ajax_query('?ajax',{ 'call':'add','gb_name':document.getElementById('gb_name').value,'gb_text':document.getElementById('gb_text').value});\r\n//	loadXML('?ajax',{ 'call':'add','gb_name':gb_name,'gb_text':gb_text},AnswerReply);\r\n	return false;\r\n}\r\nfunction printAnswer(){\r\n\r\n	checkenter();\r\n	ajax_query('?ajax',{'call':'print'});\r\n//	loadXML('?ajax',{'call':'print'},AnswerReply);\r\n\r\n	return false;\r\n}\r\n\r\n\r\nvar AnswerReply=function() {\r\n		eval(req.responseText);\r\n}\r\n\r\n\r\n\r\nfunction checkenter() {\r\n\r\n\r\n	var gbname=document.getElementById('gb_name').value;\r\n	var gbtext=document.getElementById('gb_text').value;\r\n	var bt=document.getElementById('butt');\r\n\r\n\r\n	if(gbname == \"\" || gbtext== \"\")\r\n	{\r\n		bt.disabled=true;\r\n	} else {\r\n		bt.disabled=false;\r\n		\r\n	}\r\n\r\n\r\n}\r\n\r\n\r\nprintAnswer();\r\n</script>\r\n</BODY>\r\n</HTML>\r\n",849,1,cachef);
}
void foto_view_footer_do() {
	foto_view_footer();
}


void foto_view() {
	printf ("\r\n");
	foto_view_header_do();
	foto_view_body_do();
	foto_view_footer_do();
}
