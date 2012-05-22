// MYSQL_HELPER_MODULE VERSION 2 (extended)

#include <mysql/mysql.h>
#include <unistd.h>
#include <string.h>

#define MYSQL_VARS(ID) 								\
	char ID ## _x_mysql_query[4096]; 					\
	MYSQL * ID ## _mysql_db; 						\
	MYSQL_RES * ID ## _mysql_res; 						\
	int ID ## _mysql_res_entry=0; 						\
	MYSQL_ROW ID ## _mysql_row;

#define MYSQL_EXTERN_VARS(ID) 								\
	extern char ID ## _x_mysql_query[4096]; 					\
	extern MYSQL * ID ## _mysql_db; 						\
	extern MYSQL_RES * ID ## _mysql_res; 						\
	extern int ID ## _mysql_res_entry; 						\
	extern MYSQL_ROW ID ## _mysql_row;

#define MYSQL_CONNECT(ID,user,pass,db,host) 					\
	ID ## _mysql_db = mysql_init(NULL); 					\
	if (! ID ## _mysql_db) { 						\
                puts("Init faild, out of memory?\n");exit(-1);			\
	}									\
	ID ## _mysql_db = mysql_real_connect( ID ## _mysql_db,host,user, 	\
		pass,db,0,NULL,0); 						\
	if (! ID ## _mysql_db) { 						\
                puts("Connect failed\n");exit(-1);				\
	}

#define MYSQL_RECONNECT(ID,user,pass,db,host) 					\
	ID ## _mysql_db = mysql_real_connect( ID ## _mysql_db,host,user, 	\
		pass,db,0,NULL,0); 						\
	if (! ID ## _mysql_db) { 						\
                puts("Connect failed\n");exit(-1);				\
	}

#define MYSQL_QUERY(ID,...) \
	snprintf(ID ## _x_mysql_query,sizeof(ID ## _x_mysql_query),__VA_ARGS__);\
	print("sql:%s",ID ## _x_mysql_query);					\
	if(mysql_query(ID ## _mysql_db,ID ## _x_mysql_query)) {       		\
		db_reinit();printf("Query failed: %s\n",mysql_error(ID ## _mysql_db));	\
	}									\
	if (ID ## _mysql_res_entry) mysql_free_result(ID ## _mysql_res); 	\
	else ID ## _mysql_res_entry = 1;					\
	ID ## _mysql_res = mysql_store_result(ID ## _mysql_db); 		\
	if(ID ## _mysql_res != NULL)						\
		while ( (ID ## _mysql_row = mysql_fetch_row(ID ## _mysql_res)))

#define MYSQL_QUERY_NR(ID,...) \
	snprintf(ID ## _x_mysql_query,sizeof(ID ## _x_mysql_query),__VA_ARGS__);	\
	print("sql nr:%s",ID ## _x_mysql_query);					\
	if(mysql_query(ID ## _mysql_db,ID ## _x_mysql_query)) {			\
		db_reinit();printf("Query failed: %s\n",mysql_error(ID ## _mysql_db));	\
	} 

#define MYSQL_QUERY_EXEC(ID,...) \
	snprintf(ID ## _x_mysql_query,sizeof(ID ## _x_mysql_query),__VA_ARGS__);\
	print("sql exec:%s",ID ## _x_mysql_query);				\
	if(mysql_query(ID ## _mysql_db,ID ## _x_mysql_query)) {       		\
		db_reinit();							\
		printf("Query failed: %s\n",					\
		mysql_error(ID ## _mysql_db));					\
	}									\
	if (ID ## _mysql_res_entry) mysql_free_result(ID ## _mysql_res); 	\
	else ID ## _mysql_res_entry = 1;					\
	ID ## _mysql_res = mysql_store_result(ID ## _mysql_db); 		\
	if(ID ## _mysql_res != NULL)						


#define MYSQL_NAMES(ID,CP) \
	snprintf(ID ## _x_mysql_query,sizeof(ID ## _x_mysql_query),"set names '" #CP "'"); \
	print("sql names:%s",ID ## _x_mysql_query);					\
	mysql_query(ID ## _mysql_db,ID ## _x_mysql_query); 

#define MYSQL_ITEM(ID) ID ## _mysql_row

#define MYSQL_RES(ID) ID ## _mysql_res

#define MYSQL_QUERY_STR(ID) ID ## _x_mysql_query

#define MYSQL_NUM(ID) mysql_num_rows(ID ## _mysql_res)

#define MYSQL_OK(ID) if(ID ## _mysql_res != NULL)
