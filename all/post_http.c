/*

    ainmarh.com lab // 2007
    sample poster
    .
    дефолтовая сборка, при которой заголовки читаются общей функцией чтения, и POST формируется заранее
    gcc post_http.c -o post_http -Wall -lcurl
    сборка при которой заголовки читает отдельная функция
    gcc post_http.c -o post_http -Wall -lcurl -DHEADFUNC
    сборка при которой POST формируется средствами libcurl
    gcc post_http.c -o post_http -Wall -lcurl -DFORMPOST

*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>

#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>


#define BUF 1024

char UserAgent[]= "Mozilla/5.0 (Windows; U; Windows NT 5.1; en-US; rv:1.8.0.6) Gecko/20060728 Firefox/1.5.0.6";
char Post[]="&hash1=dfjdsfjdkjfhdsk&hash2=fdskfhdksfhdksfhdjkdf&action=dfdskjfhdksjfh&code=dfjdslkfjdslfjd&message=djfdskfhdskfhj";

char error[CURL_ERROR_SIZE];
CURL *curl;
struct curl_httppost* post = NULL,* last = NULL;

#ifdef HEADFUNC
struct bufferStruct {  char  buffer[BUF];  size_t size; } *buf,*bufhead;
#else
struct bufferStruct {  char  buffer[BUF];  size_t size; } *buf;
#endif

size_t callback(void *ptr, size_t size, size_t nmemb, void *data) {
  size_t realsize = size * nmemb;
  if(buf->size+realsize > BUF) return -1;
  memcpy(buf->buffer+buf->size, ptr, realsize);
  buf->size += realsize;
  buf->buffer[buf->size] = 0;
  return realsize;
}

#ifdef HEADFUNC
size_t head_callback(void *ptr, size_t size, size_t nmemb, void *data) {
  size_t realsize = size * nmemb;
  if(bufhead->size+realsize > BUF) return -1;
  memcpy(bufhead->buffer+bufhead->size, ptr, realsize);
  bufhead->size += realsize;
  bufhead->buffer[bufhead->size] = 0;
  return realsize;
}
#endif

int curlization(char *uri) {

 int ret=0;
 buf->size=0;
 curl_easy_setopt(curl, CURLOPT_USERAGENT, UserAgent);
 curl_easy_setopt(curl, CURLOPT_URL, uri);

#ifdef HEADFUNC
 bufhead->size=0;
 curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, head_callback);
#else
 curl_easy_setopt(curl, CURLOPT_HEADER, 1);
#endif  
 curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);
 curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buf);


//можно так
#ifdef FORMPOST
 curl_formadd(&post, &last, CURLFORM_COPYNAME, "hash",CURLFORM_PTRCONTENTS, "aaaaaaaaaaaa", CURLFORM_END);
 curl_formadd(&post, &last, CURLFORM_COPYNAME, "hash2",CURLFORM_PTRCONTENTS, "bbbbbbbbbbbbb", CURLFORM_END);
 curl_formadd(&post, &last, CURLFORM_COPYNAME, "hash3",CURLFORM_PTRCONTENTS, "cccccccccccccccc", CURLFORM_END);
 curl_easy_setopt(curl, CURLOPT_HTTPPOST, post);
#else
 curl_easy_setopt(curl, CURLOPT_POSTFIELDS, Post);
 curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, sizeof(Post)-1);
#endif

 if (curl_easy_perform(curl) != CURLE_OK) {
      fprintf(stderr, "%s: %s\n\n", uri, (error[0]) ? error : "Unknown Error");
      ret=-1;
  }


  curl_easy_cleanup(curl);

#ifdef FORMPOST
  curl_formfree(post);
#endif

 return ret;
}


int main(int argc, char **argv)
{
  buf=malloc(sizeof(struct bufferStruct));

#ifdef HEADFUNC
  bufhead=malloc(sizeof(struct bufferStruct));
#endif

  if (!(curl = curl_easy_init())) {      fprintf(stderr, "Unable to initialize curl\n");      exit(1);  }
  if(curlization(argv[1] ? argv[1] : "http://i18n.ru/1.php" ) == 0) {
  
#ifdef HEADFUNC
  printf("--------:[HEAD]:--------\n%s\n------------------------\n",bufhead->buffer);
  printf("--------:[BODY]:--------\n%s\n------------------------\n",buf->buffer);
#else
  printf("--------:[HEAD+BODY]:--------\n%s\n-----------------------------\n",buf->buffer);
#endif
  
  }
  exit(0);
}

