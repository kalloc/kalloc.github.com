#include <stdio.h>
#include <stdlib.h>
#define NETH
#define DEFINE
#include "var.h"
#include <qtl_tailq.h>

char buffer[MAXLINE];





int counter=0;
time_t time_start,now_start,now_end,now;

int uptime() {


    return (int)(time(NULL)-time_start);


}


void stat_cps() {
	int l;
	now_end = time(NULL);
	l=(int)(now_end-now_start);
	if(l==0) l=1;
	snprintf(buffer,MAXLINE-1,"%d",(int)(counter/l));
	net_send(reply(CPS),buffer);
	return ;
}

void send_uptime() {
	snprintf(buffer,MAXLINE-1,"%d",uptime());
	net_send(reply(UPTIME),buffer);
	return ;
}
void send_exit() {
	snprintf(buffer,MAXLINE-1,"%d",uptime());
	net_send(reply(EXIT),buffer);
	exit(0);
}

void stat_mrtg() {
	int l;
	now_end = time(NULL);
	l=(int)(now_end-now_start);
	if(l==0) l=1;
	snprintf(buffer,MAXLINE-1,"%d %d",(int)now_end,(int)(counter/l));
	net_send(reply(MRTG),buffer);
	now_start=now_end;
	counter=0;
	return ;
}

void hexdump(char * str,int len) {
    int i;
    for(i=0;i <= len;i++) {
	printf("0x%x(%c) ",str[i],str[i]);
    
    }
    printf("\n");

}
	unsigned int len;
	char * login=NULL;
	char * password=NULL;


