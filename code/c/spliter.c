/*

    ainmarh.com 
    
    mysql dump file spliter


    gcc spliter.c -o spliter
    ./spliter mysql_dump.sql 

*/
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#define DIE(...) {  printf(__VA_ARGS__);exit(-1); }
#define filetrunc(x) fclose(fdwrite);truncate(filename,len_total-x)

struct stat st;
char buf[1024+1],buf_write[1024+1],dbname[512+1],tablename[512+1],filename[512+1];
int i=0,len=0,marker_start=0,marker_end=0,name_len=0,len_write=0,len_total=0,flag_write=1,flag_stdin=0;

FILE *fd,*fdwrite;
extern FILE *stdin;

char *table=NULL;

int if_stdin()
{
    struct timeval tv = {0,2};
    fd_set rdfds;
    FD_ZERO(&rdfds);
    FD_SET(STDIN_FILENO, &rdfds);
    if(select(STDIN_FILENO + 1, &rdfds, NULL, NULL, &tv) == -1)   DIE("select");
    return FD_ISSET(STDIN_FILENO, &rdfds) ? 1 : 0;
}



int main(int argc , char **argv) {

    if(if_stdin()) flag_stdin=1;
    if(!flag_stdin && argc < 2) DIE("usage: %s *file *table\n"
    "\n"
    "\t*table - neobyzatelnuy parametr, imy tablisy kotoray budet iskatsy\n"
    "\t*file - neobyzatelnuy parametr v sluchae esli nujno chitat iz stdin\n"
    "\n"
    ,argv[0])
    if(argc == 3) { table=argv[2];flag_write=0;}
    if(flag_stdin&& argc == 2) { flag_write=1; table=argv[1]; }

    bzero(&filename,512);bzero(&dbname,512);bzero(&tablename,512);bzero(&buf_write,1024);

    char search_start[]="--\n"
    "-- Table structure for table `";
    
    if(!flag_stdin) {  if(stat(argv[1], &st)  || !S_ISREG(st.st_mode) || !(fd=fopen(argv[1],"rb")) ) DIE("error open file:%s (%s)\r\n",argv[1],strerror(errno));}
    else fd=stdin;

    len=fread(&buf,1,256,fd);
    marker_end=(int)strstr(buf,"Database: ")-(int)&buf+10;

    if(marker_end==0) DIE("Mismath sql format (\"Database\" not found)");
    
    for(i=0;buf[marker_end+i]>32;i++) dbname[i]=buf[marker_end+i];
    dbname[marker_end+i]=0;
    
    while((len=fread(&buf,1,1024,fd))) {
	buf[len]=0;
	for(i=0;i<len;i++) {

	    if(marker_start > 0 && search_start[marker_start] == 0) {
		
		if(len_write > 0) {  fwrite(buf_write,len_write,1,fdwrite); len_write=0; }

		if(buf[i] != '`') {
		    if(name_len > 512) DIE("name_len > 512. exit\r\n");

		    tablename[name_len]=buf[i];
		    marker_end++;
		    name_len++;

    		} else {
		    if(flag_write && filename[0] != 0) { 
			    filetrunc(31);
			    printf("\nwrite size:%5dkb\n-------------[close filename:%20s]------------\n",(len_total/1024),filename);
		    }

		    marker_start=name_len=marker_end=len_write=len_total=tablename[name_len]=0;
		    if(table) flag_write= strcmp(tablename,table) ? 0 : 1;
		    if(flag_write) {
			snprintf(filename,512,"%s_%s.sql",dbname,tablename);
			if(!(fdwrite=fopen(filename,"w")))  DIE("error open file:%s (%s)\r\n",filename,strerror(errno));
			printf("\n::\n-------------[open  filename:%20s]------------\n",filename);
		    }


		}

	    } else  {
		if(buf[i] == search_start[marker_start]) {
		    marker_start++;
		} else if(marker_start > 0) {
		    marker_start=0;
		}

		if(flag_write && filename[0] != 0) {
		    if(len_write >= 1023) { fwrite(buf_write,len_write,1,fdwrite); len_write=0; }
		    buf_write[len_write++]=buf[i];
		    len_total++;
		    if((len_total%(1024)) == 0) {
			write(2,".",1);
		    }
		} 
	    }
	}
    }

    if(filename[0]==0) {
	    DIE("table: `%s` not found in dump\n",table);
    } else  if(flag_write && len_write > 0) { 
	    fwrite(buf_write,len_write,1,fdwrite); 
	    filetrunc(39);
	    printf("\nwrite size:%5dkb\n-------------[close filename:%20s]------------\n",(len_total/1024),filename);
    } else if(flag_write){
	    truncate(filename,len_total-8);
    } 




}


//[EOF]
