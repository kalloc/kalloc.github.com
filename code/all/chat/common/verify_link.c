#include <stdio.h>
#include <stdlib.h>
#define _XOPEN_SOURCE
#include <unistd.h>

#ifdef BUILD

#define NET
#define DEFINE
#include "var.h"
#include <ctype.h>
#include <iconv.h>
#else
#define BLOCK 1024

#endif
#include "aes.h"

#define KEYLEN 256
#define b64encode() to64frombits((unsigned char *)base64,(unsigned char *)out,len)
#define b64decode() from64tobits(in,base64)
#define bh() plain2hex(out,base64,len)
#define hb() hex2plain(base64,in,strlen(base64))
#define secretcode "livEvil"
#define SALT "ÍÎÏÇÏÍÎÏÇÏÓÏÌÉ"


char *crypt(const char *key, const char *salt);
   
aes_context ctx;
unsigned int setkey=0,len;
char in[BLOCK+1],out[BLOCK+1],base64[BLOCK+1];
char cryptout[128],cryptb64[256];

char *login,*email;
unsigned char key[]="ÛÌÁÓÁÛÁÐÏÛÁÓÅÉÛÁÛÁÌÁÓÕÓËÕ";
unsigned char iv_key[]="üôï ÷åëôïò îáþáìøîïê éîéãéáìéúáãéé";



char * aes64decrypt(char *base64) {


    if(setkey == 0)  { aes_set_key( &ctx, key, KEYLEN); setkey=1;}

    unsigned char iv[16];
    memcpy(iv,iv_key,16);

    hb();

    printf("%s\n\n",in);
    len=aes_cbc_decrypt(&ctx,iv,(unsigned char*)in,(unsigned char*)out,len); 


    return out;
}

char * aes64encrypt(char *in) {

    if(setkey == 0)  { aes_set_key( &ctx, key, KEYLEN); setkey=1;}

    unsigned char iv[16];
    memcpy(iv,iv_key,16);

    len=aes_cbc_encrypt(&ctx,iv,(unsigned char*)in,(unsigned char*)out,len); 
    bh();
    printf("%s\n\n",out);
    snprintf(out,BLOCK,"%s",base64);
    return out;
}



char * genLink (char *email,char *login) {
	len=snprintf(base64,BLOCK,"%s:%s:%s:%d",email,secretcode,login,(int)time(NULL));
	snprintf(cryptout,126,"%s",(char *)crypt(base64,SALT));
	plain2hex(cryptout,cryptb64,strlen(cryptout));
	return  aes64encrypt(base64);
}



int checkLink (char *Link,char *Hash) {
	len=strlen(Link);
	if(len > BLOCK) return -1;

	aes64decrypt(Link);
	if(len > BLOCK) return -2;

	if(!strstr(out,":"secretcode":")) return -3;

	hex2plain(Hash,cryptout,strlen(Hash));

	if(strcmp((char *)crypt(out,cryptout),cryptout) < 0) return -4;

	int i=0;
	for(;i<=len;i++) {
	    if(out[i] == ':') {
		out[i]=0;
		email=out;    
		break;
	    }
	}
	for(;i<=len;i++) {
	    if(out[i] == ':') 	break;
	}
	i++;
	login=out+i;

	for(;i<=len;i++) {
	    if(out[i] == ':') {
		out[i]=0;
		break;
	    }
	}
	
	if(atoi(out+i+1) + 3600 < time(NULL)) return -5;

	return 1;
}



char * getMail () {
	return email;
}

char * getHash () {
	return cryptb64;
}

char * getLink () {
	return out;
}

char * getLogin () {
	return login;
}





#ifndef BUILD

int main() {

/*
    len=snprintf(in,1024,"%s","LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLOL");
    printf("!%s!\n",aes64encrypt(in));
    printf("!%s!\n",aes64decrypt(base64));
    printf("2print:%s\n",aesdecrypt(aesencrypt("LLLLAAALALALALALALALALALALALALALALALALALLAALLALAALLALLALLALALA")));
*/

    printf("len=%d\r\n",strlen(genLink("me@n1ck.name","looooooooooooongnicki"))+strlen(getHash()));
    printf("http://site.ru/?Link=%d\n",checkLink(getLink(),getHash()));
    printf("http://site.ru/?Link=%s\n",getMail());
    printf("http://site.ru/?Link=%s\n",getLogin());
/*    len=sprintf(in,"%s","loolag");
    bin2hex(in,out,len);
    printf("out:%s\n",out);
    hex2bin(out,in,strlen(out));
    printf("in:%s\n",in);
    */
	return 0;

}


#endif
//EOF
