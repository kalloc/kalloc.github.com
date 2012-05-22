#include <getopt.h>
#include <ctype.h>
#include "var.h"


struct nlist *hashtab[1024];

static struct option long_options[] =
{
	{"verbose", no_argument,       &verbose_flag, 1},
	{"ip",     required_argument,       0, 'i'},
	{"port",     required_argument,       0, 'p'},
	{"file",    required_argument, 0, 'f'},
	{"key",    required_argument, 0, 'k'},
	{"db_host",    required_argument, 0, '1'},
	{"db_user",    required_argument, 0, '2'},
	{"db_pass",    required_argument, 0, '3'},
	{"db_name",    required_argument, 0, '4'},
	{0, 0, 0, 0}
};




int is_config(char *name) {
	if(lookup(hashtab,name) == NULL) 
		return FALSE; 
	else 
		return TRUE;
}

char * get_config(char *name) {
	struct nlist *np;
	if((np=lookup(hashtab,name)) == NULL) 
		return NULL; 
	else 
		return np->value;
}

void set_config(char *name,char *value) {
	install(hashtab,name,value); 
}


void show_help(char *error) {
	char buf[BUFFSIZE];
	int i;

	snprintf(buf,BUFFSIZE-1,"...::] chatd build ver %s ("__DATE__" "__TIME__") [::...",CHATD_VERSION);
	i=strlen(buf);
	printf("%s\n",buf);
	memset(&buf,'\0',BUFFSIZE-1);
	for(;i>=0;i--) {
		buf[i]=(i%2 ? '-' : '=');
	}
	printf(
"%s\n"
"--verbose	verbose output to stdout\n"
"--file		config file (default "dFILE")\n"
"--ip		bind ip (default "dIP")\n"
"--port		bind port (default "dPORT")\n"
"--help		this help\n",buf);

	if(strlen(error)>0)
		printf("%s\nError: %s\n",buf,error);

	printf("%s\n.EOH\n",buf);

	exit (0);

}

void check_arg() {


  struct hostent *hp;


  print("init key,port and ip");

/*
	memset(&MASTER_KEY,'\0',sizeof(int));
	memcpy(&MASTER_KEY,get_config("key"),sizeof(int));
	print("%x",MASTER_KEY);
*/

  if (strlen(get_config("key")) > sizeof(int) || strlen(get_config("key")) <= 0)
  {
    show_help("key is greater than 0 and less than sizeof(int)+1 = 5");
  }
  memset(&MASTER_KEY,'\0',sizeof(int));
  MASTER_KEY=(*(int *)get_config("key"));


  if (atoi(get_config("port"))<=0 || atoi(get_config("port"))>=65535 )
  {
    show_help("please enter correct port");
  }
  LISTEN_PORT=atoi(get_config("port"));

  hp = gethostbyname (get_config("ip"));
  if (!hp)
  {
    show_help("please enter correct ip/host");
  }

  LISTEN_IP=strdup(get_config("ip"));

  DB_PASS=strdup(get_config("db_pass"));
  DB_USER=strdup(get_config("db_user"));
  DB_NAME=strdup(get_config("db_name"));
  DB_HOST=strdup(get_config("db_host"));



}

void read_arg(int argc , char **argv) {

	struct hostent *hp;
        struct in_addr u;
	int c;
	while (1) {

		int option_index = 0;
     
		c = getopt_long (argc, argv, "?f:",long_options, &option_index);
     
		if (c == -1)
			break;
     
		switch (c)
		{
			case 0:
				if (long_options[option_index].flag != 0)
					break;
				printf ("option %s", long_options[option_index].name);
				if (optarg)
					printf (" with arg %s", optarg);
				printf ("\n");
				break;
			case 'i':
				  hp = gethostbyname (optarg);
				  if (!hp)
				  {
				    show_help("please enter correct ip/host");
				  }
			          u.s_addr = **(unsigned long **)hp->h_addr_list;
				  set_config("ip",inet_ntoa (u));
				  if (verbose_flag)
 	 				  printf ("set bind ip to: %s\n", inet_ntoa (u));
				break;
			case 'p':
				  
				  set_config("port",optarg);
				  if (verbose_flag)
 	 				  printf ("set bind port to: %s\n", optarg);
				break;
			case 'k':

				  set_config("key",optarg);
				  if (verbose_flag)
 	 				  printf ("set key to: %s\n", optarg);
				break;
			case 'f':
	 		        if (verbose_flag)
 	 				  printf ("set config file to: %s\n", optarg);
			  	set_config("file",optarg);

				break;
			case '?':
				show_help("");
				break;
			default:
				abort ();
		}

	}
     

	if (optind < argc) {
		printf ("non-option ARGV-elements: ");
		while (optind < argc)
			printf ("%s ", argv[optind++]);
		putchar ('\n');
	}
        

}

