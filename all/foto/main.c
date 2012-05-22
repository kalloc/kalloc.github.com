#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>
#include "iconv_string.h"

#define BLOCK 1024

void main() {



  const char* s = "Ð¿Ñ_Ð¸Ð_Ð÷Ñ'";
  char* result = NULL;
  if (iconv_string("CP1251","UTF-8",s, s+strlen(s)+1, &result, NULL) < 0)
    perror("iconv_string");

 printf("res:%s",result);


}res:(null)