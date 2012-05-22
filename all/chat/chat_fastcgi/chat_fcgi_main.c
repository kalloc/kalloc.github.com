#include <stdio.h>
#include <stdlib.h>
#define NETH
#define DEFINE
#include "var.h"
#include <qtp.h>
#include <qtl_tailq.h>

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
   if(id==0) 
        return FALSE;
   else if(id == ACCESS_ERROR) {
        cgiWriteStr("<debug><msg>access error</msg></debug>\n");LOGOUT;
	return FALSE;
   } else if(id == PLACE_ERROR) {
        cgiWriteStr("<error><place>wrong</place></error>\n<debug><msg>placeerror</msg></debug>\n");LOGOUT;
	return FALSE;
   } else if(id != code) {
	LOGOUT;
	return FALSE;
   }
   return TRUE;
}
//account action

int user_check_account(char *login) {
   net_send(request(REG_CHECK),login);
   if(!receiver(reply(REG_CHECK))) return -1;
   rawnet_get(net_box(),net_len());
   return rawnet_get_int();
}

int user_login(char *login,char *password) {
   rawnet_clear();
   rawnet_add_char(login);
   rawnet_add_char(password);
   net_send_raw(request(USER_LOGIN),rawnet_box(),rawnet_len());
   if(!receiver(reply(USER_LOGIN))) return -1;
   rawnet_get(net_box(),net_len());
   return rawnet_get_int();
}

int guest_login() {
   rawnet_clear();
   rawnet_add_char("guest");
   net_send_raw(request(GUEST_LOGIN),rawnet_box(),rawnet_len());
   if(!receiver(reply(GUEST_LOGIN))) return -1;
   rawnet_get(net_box(),net_len());
   return rawnet_get_int();
}

int user_logout() {
   rawnet_clear();
   rawnet_add_int(uid);
   net_send_raw(request(USER_LOGOUT),rawnet_box(),rawnet_len());
   if(!receiver(reply(USER_LOGOUT))) return -1;
   rawnet_get(net_box(),net_len());
   return rawnet_get_int();
}

int status() {
   rawnet_clear();
   rawnet_add_int(uid);
   net_send_raw(request(USER_STATUS),rawnet_box(),rawnet_len());
   if(!receiver(reply(USER_STATUS))) return -1;
   rawnet_get(net_box(),net_len());
   return rawnet_get_int();
}

//chat action


int user_join(int place) {
   rawnet_clear();
   rawnet_add_int(uid);
   rawnet_add_int(place);
   net_send_raw(request(USER_JOIN),rawnet_box(),rawnet_len());
   if(!receiver(reply(USER_JOIN))) return -1;
   rawnet_get(net_box(),net_len());
   return rawnet_get_int();
}


void place_list() {

   rawnet_clear();
   rawnet_add_int(uid);
   net_send_raw(request(USER_PLACE_LIST),rawnet_box(),rawnet_len());

   if(!receiver(reply(USER_PLACE_LIST))) {
        cgiWriteStr("<debug><msg>list place error</msg></debug>\n");
	LOGOUT;
        return;
   }

   cgiWriteStr("<chat>\n"); 

   do {
   rawnet_get(net_box(),net_len());
   ptr=rawnet_get_char();
   cgiprintf("<place><name><![CDATA[%s]]></name><users>%d</users><id>%d</id></place>\n",ptr,rawnet_get_int(),rawnet_get_int());

   } while(receiver(reply(USER_PLACE_LIST)));
   
   cgiWriteStr("</chat>"); 

}


void get_user() {

   rawnet_clear();
   rawnet_add_int(uid);
   net_send_raw(request(USER_LIST),rawnet_box(),rawnet_len());

   
   if(!receiver(reply(USER_LIST))) {
        cgiWriteStr("<debug><msg>list user error</msg></debug>\n");
	LOGOUT;
        return;
   }

   cgiWriteStr("<chat>\n"); 
   do {
       rawnet_get(net_box(),net_len());
       ptr=rawnet_get_char();
       len=snprintf(query,BLOCK-1,"<user><name><![CDATA[%s]]></name><ping>%d</ping><id>%d</id></user>\n",ptr,rawnet_get_int(),rawnet_get_int());
       cgiWrite(query,len); 
   } while(receiver(reply(USER_LIST)));
   cgiWriteStr("</chat>"); 

}