void net_loop() {


//	print("key:%d\n",MASTER_KEY);		
	while(1) { 

		if ((id=net_recv()) == 0 || id < 0 || (len=net_len()) <= 0)  {
//		    	print("key:%d\n",MASTER_KEY);		
			continue;
		}

		counter++;

		rawnet_clear();
	//	hexdump(net_box(),len);
		switch(id) {
			//account action
			case request(USER_LOGIN):
				rawnet_get(net_box(),len);
				login=rawnet_get_char();
				password=rawnet_get_char();
				print("ID: USER_LOGIN request, Login: %s, Pass: %s",login,password);		
				sid=user_add(login,password);
				rawnet_clear();
				rawnet_add_int(sid);
				net_send_raw(reply(USER_LOGIN),rawnet_box(),rawnet_len());
				break;	

			case request(GUEST_LOGIN):
				rawnet_get(net_box(),len);
				print("ID: GUEST_LOGIN request");		
				sid=user_guest_add();
				rawnet_clear();
				rawnet_add_int(sid);
				net_send_raw(reply(GUEST_LOGIN),rawnet_box(),rawnet_len());
				break;	


			case request(USER_LOGOUT):
				print("ID: USER_LOGOUT request, buffer:%s %d",net_box(),len);
				rawnet_get(net_box(),len);
				if(user_check_access(rawnet_get_int()) == -1) break;
				id=user_del();
				rawnet_clear();
				rawnet_add_int(id);
				net_send_raw(reply(USER_LOGOUT),rawnet_box(),rawnet_len());
				break;	

			case request(REG_CHECK):
				print("ID: REG_CHECK request, buffer:%s %d",net_box(),len);		
				rawnet_get(net_box(),len);
				login=rawnet_get_char();
				id=db_check_account(login);
				rawnet_clear();
				rawnet_add_int(id);
				net_send_raw(reply(REG_CHECK),rawnet_box(),rawnet_len());
				break;	

			case request(REG_USER):
				print("ID: REG_USER request, buffer:%s %d",net_box(),len);		
				rawnet_get(net_box(),len);


				id=db_reg_user(
				    rawnet_get_char(),
				    rawnet_get_char(),
				    rawnet_get_char(),
				    rawnet_get_char(),
				    rawnet_get_char(),
				    rawnet_get_char(),
				    rawnet_get_char(),
				    rawnet_get_char(),
				    rawnet_get_char(),
				    rawnet_get_char(),
				    rawnet_get_char()
				   );
				rawnet_clear();
				rawnet_add_int(id);
				net_send_raw(reply(REG_USER),rawnet_box(),rawnet_len());
				break;	


			case request(USER_JOIN):
				print("ID: USER_JOIN request");
				rawnet_get(net_box(),len);
				if(user_check_access(rawnet_get_int()) == -1) break;
				id=user_join(rawnet_get_int());
				rawnet_clear();
				rawnet_add_int(id);
				net_send_raw(reply(USER_JOIN),rawnet_box(),rawnet_len());
				break;	

			case request(USER_STATUS):
				print("ID: USER_STATUS request for sid %d",*(int*)net_box());
				rawnet_get(net_box(),len);
				if((id=user_status(rawnet_get_int())) == -1) break;
				print("ID: USER_STATUS reply for sid %d",id);

				rawnet_clear();
				rawnet_add_int(id);
				net_send_raw(reply(USER_STATUS),rawnet_box(),rawnet_len());
				break;	

			case request(USER_LIST):
				print("ID: USER_LIST request for sid %u",*(int*)net_box());		
				rawnet_get(net_box(),len);
				if(user_check_access(rawnet_get_int()) || place_check()) break;
				rawnet_clear();
				get_user_list();
				break;	

			//char action
			case request(CHAR_LIST):
				print("ID: CHAR_LIST request for sid %d",*(int*)net_box());		
				rawnet_get(net_box(),len);
				if(user_check_access(rawnet_get_int()) == -1) break;
				get_char_list();
				break;	
			case request(CHAR_SELECT):
				print("ID: CHAR_SELECT request for sid %d",*(int*)net_box());		
				rawnet_get(net_box(),len);
				if(user_check_access(rawnet_get_int()) == -1) break;
				id=char_select(rawnet_get_int());
				rawnet_clear();
				rawnet_add_int(id);
				net_send_raw(reply(CHAR_SELECT),rawnet_box(),rawnet_len());
				break;	
			case request(REG_CHAR):
				print("ID: REG_CHAR request for sid %d",*(int*)net_box());		
				rawnet_get(net_box(),len);
				if(user_check_access(rawnet_get_int()) == -1) break;
				id=db_reg_char(rawnet_get_char(),uid);
    				rawnet_clear();
				rawnet_add_int(id);
				net_send_raw(reply(REG_CHAR),rawnet_box(),rawnet_len());

				break;	

			case request(CHAR_DEL):
				print("ID: CHAR_DEL request for sid %d",*(int*)net_box());		
				rawnet_get(net_box(),len);
				if(user_check_access(rawnet_get_int()) == -1) break;
				if((id=db_char_check_login(rawnet_get_char(),uid))==0) {
				    id=0;
				} else {
				    db_char_del(id);
	
				}
    				rawnet_clear();
				rawnet_add_int(id);
				net_send_raw(reply(CHAR_DEL),rawnet_box(),rawnet_len());

				break;	

			case request(REG_CHAR_CHECK):
				print("ID: REG_CHAR_CHECK request for sid %d",*(int*)net_box());		
				rawnet_get(net_box(),len);
				if(user_check_access(rawnet_get_int()) == -1) break;
				id=db_char_check_login(rawnet_get_char(),0);
				rawnet_clear();
				rawnet_add_int(id);
				net_send_raw(reply(REG_CHAR_CHECK),rawnet_box(),rawnet_len());

				break;	
				
				
			//chat action
			case request(USER_PLACE_LIST):
				print("ID: USER_PLACE_LIST request for uid %d",*(int*)net_box());		
				rawnet_get(net_box(),len);
				if(user_check_access(rawnet_get_int()) == -1) break;
				get_place_list();
				break;	

			case request(ADD_MESSAGE):
				print("ID: ADD_MESSAGE request, buffer:%s %d",net_box()+4,len);
				rawnet_get(net_box(),len);
				if(user_check_access(rawnet_get_int()) || place_check()) break;
				place_add_msg(rawnet_get_char());
				place_get_msg();
				break;	

			case request(ADD_PRIVATE):
				print("ID: ADD_PRIVATE request, buffer:%s %d",net_box()+4,len);
				rawnet_get(net_box(),len);
				if(user_check_access(rawnet_get_int()) || place_check()) break;
				place_add_pvt(rawnet_get_char());
				place_get_msg();
				break;	

			case request(GET_MESSAGE):
				print("ID: GET_MESSAGE request");
				rawnet_get(net_box(),len);
				if(user_check_access(rawnet_get_int()) || place_check()) break;
				place_get_msg();
				break;	

			//stat
			case request(CPS):
				print("ID: CPS request, buffer:%s %d",net_box(),len);
				stat_cps();
				break;

			case request(MRTG):
				print("ID: MRTG request, buffer:%s %d",net_box(),len);
				stat_mrtg();
				break;

			case request(EXIT):
				print("ID: EXIT request, buffer:%s %d",net_box(),len);
				send_exit();
				break;

			case request(UPTIME):
				print("ID: UPTIME request");
				send_uptime();
				break;

        		default:
				print("ID: Unknow (0x%x) request, buffer:%s",id,net_box());		
				break;

		}


	}


}

     
int main (int argc , char **argv){
	time_start=time(NULL);

	print("init config");

	init_config(argc,argv);

	now_start = time(NULL);

	init_users();
	db_init();
	net_init_server();
	net_loop();
	print("exit");
	exit (0);
}
