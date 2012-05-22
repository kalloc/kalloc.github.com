#define MKDWORD(a,b) (((int)a<<16)|((int)b))
#define LOWORD(a) ( (short int)a )
#define HIWORD(a) ( (short int)(((int)a)>>16) )
#define MIN(a,b) (a<b ? a : b )

#define FOREACH_ARRAY( p , array) for(p=array[0],n=0;p; p = array[++n])

void cgi_parse_hex(char * gdata);
unsigned char hex2asc(char s, char f);
char matoh(char x);
void replace(char *buf,char *src, char *dest, int all);
int digit(char * data);
void clow (char *data);
char *rehack (char *data);