void place_get_message() {

   rawnet_clear();
   rawnet_add_int(uid);
   net_send_raw(request(GET_MESSAGE),rawnet_box(),rawnet_len());

   if(!receiver(reply(GET_MESSAGE))) {
        return;
   }


   cgiWriteStr("<chat>\n"); 
   do {
       rawnet_get(net_box(),net_len());
       len=snprintf(query,BLOCK-1,"<msg>"
				"<time>%d</time>"
				"<private>%d</private>"
				"<string><![CDATA[%s]]></string>"
    				"<name><![CDATA[%s]]></name>"
				"</msg>\n",
				rawnet_get_int(),
				rawnet_get_int(),
				rawnet_get_char(),
				rawnet_get_char()
				);
       cgiWrite(query,len); 
   }   while(receiver(reply(GET_MESSAGE)));
   cgiWriteStr("</chat>"); 

}


void place_add_message(char *msg) {

   rawnet_clear();
   rawnet_add_int(uid);
   rawnet_add_char(msg);
   net_send_raw(request(ADD_MESSAGE),rawnet_box(),rawnet_len());


   if(!receiver(reply(GET_MESSAGE))) {
        return;
   }


   cgiWriteStr("<chat>\n"); 
   do {
       rawnet_get(net_box(),net_len());
       len=snprintf(query,BLOCK-1,"<msg>"
				"<time>%d</time>"
				"<private>%d</private>"
				"<string><![CDATA[%s]]></string>"
    				"<name><![CDATA[%s]]></name>"
				"</msg>\n",
				rawnet_get_int(),
				rawnet_get_int(),
				rawnet_get_char(),
				rawnet_get_char()
				);
       cgiWrite(query,len); 
   }   while(receiver(reply(GET_MESSAGE)));
   cgiWriteStr("</chat>"); 

}

void place_add_private(char *msg) {

   rawnet_clear();
   rawnet_add_int(uid);
   rawnet_add_char(msg);
   net_send_raw(request(ADD_PRIVATE),rawnet_box(),rawnet_len());


   if(!receiver(reply(GET_MESSAGE))) {
        return;
   }


   cgiWriteStr("<chat>\n"); 
   do {
       rawnet_get(net_box(),net_len());
       len=snprintf(query,BLOCK-1,"<msg>"
				"<time>%d</time>"
				"<private>%d</private>"
				"<string><![CDATA[%s]]></string>"
    				"<name><![CDATA[%s]]></name>"
				"</msg>\n",
				rawnet_get_int(),
				rawnet_get_int(),
				rawnet_get_char(),
				rawnet_get_char()
				);
       cgiWrite(query,len); 
   }   while(receiver(reply(GET_MESSAGE)));
   cgiWriteStr("</chat>"); 


}




//char action 

void char_list() {

   rawnet_clear();
   rawnet_add_int(uid);
   net_send_raw(request(CHAR_LIST),rawnet_box(),rawnet_len());


    if(!receiver(reply(CHAR_LIST))) {
	cgiprintf("<debug><msg>list char error</msg></debug>\n");
	LOGOUT;
        return;
    }

    rawnet_get(net_box(),net_len());
    id=rawnet_get_int();
   

    cgiWriteStr("\n<chars>\n"); 
    id=rawnet_get_int();
    cgiprintf("<count>%d</count>\n",id);

    if(id>0) {
        while(receiver(reply(CHAR_LIST))) {
	    rawnet_get(net_box(),net_len());
	    cgiprintf("<char><id>%d</id><name><![CDATA[%s]]></name><prof>%d</prof></char>\n",rawnet_get_int(),rawnet_get_char(),rawnet_get_int());
	}
    } 

   cgiWriteStr("</chars>"); 
   
}


