#define NETH

#define DEFINE
#include "var.h"
#include <qtl_tailq.h>


//Expire time
int expire_time=60;

//define message list
typedef struct {
	unsigned int id;
	unsigned int pvt;
	time_t time;
	char name[NAME_SIZE+1];
	char string[MAXLINE+1];
} msgs_struct;



//define places list
TQ_DEFINE(places)
	short int id;
	short int mid;
	short int count_user;
	char name[NAME_SIZE+1];
	msgs_struct msgs [MAX_MSG_SIZE+1];
TQ_DEFINE_END(places)

void *places_root=NULL;
static int place_max_id=0;



//Define user
TQ_DEFINE(users)
	unsigned int uid;
	unsigned int sid;
	unsigned int cid;
	unsigned int cprof;
	short int pid;
	short int status;
	short int last_msg_id;
	TQ_VAR(places) *place;
	char login[NAME_SIZE+1];
	char name[NAME_SIZE+1];
	time_t last_time;
	time_t last_read_time;
TQ_DEFINE_END(users)

void *users_root_uid=NULL;
void *users_root_login=NULL;
void *users_root_name=NULL;
void *users_root_sid=NULL;



TQ_VAR(users) *current_user;
time_t now;


void print_node (arg, order, level)	void **arg;	VISIT order;	int level;	{
   if (order == preorder || order == leaf) {
	  TQ_VAR(places) *node;
	  node=(TQ_VAR(places) *)*arg;
   }
}



/****************************************************

	    tsearch

*****************************************************/

int id_compare(const void * left,const void * right) {

	if(((TQ_VAR(places) *)left)->id < ((TQ_VAR(places) *)right)->id)
		return -1;
	else if(((TQ_VAR(places) *)left)->id > ((TQ_VAR(places) *)right)->id)
		return 1;
	else
		return 0;
}

int name_compare(const void *pa, const void *pb) {

	TQ_VAR(places) *left, *right;
	left = (TQ_VAR(places) *)pa;
	right = (TQ_VAR(places) *)pb;
	return (strcmp(left->name,right->name));
}



int users_name_compare(const void *pa, const void *pb) {
	TQ_VAR(users) *left, *right;
	left = (TQ_VAR(users) *)pa;
	right = (TQ_VAR(users) *)pb;
	return (strcmp(left->name,right->name));
}

int users_login_compare(const void *pa, const void *pb) {
	TQ_VAR(users) *left, *right;
	left = (TQ_VAR(users) *)pa;
	right = (TQ_VAR(users) *)pb;
	return (strcmp(left->login,right->login));
}


int users_uid_compare(const void *pa, const void *pb) {

	TQ_VAR(users) *left, *right;
	left = (TQ_VAR(users) *)pa;
	right = (TQ_VAR(users) *)pb;
	if(left->uid<right->uid)
		return -1;
	else if(left->uid>right->uid)
		return 1;
	else
		return 0;
}

int users_sid_compare(const void *pa, const void *pb) {

	TQ_VAR(users) *left, *right;
	left = (TQ_VAR(users) *)pa;
	right = (TQ_VAR(users) *)pb;
	if(left->sid<right->sid)
		return -1;
	else if(left->sid>right->sid)
		return 1;
	else
		return 0;
}




void walk_get_place_list(TQ_VAR(places) **node,VISIT order,int level)	 {
	if (order == preorder || order == leaf) {
		rawnet_clear();
		rawnet_add_char	((*node)->name);
		rawnet_add_int	((*node)->id);
		rawnet_add_int	((*node)->count_user);
		net_send_raw(reply(USER_PLACE_LIST),rawnet_box(),rawnet_len());
		
	}
}


void get_place_list() {
	twalk((void *)places_root,(void *)walk_get_place_list);
}



void walk_get_user_list(TQ_VAR(users) **node,VISIT order,int level)	 {
	if ((order == preorder || order == leaf) && (*node)->pid == current_user->pid) {
		rawnet_clear();
		rawnet_add_char((*node)->name);
		rawnet_add_int((*node)->uid);
		rawnet_add_int((int)(now-(*node)->last_time));
		net_send_raw(reply(USER_LIST),rawnet_box(),rawnet_len());
	}
}

void get_user_list() {
	twalk((void *)users_root_sid,(void *)walk_get_user_list);
}


