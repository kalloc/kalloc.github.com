#ifdef FCGI
#include <fcgi_stdio.h>
#endif
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <stdio.h>
#include <sys/stat.h>
#include <qtl_tailq.h>
#include <sys/queue.h>
#include <search.h>

extern char **environ;


#define PNG 0
#define JPG 1
#define GIF 2
#define HTML 3
#define TEXT 4

#define PATH "./banner/"

const char * mime[]={"image/png","image/jpeg","image/gif","text/html","text/plain"};

struct circleq_entry {
	size_t len;
	char *buf;
	CIRCLEQ_ENTRY(circleq_entry) entries;           /* Circular queue. */
};


TQ_DEFINE(formats)
	int ext;
	char type[10];
	CIRCLEQ_HEAD(head, circleq_entry) head ; 
	struct circleq_entry *headp;
TQ_DEFINE_END(formats)
                                        
TQ_VAR(formats) *format=NULL;
                                     
void *root=NULL;

int comparet(const void *a) {
	printf("add: 0x%x\n",a);
}

int compare(const void *pa, const void *pb) {
	TQ_VAR(formats) *left, *right;
	left  = (void *)pa;
	right = (void *)pb;

	if(left->ext == right->ext) {
	//	printf("pa->ext(%d) == (%d)pb->ext\n",left->ext,right->ext);
		return strcmp(left->type,right->type);
	} else if (left->ext < right->ext) {
	//	printf("pa->ext(%d) < (%d)pb->ext\n",left->ext,right->ext);
		return -1;
	} else {
	//	printf("pa->ext(%d) > (%d)pb->ext\n",left->ext,right->ext);
		return 1;	
	} 

}


unsigned int stricmp(char * s1,char * s2) {
    for(;*s2 != 0 && *s1 != 0;s1++,s2++) if(*s2 != *s1 && *s2+32 != *s1) return 0;
    return 1;

}

unsigned int getMimeByExtension(char * ext) {
    if(stricmp(ext,"jpg") == 1) return JPG;
    if(stricmp(ext,"jpeg") == 1) return JPG;
    if(stricmp(ext,"gif") == 1) return GIF;
    if(stricmp(ext,"png") == 1) return PNG;
    if(stricmp(ext,"html") == 1) return HTML;
    return TEXT;
}

void ContentType(int ext) {
    if(mime[ext] != NULL)
        printf("Content-type: %s\r\n\r\n",mime[ext]);
    else
		printf("Content-Unknow: %d\r\nContent-type: text/html\r\n\r\n",ext);
}


void printenv () {
        char **p = environ;
        char *ENV_NAME;
        char *ENV_VALUE;
	    while(*p != NULL) {
            ENV_VALUE=strstr(*p,"=");
            ENV_VALUE[0]='\0';
            ENV_VALUE+=1;
            ENV_NAME=*p;
            printf("%s:%s\n",ENV_NAME,ENV_VALUE);
            *p++;

	   }
}

TQ_VAR(formats)  * getFormat(char *type,char *ext) {

	TQ_VAR(formats) *new,*ptr;

	new=TQ_MALLOC(formats);
	bzero(new,sizeof(TQ_VAR(formats)));
	CIRCLEQ_INIT(&(new->head));  

	snprintf(new->type,10,"%s",type);
	new->ext=getMimeByExtension(ext);
	if((ptr=tfind(new,&root,compare)) == 0) {
		ptr=tsearch((void *)new,&root,compare);
#ifdef DEBUG			
		printf("format init head:%x for: %s %s\n",ptr,type,ext);
#endif

	} else {
		free(new);
	}
#ifdef DEBUG			
	printf("return ptr: 0x%x\n",ptr);
#endif
	return ptr;

}


void * m (size_t size) {
	void * ptr=0;
	ptr=malloc(size);
	bzero(ptr,size);
	return ptr;
}


void action(const void *nodep, const VISIT which, const int depth) 	{
	int *datap;

	datap = *(int **) nodep;
	printf("%x\n", datap);

}



