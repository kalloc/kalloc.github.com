#define memscpy(static)  memcpy(buf+buflen,static,sizeof(static)-1);buflen+=sizeof(static)-1
#define memscmp(source,sample)   memcmp(source,sample,sizeof(sample)-1)