int char_select(int cid) {
   rawnet_clear();
   rawnet_add_int(uid);
   rawnet_add_int(cid);
   net_send_raw(request(CHAR_SELECT),rawnet_box(),rawnet_len());
   if(!receiver(reply(CHAR_SELECT))) return -1;
   rawnet_get(net_box(),net_len());
   return rawnet_get_int();
}


int cgiParser() {
	    //request call=status
	    if(!memscmp(query,"status") && uid > 0){
	        id=status();
		if(id < 0) { 
			return FALSE;
		} 

		len=snprintf(query,BLOCK-1,"<check><place>%d</place></check>",id);
		if ( id == 0 ) place_list();
	        cgiWrite(query,len); 

	    //request call=list
	    } else if(!memscmp(query,"getplace") && uid > 0){
		place_list();
	    //request call=getuser
	    } else if(!memscmp(query,"getuser") && uid > 0){
		get_user();
	    //request call=getmsg
	    } else if(!memscmp(query,"getmsg") && uid > 0){
		place_get_message();
	    //request call=fresh
	    } else if(!memscmp(query,"fresh") && uid > 0){
		get_user();
		if(uid>0) place_get_message();
	    //request call=addmsg
	    } else if(!memscmp(query,"addmsg") && !cgiFormString ("msg",query,150) && uid > 0){
		if(!memcmp(query,"/MSG ",5)) {
			place_add_private(query+5);
		} else {
			place_add_message(query);
		}
	    //request call=join
	    } else if(!memscmp(query,"join")  && !cgiFormString ("place",query,3) && uid > 0){
		id=atoi(query);
	        user_join(id);
                cgiWriteStr("<action>join</action>");
		cgiprintf("<place_id>%s</place_id>",query); 
	    //request call=logout
	    } else if(!memscmp(query,"logout") ){
		LOGOUT
		return FALSE;	
	    //request call=login
            } else if(!memscmp(query,"login")  && !cgiFormString ("login",REG_LOGIN,REG_LOGIN_SIZE)  && !cgiFormString ("password",REG_PASS,REG_PASS_SIZE)){
		if(!rx_check_login(REG_LOGIN) &&  !rx_check_password(REG_PASS)) {
		    id=0;    
		} else {
		    id=user_login(REG_LOGIN,REG_PASS);
		}
		if(id <= 0) {
	     		LOGIN_DENIED
			return FALSE;
		} else {
	                cgiWriteStr("<action>char_select</action>");
			uid=id;
			snprintf(query,BLOCK-1,"%d",id);
			qsm_set_param("uid",query);
			char_list();			
			return FALSE;
		}
	    //request call=char_list
            } else if(!memscmp(query,"char_list") && uid > 0){
	        cgiWriteStr("<action>char_select</action>");
		char_list();			
		return FALSE;
	    //request call=char_select
            } else if(!memscmp(query,"char_select")  && !cgiFormString ("login",REG_LOGIN,REG_LOGIN_SIZE)  && uid > 0){
		id=char_select(atoi(REG_LOGIN));
		if(id <= 0) {
	     		CHAR_DENIED
			return FALSE;
		} else {
			CHAR_OK
			snprintf(query,BLOCK-1,"%d",id);
			qsm_set_param("cid",query);
			return FALSE;
		}
	    //request call=login_guest
            } else if(!memscmp(query,"login_guest") ){
		id=guest_login();
		if(id < 0) {
	     	    LOGIN_DENIED
		    return FALSE;
		} else {
		    CHAR_OK
		    snprintf(query,BLOCK-1,"%d",id);
		    qsm_set_param("uid",query);
		}
	    //был вызван не известный природе call или не прошли нужные условия
	     } else  {
		 cgiprintf("<debug><uid>%d</uid><uidadr>%x</uidadr></debug>",uid,qsm_get_param("uid"));
		 LOGOUT;
	    	 return FALSE;


	     }
	    return 0;	     



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
	    cgiTop();
	    cgiParser();
	    cgiDown();
	    return 0;
        }

    error_404();
    return 0;

}





