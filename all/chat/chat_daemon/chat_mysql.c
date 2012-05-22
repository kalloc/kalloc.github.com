#include <stdio.h>
#include <stdlib.h>
#define NETH
#define DEFINE
#include "var.h"
#include <iconv.h>
#include <qtl_tailq.h>
#include <qtl_mysql.h>


MYSQL_VARS(user);
MYSQL_FIELD *field;


void db_init() {
		MYSQL_CONNECT(user,DB_USER,DB_PASS,DB_NAME,DB_HOST);
}

void db_reinit() {
		MYSQL_RECONNECT(user,DB_USER,DB_PASS,DB_NAME,DB_HOST);
}


int db_check_account (char *user) {

    MYSQL_QUERY_EXEC(user,"select * from account where aLogin = '%s'",user);

    MYSQL_OK(user) {
	  if(MYSQL_NUM(user) > 0) return TRUE;
	  else return FALSE;
    }


    return FALSE;
}


int db_check_access (char *user,char *pass) {

    MYSQL_QUERY_EXEC(user,"select aID,aStatus from account where aLogin = '%s' and aPassword = '%s'",user,pass);
    MYSQL_OK(user) {
	  if(MYSQL_NUM(user) > 0) {
	    MYSQL_ITEM(user) = mysql_fetch_row(MYSQL_RES(user));
    	    db_account.aStatus=atoi(MYSQL_ITEM(user)[1]);
    	    db_account.aID=atoi(MYSQL_ITEM(user)[0]);
	    return db_account.aID;
	  }
    }


    return 0;
}


int db_reg_user (char *race,char * day,char * month,char * year,char * gender,char *city,char *country,char *name,char *email,char *pass,char *user) {

    MYSQL_QUERY_NR(user,"insert into  `account` set "
			"aLogin = '%s', "
			"aPassword = '%s',"
			"aEmail = '%s', "
			"aName = '%s', "
			"aCountry = '%s', "
			"aCity = '%s', "
			"aGender = '%s',"
			"aHB = '%s-%s-%s',"
			"aRace = '%s'",user,pass,email,name,country,city,gender,year,month,day,race);
    MYSQL_OK(user) {
	  if(MYSQL_NUM(user) > 0) {
	    MYSQL_ITEM(user) = mysql_fetch_row(MYSQL_RES(user));
	    return atoi(MYSQL_ITEM(user)[0]);
	  }
    }
    return 0;
}



//char action



int db_char_check_login (char *user,int uid) {

    if(uid == FALSE)	{
        MYSQL_QUERY_EXEC(user,"select cID,cProf from characters where cLogin = '%s'",user);
    }    else	{
	MYSQL_QUERY_EXEC(user,"select cID,cProf from characters where cLogin = '%s' and aID = '%d'",user,uid);
    }

    MYSQL_OK(user) {
	  if(MYSQL_NUM(user) > 0) {
	    MYSQL_ITEM(user) = mysql_fetch_row(MYSQL_RES(user));
    	    db_char.cID=atoi(MYSQL_ITEM(user)[0]);
    	    db_char.cProf=atoi(MYSQL_ITEM(user)[1]);
	    return db_char.cID;
	  }
	  else return FALSE;
    }
    return FALSE;
}

int db_char_check_id (unsigned int cid,int uid) {

    if(uid == FALSE)  {
        MYSQL_QUERY_EXEC(user,"select cID,cProf,cLogin from characters where cID  = '%d'",cid); 
    }	else	{
	MYSQL_QUERY_EXEC(user,"select cID,cProf,cLogin from characters where cID = '%d' and aID = '%d'",cid,uid);
    }

    MYSQL_OK(user) {
	  if(MYSQL_NUM(user) > 0) {
	    MYSQL_ITEM(user) = mysql_fetch_row(MYSQL_RES(user));
    	    db_char.cID=atoi(MYSQL_ITEM(user)[0]);
    	    db_char.cProf=atoi(MYSQL_ITEM(user)[1]);
    	    snprintf((char *)db_char.cLogin,NAME_SIZE,"%s",MYSQL_ITEM(user)[2]);
	    return db_char.cID;
	  }
	  else return FALSE;
    }
    return FALSE;
}


void db_char_del (unsigned int cid) {
    MYSQL_QUERY_EXEC(user,"delete from characters where cID  = '%d'",cid); 
}




void db_char_list (unsigned int uid) {

											
    MYSQL_QUERY_EXEC(user,"select cID,cLogin,cProf from characters  where aID = '%d'",uid);

    MYSQL_OK(user) {

	rawnet_add_int(MYSQL_NUM(user));
	net_send_raw(reply(CHAR_LIST),rawnet_box(),rawnet_len());
	
	if(MYSQL_NUM(user) > 0) {
	    while(MYSQL_ITEM(user) = mysql_fetch_row(MYSQL_RES(user))) {
		    rawnet_clear();
		    rawnet_add_int(MYSQL_ITEM(user)[2]);
		    rawnet_add_char(MYSQL_ITEM(user)[1]);
		    rawnet_add_int(atoi(MYSQL_ITEM(user)[0]));
		    net_send_raw(reply(CHAR_LIST),rawnet_box(),rawnet_len());
	    }
	}
    } 
}


int db_reg_char (char *login,int uid) {

    MYSQL_QUERY_NR(user,"insert into  `characters` set "
			"cLogin = '%s',aID = '%d'" ,login,uid);
    MYSQL_OK(user) {
	  if(MYSQL_NUM(user) > 0) {
	    MYSQL_ITEM(user) = mysql_fetch_row(MYSQL_RES(user));
	    return atoi(MYSQL_ITEM(user)[0]);
	  }
    }
    return 0;
}






