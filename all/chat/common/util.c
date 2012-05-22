#include "var.h"
	
void setguid()
{


  if(getenv("GID") == NULL) 
		err_sys("need ENV GID");
  if(setegid(atoi(getenv("GID"))) == -1) 
		err_sys("failed setegid to %i",atoi(getenv("GID")));
  if(setgid(atoi(getenv("GID"))) == -1) 
		err_sys("failed setgid to %i",atoi(getenv("GID")));

  if(getenv("UID") == NULL) 
		err_sys("need ENV UID");
  if(seteuid(atoi(getenv("UID"))) == -1) 
		err_sys("failed seteuid to %i",atoi(getenv("UID")));
  if(setuid(atoi(getenv("UID"))) == -1) 
		err_sys("failed setuid to %i",atoi(getenv("UID")));

   printf("UID=%d, EUID=%d\n", (int)getuid(),(int)geteuid());
   printf("GID=%d, EGID=%d\n", (int)getgid(),(int)getegid());



}

char  strreplace(char * buf, char search , char replace)
{

 char output[BUFFSIZE];
 memset(output,0,BUFFSIZE);
 int j,i;
 i=strlen(buf);
 
 for(j=0;j<=i;j++)
 {
	if(buf[j] == search)
	   output[j]=replace;
	else
	   output[j]=buf[j];
 }
 bzero(buf,strlen(buf));
 strcpy(buf,output);

 return 0;

} 


char  substr(char * buf, int l, int start)
{
 static char output[BUFFSIZE];bzero(output,BUFFSIZE);
 bzero(output,BUFFSIZE);
 int j,i;i=strlen(buf);
 
 if(i<l) { }
 else {
	 if(i<(l+start))
		 l=i-start;

	 for(j=0;j<l;j++)
	 	output[j]=buf[j+start];
	 bzero(buf,BUFFSIZE);
	 strcpy(buf,output);
 }

 return 0;
} 

int check_config(char * string)
{
        int i,ii,flag=FALSE;
	ii=sizeof(service_table)/sizeof(config_struct);
	for(i=1;i<=ii;i++)
        {
		if(!strcmp(service_table[i-1].options,string))
		{
			flag=TRUE;
			break;
		}
		
        }
	if(flag)
		return i;
	else
		return FALSE;
}

