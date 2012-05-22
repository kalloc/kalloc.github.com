#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>

#define BLOCK 1024

char *dir;


void dir_read(char *path) {

        DIR *dirp;
        struct dirent *dp;
        struct stat *buf;
	char fullpath[1024];
	char tn_fullpath[1024];
	char dirname[1024];
	char cptn_fullpath[1024];
	char exec[1024];

	buf=malloc(sizeof(*buf));
	bzero(buf,sizeof(*buf));
	
	stat(path,buf);
	 if(S_ISDIR(buf->st_mode)) {

	dirp = opendir(path);
	while ((dp = readdir(dirp)))  {
		bzero(buf,sizeof(*buf));
		bzero(&fullpath,1024);
		if(strcmp(dp->d_name,"..") &&  strcmp(dp->d_name,".")) {
			snprintf(fullpath,BLOCK-1,"%s/%s",path,dp->d_name);
			stat(fullpath,buf);
			if(S_ISDIR(buf->st_mode)) {
				printf("dir: %s\n",fullpath);
				dir_read(fullpath);
			} else if(S_ISREG(buf->st_mode) && !strstr(dp->d_name,"tn_") &&  strstr(dp->d_name,".jpg")) {
				printf("file: %s\n",fullpath);
				bzero(&tn_fullpath,1024);
				bzero(&cptn_fullpath,1024);
				bzero(&dirname,1024);
				snprintf(tn_fullpath,BLOCK-1,"%s/tn_%s",path,dp->d_name);

				strncpy(dirname,strrchr(path,'/')+1,strlen(path)-(int)(strrchr(path,'/')-path)-1);

				bzero(buf,sizeof(*buf));
				stat(tn_fullpath,buf);

				if(!S_ISREG(buf->st_mode)) {
					bzero(&exec,BLOCK);
					snprintf(exec,BLOCK-1,"C:/DRV/im.exe -verbose -resize 120x120 %s %s",fullpath,tn_fullpath);
					system(exec);
					printf("create: %s\n",tn_fullpath);
				}
				snprintf(cptn_fullpath,BLOCK-1,"%s/tn_%s.jpg",path,dirname);
				bzero(buf,sizeof(*buf));
				stat(cptn_fullpath,buf);
				if(!S_ISREG(buf->st_mode)) {
					bzero(&exec,BLOCK);
					snprintf(exec,BLOCK-1,"cp %s %s",tn_fullpath,cptn_fullpath);
					system(exec);
				}
			}
		} 
	}
	closedir(dirp);

	}
	
	free(buf);

}       


int main () {
	dir=strdup("E:/www/default/foto_content");
	dir_read(dir);	
	return 0;
}