int read_config()
{

       if (verbose_flag)
	       printf ("try to parse config file: %s.\n", get_config("file"));

	FILE *fl=NULL;

	struct nlist *np;
	char * buf;
	char tmp_param[BUFFSIZE];
	char tmp_value[BUFFSIZE];
	fl = fopen((char *)get_config("file"),"r");
	if (!fl) { return -1; }
		
	buf=malloc(BUFFSIZE);
	while (fgets(buf,BUFFSIZE-1,fl)) 
	{


		int x,cv,cp,flag=0,i=strlen(buf);
		for(cv=0,cp=0,x=0;x<=i;x++) 
		{	
			switch(buf[x]) {
			
			case 0x0A: case 0x0D: case 0x3B:
				x=i;
				break;

			case '=': case ' ': case '\t':
				if(cp>0)
					flag=1;	
				break;
			default:

				if(flag > 0) {
	 			    if((buf[x] > 64 &&  buf[x]  < 123)  || (buf[x] > 45 &&  buf[x]  < 58))  {
					tmp_value[cv++]=buf[x];
					flag=2;
				    }
				}
				else {
	 			    if(buf[x] > 64 &&  buf[x]  < 123)  {
					tmp_param[cp++]=buf[x];
				    }
				}
				break;
		        }
		}
	    if(cp > 0)
	    {
		tmp_param[cp++]=0;
		if((np=lookup(hashtab,tmp_param)) != NULL)
		{
			if(cv == 0)
				err_sys("param %s, value not set\n",tmp_param);
			  else  {
				tmp_value[cv++]=0;
				install(hashtab,tmp_param,tmp_value);
		 		    if (verbose_flag)
					log_sys("options %s value reset to %s\n",tmp_param,tmp_value);
			}
		}
		else  
			err_quit("options %s, not found\n",tmp_param);
		  
	     }

	}
        free(buf);
	fclose(fl);
	return 0;
}



void init_config(int argc , char **argv) {


        int i,ii;
	ii=sizeof(long_options)/sizeof(struct option)-1;

	memset(hashtab,'\0',sizeof(hashtab));

	for(i=0;i<ii;i++) {
		if(long_options[i].flag == 0) install(hashtab,(char *)long_options[i].name, "");
		else install(hashtab,(char *)long_options[i].name,(char *)long_options[i].flag);
	}

	set_config("file",dFILE);
	set_config("port",dPORT);
	set_config("ip",dIP);
	set_config("key",dKEY);
	set_config("db_user",dDB_USER);
	set_config("db_pass",dDB_PASS);
	set_config("db_name",dDB_NAME);
	set_config("db_host",dDB_HOST);


	print("read config");
	if(read_config() == -1) {
	    if (verbose_flag) {
		printf("unable to open config file: %s\n",get_config("file"));
	    }
	}

	print("read arg");
	read_arg(argc,argv);

	check_arg();

}



void print_config(int depth,struct nlist *np)
{

	int i,ii;


	if(depth==0) {
		 depth=0;
	}


	char buf[BUFFSIZE];
	memset(&buf,'\0',BUFFSIZE-1);
	for(i=depth;i>=0;i--) {

		buf[i]=' ';
		
	}


if(np==NULL) {
	ii=sizeof(hashtab)/sizeof(struct nlist *);

	for(i=0;i<ii;i++)
        {
		np=hashtab[i];

		if(np != NULL) {
			if(np->name != NULL ) {
				if(*np->value == 0 || *np->value == 1)
					log_sys("options %s equal %s\n",np->name,np->value ? "TRUE" : "FALSE");
				else 
					log_sys("options %s equal %s\n",np->name,np->value);
			}

			if(np->next != NULL)
				print_config(depth+1,np->next);

		}


	}

}


}




//EOF