TQ_VAR(places) * find_place_by_name(char *name) {

	TQ_VAR(places) **place,query;
	strncpy(query.name,name,NAME_SIZE);

	if ((place=tfind(&query,&places_root,name_compare))) return *place; 
	return 0;
}

TQ_VAR(places) * find_place_by_id(int pid) {

	TQ_VAR(places) **place,query;
	query.id=pid;
	if ((place=tfind(&query,&places_root,id_compare))) {
		return *place;
	}
	return 0;
}


TQ_VAR(users) * check_user_by_name(char *username) {

	TQ_VAR(users) query,**ptr;
	strncpy(query.name,username,NAME_SIZE);
	if ((ptr=tfind(&query,&users_root_name,users_name_compare))) return *ptr;
	return 0;

}

TQ_VAR(users) * check_user_by_id(int uid) {

	TQ_VAR(users) query,**ptr;
	query.uid=uid;
	if ((ptr=tfind(&query,&users_root_uid,users_uid_compare))) 
		return 0;
	return *ptr;
}

/****************************************************

	    account action

*****************************************************/


void expire()
{
	now=time(NULL);
	
	TQ_EXPIRE(users) {

		
//		print("TRY EXPIRE %s [%d:%d]",TQ_ITEM(users)->login,TQ_ITEM(users)->sid,TQ_ITEM(users)->last_time);
		if (TQ_ITEM(users)->last_time+expire_time <= now) {

			if(TQ_ITEM(users)->place  != NULL) {
				TQ_ITEM(users)->place->count_user--;
			}			

			print("EXPIRE %s [%d:%d]",TQ_ITEM(users)->login,TQ_ITEM(users)->sid,TQ_ITEM(users)->uid);
			TQ_REMOVE(users,TQ_ITEM(users));
			tdelete(TQ_ITEM(users),&users_root_name,users_name_compare);
			tdelete(TQ_ITEM(users),&users_root_uid,users_uid_compare);
			tdelete(TQ_ITEM(users),&users_root_sid,users_sid_compare);
			free(TQ_ITEM(users));
		}
		 else break;
	}
}

int user_add(char *username,char *password) {

	TQ_VAR(users) *user,query,**ptr;


        if((uid=db_check_access(username,password)) == 0)  {
		print("user %s, password - %s, wrong",username,password);
		return 0;
        }

	query.uid=uid;
			
	if (ptr=tfind(&query,&users_root_uid,users_uid_compare)) {
		print("user %s, already exists, kick",(*ptr)->login);
		user_del_cu(*ptr);

	}

	user = TQ_MALLOC(users);
	expire();

	bzero(user,sizeof(TQ_VAR(users)));
	strncpy(user->login,username,NAME_SIZE);	

	user->uid = uid;
	user->status = db_account.aStatus;//db_get_status();
	user->sid = (int)user;
	user->last_time= now;

	//в дерево по uid
	tsearch(user,&users_root_uid,users_uid_compare);
	//в дерево по sid
	tsearch(user,&users_root_sid,users_sid_compare);

	TQ_INSERT(users,user);

	current_user=user;
	print("user %s (uid:sid:status/%d:%d:%d), added",username,user->uid,user->sid,user->status);
	return user->sid;
}

int user_guest_add() {

	int count=5;
	TQ_VAR(users) *user;
	expire();
	user = TQ_MALLOC(users);
	bzero(user,sizeof(TQ_VAR(users)));

        snprintf(user->login,NAME_SIZE,"Guest");
	
	do {
	    if(count <=0) return 0;
	    count--;
	    user->uid = (unsigned short)((uptime()+(mt_rand(0,255)<<8))+mt_rand(0,255));
    	    snprintf(user->name,NAME_SIZE,"Guest%d",user->uid);
	} while(tfind(user,&users_root_name,users_name_compare)); 

	user->uid = 1;
	user->cid = 1;
	user->sid = (int)user;
	user->status = 1;

	user->last_time= now;
	//в дерево по uid
	tsearch(user,&users_root_uid,users_uid_compare);
	//в дерево по sid
	tsearch(user,&users_root_sid,users_sid_compare);
	//в дерево по name
	tsearch(user,&users_root_name,users_name_compare);


	TQ_INSERT(users,user);
	current_user=user;
	return user->sid;

}


