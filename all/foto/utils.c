#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <locale.h>
#include "utils.h"

#define MAX_BUF 500000
#define OUTBUF_LEN 800000
static char BigTempBuf[MAX_BUF];
static char outbuf[OUTBUF_LEN];
static int outbuf_len;

int clow_flag=0;

void cgi_parse_hex(char * gdata)
{
  unsigned int len = 0;
  unsigned char r = 0;
  unsigned char *p = NULL;

  len = strlen(gdata);

  for (p = gdata; *p; p++) {
      len--;

      if (*p == '%') {
         if ((*(p + 1)) && (*(p + 2))) {
            r = hex2asc(*(p + 1), *(p + 2));
            
            *p = r;

            memcpy((char *)(p + 1), (char *)(p + 3), (len - 2));

            *(p + len - 1) = '\0';
         }         
      }

      else if (*p == '+')
	 *p = ' ';
  }
}
unsigned char hex2asc(char s, char f)
{
  char ret = 0;
  unsigned char val1 = 0, val2 = 0, val3 = 0;

  ret = matoh(f);
  if (ret == -1)
     return 0;

  val1 = ret;

  ret = matoh(s);
  if (ret == -1)
     return 0;

  val2 = ret;

  val3 = val1 + (val2 * 15) + val2;
  return val3;
}
char matoh(char x)
{
  char ret = 0;

  switch(x) {
    case '0':
             ret = 0;
             break;
    case '1':
             ret = 1;
             break;
    case '2':
             ret = 2;
             break;
    case '3':
             ret = 3;
             break;
    case '4':
             ret = 4;
             break;
    case '5':
             ret = 5;
             break;
    case '6':
             ret = 6;
             break;
    case '7':
             ret = 7;
             break;
    case '8':
             ret = 8;
             break;
    case '9':
             ret = 9;
             break;
    case 'A':
             ret = 10;
             break;
    case 'B':
             ret = 11;
             break;
    case 'C':
             ret = 12;
             break;
    case 'D':
             ret = 13;
             break;
    case 'E':
             ret = 14;
             break;
    case 'F':
             ret = 15;
             break;
    default:
             ret = -1;
             break;
  }

  return ret;
}
void replace(char *buf,char *src, char *dest, int all){
    char *p;
	int f=0;
    p=strstr(buf,src);
	
	while(p){
		f=0;
		if ( *(p+strlen(src)) ) {str1cpy( BigTempBuf, p+strlen(src), MAX_BUF); f=1;}
		strcpy(p,dest);
        if (f) strcpy(p+strlen(dest),BigTempBuf);
		if (all) p=strstr(p+strlen(dest),src);
            else break;
	}
	return;
}
int digit(char * data){
	char *p;
	for(p=data;*p;p++)
		if (!isdigit(*p))
			 return 0;

	return 1;
}

void clow (char *data){
	char *p;
	if (!clow_flag) { setlocale (LC_CTYPE,"ru_RU.cp1251"); clow_flag=1; }
	for (p=data;*p;p++)
		p[0] = tolower (p[0]);
}

void strqcat (char *str)
{
	while ((*str)&&(outbuf_len+3<OUTBUF_LEN)) {
		outbuf[outbuf_len++] = str[0];str++;
	}
}

void strqqcat (char str)
{
	if (outbuf_len+3>=OUTBUF_LEN) return;
	outbuf[outbuf_len++] = str;
}

char *rehack (char *data) {
	int i,len;
	outbuf[0]='\0';outbuf_len = 0;
	len = strlen (data);
	for (i=0;i<len;i++) switch (data[i]) {
		case '&':
			strqcat ("&#38;");
			break;
		case '<':
			strqcat ("&lt;");
			break;
		case '>':
			strqcat ("&gt;");
			break;
		case '\'':
			strqcat ("&#39;");
			break;
		case '\\':
			strqcat ("&#92;");
			break;
		case '\t':
		case '\n':
		case '\r':
			strqcat (" ");
			break;
		default:
			strqqcat (data[i]);
			break;
	}
	outbuf[outbuf_len] = '\0';
	return outbuf;
}