int createmap(char * path,char *type) {
#ifdef DEBUG			
	printf("\t\tread path:%s format:%s\n", path,type);
#endif			
	DIR *dir = NULL;
	FILE *fp = NULL;
	TQ_VAR(formats) *format;
	struct stat st;
	struct circleq_entry *file=NULL;
	char static filename[1025], bigbuf[65535];
	struct dirent  *dp;
	
	if(!(dir = opendir(path))) return 0;
	while ((dp=readdir(dir))){
		if(!strcmp(dp->d_name,".") || !strcmp(dp->d_name,"..")) continue;
		
		snprintf(filename,1024,"%s/%s",path,dp->d_name);
		stat(filename, &st);
		
		if(S_ISREG(st.st_mode)) {
			file = m(sizeof(* file));
			
			fp=fopen(filename,"rb");
			file->len=fread(bigbuf,1,sizeof(bigbuf),fp);
			fclose(fp);

			if(file->len <= 0) return 0;


			file->buf=m(file->len);

			memcpy(file->buf,bigbuf,file->len);

#ifdef DEBUG			
			printf("старый format:%x\n",format);			
#endif			
			format=getFormat(type,strrchr(dp->d_name,'.')+1);


#ifdef DEBUG			
			printf("извлекаем format:%x\n",format);			
			printf("приступаем к CIRCLEQ_INSERT_TAIL:%x %x %x %x\n",(&format->head),(&format->head)->cqh_last,(&format->head)->cqh_first,file);
#endif			
			CIRCLEQ_INSERT_TAIL(&format->head,file, entries);
#ifdef DEBUG			
			printf("после CIRCLEQ_INSERT_TAIL:%x %x %x %x\n",(&format->head),(&format->head)->cqh_last,(&format->head)->cqh_first,file);
			printf("%d %s 0x%x\n\n\n",getMimeByExtension(strrchr(dp->d_name,'.')+1),strrchr(dp->d_name,'.')+1,file);
#endif			

		}
			
	}
	
	closedir(dir);

	return 1;
}

int main_createmap(char * path) {

	DIR *dir;
	struct stat st;
	char filename[1025];
	struct dirent  *dp;
	dir = opendir(path);
	if(!dir) return 0;

	while ((dp=readdir(dir))){
		if(!strcmp(dp->d_name,".") || !strcmp(dp->d_name,"..")) continue;
		
		snprintf(filename,1024,"%s/%s",path,dp->d_name);
		stat(filename, &st);
		
		if(S_ISDIR(st.st_mode)) if(createmap(filename,dp->d_name) == 0) return 0;
			
	}
	
	closedir(dir);
	return 1;
}

void FetchFile(char *type,char *ext) {
	ContentType(getMimeByExtension(ext));
	
	TQ_VAR(formats) *new,*ptr;
	new=TQ_MALLOC(formats);
	bzero(new,sizeof(TQ_VAR(formats)));
	memcpy(new->type,type,strlen(type));
	new->ext=getMimeByExtension(ext);
	
	if((ptr=tfind(new,&root,compare)) == 0) {
	
		printf("Content-type: text/plain\r\n\r\nFormat:%s not found\r\n",type);

	} else {
	
		if(ptr->headp == NULL) { ptr->headp = CIRCLEQ_FIRST(&ptr->head); }
		ptr->headp=CIRCLEQ_LOOP_NEXT(&ptr->head,ptr->headp,entries);
		fwrite(ptr->headp->buf,ptr->headp->len,1,stdout);

	}
	free(new);
}

int main(void)
{
	char *ptr,*ext,*type;
        if(main_createmap(PATH) == 0) {
	    printf("Content-type: text/html\r\n\r\nUnable to initialization Banner Engine\r\n");
	    return 0;
	}


#ifdef FCGI
   while(FCGI_Accept() >= 0) {
		if((ptr=getenv("QUERY_STRING")) ==  NULL) {

			printf(
					"Content-type: text/html\r\n\r\n"
					"Unable to get query string\r\n"
					);

		} else if((ext = strstr(ptr,"ext=")-1) == -1 || (type = strstr(ptr,"type=")+5) == 5) {
			printf(
					"Content-type: text/plain\r\n\r\n"
					"Unable to parse query string\r\n"
					);
		} else {
			ext[0] = 0;ext+= 5;

			FetchFile(type,ext);
		}
	}
#endif

#ifndef FCGI    

/*
    FetchFile("468x60","jpeg");
    FetchFile("30x30","jpeg");
    FetchFile("30x30","html");
    FetchFile("30x30","gif");
    FetchFile("30x30","png");
*/
    twalk(root,action);	
    tdestroy(root,comparet);
#endif

    return 0;
}
