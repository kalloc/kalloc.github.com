#include <stdio.h>
#include <stdlib.h>
#define NETH
#define DEFINE
#include "var.h"
#include <qtp.h>
#include <qtl_tailq.h>



#define VER_LINK query
#define VER_LINK_SIZE 257
#define VER_HASH query+257+1
#define VER_HASH_SIZE 65

#define REG_LOGIN query
#define REG_LOGIN_SIZE 30
#define REG_PASS query+30+1
#define REG_PASS_SIZE 30
#define REG_PASS2 query+60+2
#define REG_PASS2_SIZE 30
#define REG_EMAIL query+90+3
#define REG_EMAIL_SIZE 100
#define REG_NAME query+190+4
#define REG_NAME_SIZE 128
#define REG_COUNTRY query+318+5
#define REG_COUNTRY_SIZE 64
#define REG_CITY query+382+6
#define REG_CITY_SIZE 64
#define REG_GENDER query+446+7
#define REG_GENDER_SIZE 1
#define REG_BYEAR query+448+8
#define REG_BYEAR_SIZE 4
#define REG_BMONTH query+452+9
#define REG_BMONTH_SIZE 2
#define REG_BDAY query+454+10
#define REG_BDAY_SIZE 2
#define REG_RACE query+456+11
#define REG_RACE_SIZE 1
#define REG_CAP query+458+12
#define REG_CAP_SIZE 8




TQ_DEFINE(action)
        char *buf;
        int len;
        int offset;
TQ_DEFINE_END(action)

void *action_root=NULL;
unsigned int len;
char query[BLOCK+1];
char * ptr;


int receiver(int code) {
   rawnet_clear();
   id=net_recv();
   if(id==0) {
        return FALSE;
   } 
   else if(id == ACCESS_ERROR) {
	error_500();
        cgiWriteStr("<debug><msg>access error</msg></debug>\n");
	LOGOUT;
	return FALSE;
   } else if(id == PLACE_ERROR) {
    	cgiWriteStr("<error><place>wrong</place></error>");	   
        cgiWriteStr("<debug><msg>placeerror</msg></debug>\n");
	LOGOUT;
	return FALSE;
   } else if(id != code) {
    	cgiWriteStr("<debug><msg>id!=code</msg></debug>");
	cgiprintf("<code><id>%x</id><server_id>%x</server_id></code>",id,code);
	LOGOUT;
	return FALSE;
   }
   return TRUE;
}


int status() {
   rawnet_clear();
   rawnet_add_int(uid);
   net_send_raw(request(USER_STATUS),rawnet_box(),rawnet_len());
   if(!receiver(reply(USER_STATUS))) return -1;
   rawnet_get(net_box(),net_len());
   return rawnet_get_int();
}
		  
		  
int check_account(char *login) {
   net_send(request(REG_CHECK),login);
   if(!receiver(reply(REG_CHECK))) return -1;
   rawnet_get(net_box(),net_len());
   return rawnet_get_int();
}


int reg_account() {

   rawnet_clear();
   rawnet_add_char(REG_LOGIN);
   rawnet_add_char(REG_PASS);
   rawnet_add_char(REG_EMAIL);
   rawnet_add_char(REG_NAME);
   rawnet_add_char(REG_COUNTRY);
   rawnet_add_char(REG_CITY);
   rawnet_add_char(REG_GENDER);
   rawnet_add_char(REG_BYEAR);
   rawnet_add_char(REG_BMONTH);
   rawnet_add_char(REG_BDAY);
   rawnet_add_char(REG_RACE);
   net_send_raw(request(REG_USER),rawnet_box(),rawnet_len());
   if(!receiver(reply(REG_USER))) return -1;
   rawnet_get(net_box(),net_len());
   mail_reg(REG_EMAIL,REG_LOGIN,REG_PASS);
   return rawnet_get_int();


}


int char_check(char *login) {

   rawnet_clear();
   rawnet_add_int(uid);
   rawnet_add_char(login);
   net_send_raw(request(REG_CHAR_CHECK),rawnet_box(),rawnet_len());

   if(!receiver(reply(REG_CHAR_CHECK))) return -1;
   rawnet_get(net_box(),net_len());
   return rawnet_get_int();
}



int char_add() {

   rawnet_clear();
   rawnet_add_int(uid);
   rawnet_add_char(REG_LOGIN);
   net_send_raw(request(REG_CHAR),rawnet_box(),rawnet_len());
   if(!receiver(reply(REG_CHAR))) return -1;
   rawnet_get(net_box(),net_len());
   return rawnet_get_int();


}