void refresh_user() {
	TQ_UP(users,current_user);
	current_user->last_time = time(NULL);
}


int user_status(unsigned int sid) {

	expire();

	TQ_VAR(users) query,**ptr;
	query.sid = sid;
	if (!(ptr=tfind(&query,&users_root_sid,users_sid_compare))) {
		rawnet_clear();
		net_send(ACCESS_ERROR,"");
		return -1;
	}

	current_user=*ptr;

	refresh_user();

	if(find_place_by_id(current_user->pid))
		return current_user->pid;
	else
		return 0;

}

int user_check_access(unsigned int sid) {

	expire();

	TQ_VAR(users) query,**ptr;

	query.sid = sid;

	if (!(ptr=tfind(&query,&users_root_sid,users_sid_compare))) {
		rawnet_clear();
		net_send(ACCESS_ERROR,"");
		return -1;
	}
	current_user=*ptr;
	refresh_user();
	return 0;

}



int user_del() {
	user_del_cu(current_user);
	return 0;

}


int user_del_cu(TQ_VAR(users) *cu) {

	if(cu->place != NULL) {
		cu->place->count_user--;
	}			
	TQ_REMOVE(users,cu);
	tdelete(cu,&users_root_uid,users_uid_compare);
	tdelete(cu,&users_root_sid,users_sid_compare);
	tdelete(cu,&users_root_name,users_name_compare);
	free(cu);
	return 0;

}



/****************************************************

	    char action

*****************************************************/


void get_char_list() {
    print("uid:%d, sid:%d, name:%s",current_user->uid,current_user->sid,current_user->login);
    db_char_list(current_user->uid);
}


int char_select(unsigned int  cid) {

	char *name;

        if((db_char_check_id(cid,current_user->uid)) == 0)  {
	    print("ch cid %d, wrong",cid);
	    return 0;
        }

	if(current_user->cid != 0 && current_user->name[0] != 0  && tfind(current_user,&users_root_name,users_name_compare))
	    tdelete(current_user,&users_root_name,users_name_compare);

        print("cid:%d, sid:%d, name:%s",current_user->cid,current_user->sid,db_char.cLogin);
	strncpy(current_user->name,db_char.cLogin,NAME_SIZE);	
	current_user->cid=id;

	//в дерево по char name
	tsearch(current_user,&users_root_name,users_name_compare);
        print("cid:%d, sid:%d, name:%s",current_user->cid,current_user->sid,current_user->name);

	return id;
}


/*

int char_select(char *ch) {

        if((id=db_char_check(ch,current_user->uid)) == 0)  {
		print("ch user %s, wrong",ch);
		return 0;
        }

	if(current_user->ch != NULL) {
		free(TQ_ITEM(characters));
	
	}

	current_user->ch = TQ_MALLOC(characters);
	bzero(&current_user->ch,sizeof(TQ_VAR(characters)));
	strncpy((char *)current_user->ch,ch,NAME_SIZE);	
	current_user->ch->cid=id;
	TQ_INSERT(characters,current_user->ch);
	print("chuser %s %d (uid:sid:status/%d:%d:%d), added",ch,current_user->ch->cid,current_user->uid,current_user->sid,current_user->status);
	return id;

}


int char_select_guest(char *name) {



	TQ_VAR(characters) *ch;
	if(current_user->ch != NULL) {
		tdelete(ch,&ch_root_name,ch_name_compare);
		TQ_REMOVE(characters,current_user->ch);
		free(current_user->ch);
	}

	ch = TQ_MALLOC(characters);
	bzero(ch,sizeof(TQ_VAR(characters)));
	strncpy((char *)ch->name,name,NAME_SIZE);	
	tsearch(ch,&ch_root_name,ch_name_compare);

	ch->cid=1;
	current_user->ch=ch;

	TQ_INSERT(characters,ch);
	print("gchuser %s %d (uid:sid:status/%d:%d:%d), added",ch->name,current_user->ch->cid,current_user->uid,current_user->sid,current_user->status);
	return 0;

}

*/

/****************************************************

	    chat action

*****************************************************/


int place_check() {
	if(!find_place_by_id(current_user->pid)) {
		print("errro this place:%d",current_user->pid);
		rawnet_clear();	net_send(PLACE_ERROR,"");
		return 1;
	}
	return 0;
}

int user_join(int pid) {
	TQ_VAR(places) * place;
	print("ENTER TO %d",pid);
	if((place=find_place_by_id(pid)) == 0)	return 0;
	
	if(current_user->place != NULL) current_user->place->count_user--;
				
	current_user->place=place;
	current_user->place->count_user++;
	current_user->pid=pid;
	current_user->last_msg_id=0;
	current_user->last_read_time=0;
	return 1;	
}

int place_create(char *name) {
	TQ_VAR(places) *place,query;
	query.id=place_max_id+1;

	if (tfind(&query,&places_root,id_compare)) 	return 0;
	else {
		place_max_id++;
		printf("place %s, added\n",name);
		place = TQ_MALLOC(places);
		bzero(place,sizeof(TQ_VAR(places)));
		strncpy(place->name,name,NAME_SIZE);
		place->id = place_max_id;
		tsearch(place,&places_root,id_compare);
		TQ_INSERT(places,place);
		return place->id;
	}
	return 0;
}



int place_get_msg() {
	TQ_VAR(places) *place=current_user->place;
	msgs_struct *msg;

	int stop;
	for(stop=current_user->last_msg_id;place->msgs[stop].time > 0 && place->msgs[stop].time >= current_user->last_read_time;) {
		msg=&place->msgs[stop];
		if(msg->pvt == 0 || msg->pvt == current_user->sid ||  msg->id == current_user->sid)  {
		    print("get [%s/%d]->%s",msg->name,current_user->sid,msg->string);
		    rawnet_clear();
		    rawnet_add_char(msg->name);
		    rawnet_add_char(msg->string);
		    if(msg->pvt == 0)
			rawnet_add_int(0);
		    else if(msg->pvt>0) {
			if(msg->id == current_user->sid)
			    rawnet_add_int(2);
			else if(msg->pvt == current_user->sid)
    			    rawnet_add_int(1);
		    }

		    rawnet_add_int((int)msg->time);
		    net_send_raw(reply(GET_MESSAGE),rawnet_box(),rawnet_len());
		    current_user->last_read_time=msg->time;

		}
		if(stop >= MAX_MSG_SIZE) stop=0; else ++stop;
	}
	current_user->last_msg_id=stop;
	return TRUE;
}


int place_add_msg(char *text) {

	TQ_VAR(places) *place=current_user->place;

	escape(text,MAXLINE);

	msgs_struct *msg;
	if(MAX_MSG_SIZE < place->mid) place->mid=0; 
	msg=&place->msgs[place->mid++];
	strncpy(msg->name,current_user->name,NAME_SIZE);
	strncpy(msg->string,text,MAXLINE);
	msg->id = current_user->sid;
	msg->time = now;
	msg->pvt = 0;
	print("add msg to %s from:%s, msg:%s",place->name,msg->name,msg->string);
	return TRUE;
}

int place_add_pvt(char *text) {

	TQ_VAR(places) *place=current_user->place; 
	TQ_VAR(users) * user;
	char *nick;
	char *ptr=text;
	int start=-1;
	if(*ptr == '<')
	while(*ptr!=0) {
	    if((*ptr) == '<') start=(int)(ptr-text);
	    else if((*ptr) == '>' && start>=0) break;
	    ptr++;
	}
	
	if(*ptr != '>') return FALSE;
	*ptr=0;
	nick=text+start+1;
	text=ptr+1;
	if(*text == 0x20) text++;

	if((user=check_user_by_name(nick)) == 0) return FALSE;

	escape(text,MAXLINE);
	
	msgs_struct *msg;
	if(MAX_MSG_SIZE < place->mid) place->mid=0; 
	msg=&place->msgs[place->mid++];
	strncpy(msg->name,current_user->name,NAME_SIZE);
	strncpy(msg->string,text,MAXLINE);
	msg->id = current_user->sid;
	msg->pvt = user->sid;
	msg->time = now;
	print("add pvt to %s from:%s, msg:%s",place->name,msg->name,msg->string);

	return TRUE;
}

void init_users() {
	TQ_INIT(users);
	TQ_INIT(places);
	place_create("цкюбмши вюр");
	place_create("брнпни вюр");
	place_create("рперхи вюр");
	place_create("MAIN");
	place_create("FALEN");
	twalk((void *)places_root,print_node);
	return;
}