int char_del() {

   rawnet_clear();
   rawnet_add_int(uid);
   rawnet_add_char(REG_LOGIN);
   net_send_raw(request(CHAR_DEL),rawnet_box(),rawnet_len());
   if(!receiver(reply(CHAR_DEL))) return -1;
   rawnet_get(net_box(),net_len());
   return rawnet_get_int();


}


int cgiParser() {

	    if(!memscmp(query,"pre_register")) {
		    cgiTop();
		    if((ptr=qsm_get_param("Ccode")) == NULL || cgiFormString ("captcha",REG_CAP,REG_CAP_SIZE+1) || strcmp(REG_CAP,ptr)) {
			    cgiWriteEnd("<register><captcha>error</captcha></register>");	   
		    } else if(cgiFormString ("login",REG_LOGIN,REG_LOGIN_SIZE+1) || !rx_check_login(REG_LOGIN)) {
			    cgiWriteEnd("<register><login>error</login></register>");	   
		    } else if(cgiFormString ("email",REG_EMAIL,REG_EMAIL_SIZE+1)  || !rx_check_mail(REG_EMAIL)) {
			    cgiWriteEnd("<register><email>error</email></register>");	   
		    } else if((id=check_account(query)) || id < 0){
			    cgiWriteEnd("<register><status>exists</status></register>");	   
		    } else if(id == 0){
			    mail_pre_reg(REG_EMAIL,REG_LOGIN,getenv("REMOTE_ADDR"));
			    cgiprintf("<register><status>ok</status></register>");	   
		    }
		    cgiDown();

	    //request call=check
	    } else if(!memscmp(query,"check") && !cgiFormString ("login",query,BLOCK-1)){
		    cgiTop();
                    id=check_account(query);
                    if(id < 0) {
		    } else if(id) {
	        	cgiWriteStr("<check><login>exists</login></check>");
		    }  else {
	                cgiWriteStr("<check><login>free</login></check>");
		    }
		    cgiDown();
	    //request call=verify
	    } else if(!memscmp(query,"verify")) {
		    if(cgiFormString ("Verify",VER_LINK,VER_LINK_SIZE+1) || cgiFormString ("Hash",VER_HASH,VER_HASH_SIZE+1) ||  checkLink(VER_LINK,VER_HASH) < 1) {
			    qsm_del_param("reg_login");
			    qsm_del_param("reg_email");
			    cgiHeaderLocation("/registration_fail.html");
			    return 0;
		    } 
		    qsm_set_param("reg_login",getLogin());
		    qsm_set_param("reg_email",getMail());
		    cgiHeaderLocation("/registration_final.html");
		    return 0;
	    
	    //request call=get_verify
	    } else if(!memscmp(query,"get_verify")) {
		    if(qsm_get_param("reg_email") == NULL || qsm_get_param("reg_login") == NULL)  {
			    reload("/registration.html");
			    return 0;
		    } 
		    cgiTop();
		    cgiprintf("<verify><login>%s</login></verify>",qsm_get_param("reg_login"));
		    cgiprintf("<verify><email>%s</email></verify>",qsm_get_param("reg_email"));
		    cgiDown();

		    return 0;
	    //request call=char_check
	    } else if(!memscmp(query,"char_check") && !cgiFormString ("login",query,BLOCK-1)){
		    cgiTop();
                    id=char_check(query);
                    if(id < 0) {
			
		    }  else if(id) {
	        	cgiWriteStr("<char><login>exists</login></char>");
		    }   else {
	                cgiWriteStr("<char><login>free</login></char>");
		    }
	    
    	    	cgiDown();
	    //request call=char_add
	    } else if(!memscmp(query,"char_add") && uid > 0) {
		    
		    cgiTop();
		    if(cgiFormString ("login",REG_LOGIN,REG_LOGIN_SIZE+1) || !rx_check_login(REG_LOGIN)) {
			    cgiWriteStr("<char><login>error</login></char>");	   
		    } else if((id=char_check(query)) || id < 0){
			    cgiWriteStr("<char><status>exists</status></char>");	   
		    } else if(id == 0){
			
			    char_add();
			    cgiprintf("<char><status>ok</status></char>");	   
		    }
    	    	cgiDown();

	    //request call=char_del
	    } else if(!memscmp(query,"char_del") && uid > 0) {
		    
		    cgiTop();
		    if(cgiFormString ("char",REG_LOGIN,REG_LOGIN_SIZE+1) || !rx_check_login(REG_LOGIN)) {
			    cgiWriteStr("<status>failed</status>");	   
		    } else if(!char_del()){
			    cgiWriteStr("<status>failed</status>");	   
		    } else {
			    cgiWriteStr("<char><status>ok</status></char>");	   
		    }
    	    	cgiDown();

	    } else if(!memcmp(query,"register",6)) {
		    if(qsm_get_param("reg_email") == NULL || qsm_get_param("reg_login") == NULL)  {
			    reload("/registration.html");
			    return 0;
		    } 
		    cgiTop();
		    snprintf(REG_LOGIN,REG_LOGIN_SIZE,"%s",qsm_get_param("reg_login"));
		    snprintf(REG_EMAIL,REG_EMAIL_SIZE,"%s",qsm_get_param("reg_email"));
		    
		    if(cgiFormString ("password",REG_PASS,REG_PASS_SIZE+1) || !rx_check_password(REG_PASS)) {
			    cgiWriteStr("<register><password>error</password></register>");	   
		    } else  if(cgiFormString ("password2",REG_PASS2,REG_PASS2_SIZE+1) || strcmp(REG_PASS2,REG_PASS)) {
			    cgiWriteStr("<register><password2>error</password2></register>");	   
		    } else  if(cgiFormString ("name",REG_NAME,REG_NAME_SIZE+1)  || !rx_check_name(REG_NAME)) {
			    cgiWriteStr("<register><name>error</name></register>");	   
		    } else  if(cgiFormString ("country",REG_COUNTRY,REG_COUNTRY_SIZE+1) || !rx_check_country(REG_COUNTRY) ) {
			    cgiWriteStr("<register><country>error</country></register>");	   
	    	    } else  if(cgiFormString ("city",REG_CITY,REG_CITY_SIZE+1) || !rx_check_city(REG_CITY)) {
			    cgiWriteStr("<register><city>error</city></register>");	   
		    } else  if(cgiFormString ("gender",REG_GENDER,REG_GENDER_SIZE+1) || !rx_check_gender(REG_GENDER)) {
			    cgiWriteStr("<register><gender>error</gender></register>");	   
		    } else  if(cgiFormString ("byear",REG_BYEAR,REG_BYEAR_SIZE+1) || !rx_check_byear(REG_BYEAR)) {
			    cgiWriteStr("<register><byear>error</byear></register>");	   
		    } else  if(cgiFormString ("bmonth",REG_BMONTH,REG_BMONTH_SIZE+1) || !rx_check_bmonth(REG_BMONTH)) {
			    cgiWriteStr("<register><bmonth>error</bmonth></register>");	   
		    } else  if(cgiFormString ("bday",REG_BDAY,REG_BDAY_SIZE+1) || !rx_check_bday(REG_BDAY)) {
			    cgiWriteStr("<register><bday>error</bday></register>");	   
		    } else  if(cgiFormString ("race",REG_RACE,REG_RACE_SIZE+1) || !rx_check_race(REG_RACE)) {
			    cgiWriteStr("<register><race>error</race></register>");	   
		    } else if((id=check_account(query)) || id < 0){
			    cgiWriteStr("<register><status>exists</status></register>");	   
		    } else if(id == 0){
			    reg_account();
			    cgiprintf("<register><status>ok</status></register>");	   
		    }
    	    	cgiDown();

	    //был вызван не известный природе call или не прошли нужные условия
	     } else  {
    		 cgiTop();
		 cgiprintf("<debug><uid>%d</uid><uidadr>%x</uidadr></debug>",uid,qsm_get_param("uid"));
		 LOGOUT;
    	    	cgiDown();
		 
		 return FALSE;

	     }

    return TRUE;
}



int cgiInit () {

	LISTEN_IP=NULL;
	qsm_init("/tmp/","myproject",1200);
	TQ_INIT(action);
	return 0;

}




int cgiMain () {

	if(LISTEN_IP == NULL) {
		if(getenv("CHAT_IP") != NULL) LISTEN_IP=getenv("CHAT_IP"); else LISTEN_IP=strdup("127.0.0.1");
		if(getenv("CHAT_PORT") != NULL) LISTEN_PORT=atoi(getenv("CHAT_PORT")); else LISTEN_PORT=666;
         	MASTER_KEY=(*(int *)"NANA");
		len=net_init_client();
	}



	qsm_init_session();
	//if(!qsm_init_session()) { reload("/cgi-bin/set_cookie.fcgi"); return 0;}
	if((ptr=qsm_get_param("uid")) != NULL)  uid=atoi(ptr);  else uid=0;
	cgiWriteEnvironment(".save");

    if(!cgiFormString ("call",query,BLOCK-1))  {
	    cgiParser();
	    return 0;	     
    }
    
    error_404();
    return 0;

}





