#include <stdio.h>
#include <stdlib.h>
#define NETH
#define DEFINE
#include "var.h"

int cps_flag;
int mrtg_flag;
int ID;
char MSG[BUFFSIZE];
time_t time_check;
int num;

struct nlist *hashtab[1024];


static struct option long_options[] =
{
	{"verbose", no_argument,       &verbose_flag, 1},
	{"ip",     required_argument,       0, 'i'},
	{"exec_cps",no_argument,&cps_flag,1},
	{"exec_mrtg",no_argument,&mrtg_flag,1},
	{"port",     required_argument,       0, 'p'},
	{"id",     required_argument,       0, 'I'},
	{"list_id",no_argument,0,'l'},
	{"send_msg",     required_argument,       0, 's'},
	{"time_check",     required_argument,       0, 'T'},
	{"key",    required_argument, 0, 'k'},
	{"num",    required_argument, 0, 'n'},
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

void show_list() {

}

void show_help(char *error) {
	char buf[BUFFSIZE];
	int i;

	snprintf(buf,BUFFSIZE-1,"...::] chat_client build ver %s ("__DATE__" "__TIME__") [::...",CHATD_VERSION);
	i=strlen(buf);
	printf("%s\n",buf);
	memset(&buf,'\0',BUFFSIZE-1);
	for(;i>=0;i--) {
		buf[i]=(i%2 ? '-' : '=');
	}
	printf(
"%s\n"
"--verbose	verbose output to stdout\n"
"--ip		connect ip\n"
"--key		access key \n"
"--id		process id \n"
"--list_id	list available id \n"
"--num		num connect (0 inf)\n"
"--exec_cps	exec_cps (after --time, default 10)\n"
"--exec_mrtg	exec_mrtg (after --time, default 10)\n"
"--time_check	time check (cps, mrtg)\n"
"--send_msg	msg if is needed \n"
"--port		bind port\n"
"--help		this help\n",buf);

	if(strlen(error)>0)
		printf("%s\nError: %s\n",buf,error);

	printf("%s\n.EOH\n",buf);

	exit (0);

}

void check_arg() {


  struct hostent *hp;


  print("init key,port and ip");

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

  if(MSG == NULL) {
	  bzero(&MSG,BUFFSIZE-1);
	  snprintf(MSG,BUFFSIZE-1,"TesTTesTTesTTesTTesTTesT");
  }

  if(ID == 0) 
    show_help("please enter id");

  if(num == 0) 
	num=60;



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
			case 'I':
				  ID=atoi(optarg);
				  if (verbose_flag)
 	 				  printf ("set id to: %x\n",ID);
				break;
			case 'n':
				  num=atoi(optarg);
				  if (verbose_flag)
 	 				  printf ("set num request to: %d\n",num);
				break;
			case 'T':
				  time_check=(time_t)atoi(optarg);
				  if (verbose_flag)
 	 				  printf ("set time_check to: %i\n",(int)time_check);
				break;
			case 's':
				  bzero(&MSG,BUFFSIZE-1);
				  snprintf(MSG,BUFFSIZE-1,"%s",optarg);

				  if (verbose_flag)
 	 				  printf ("set send_msg to: %s\n",MSG);
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
			case '?':
				show_help("");
				break;
			case 'l':
				show_list();
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



void init_config(int argc , char **argv) {


        int i,ii;
	ii=sizeof(long_options)/sizeof(struct option)-1;

	memset(hashtab,'\0',sizeof(hashtab));

	for(i=0;i<ii;i++) {
		if(long_options[i].flag == 0)
			install(hashtab,(char *)long_options[i].name, "");
		else
			install(hashtab,(char *)long_options[i].name,(char *)long_options[i].flag);
	}

	time_check=60;

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





void verb_recv() {

	int id,len;
	char *buffer;

	if ((id=net_recv()) == 0 || id < 0 || (len=net_len()) <= 0)  
			return;


	buffer=net_box();
	buffer[len-1]='\0';
	if(verbose_flag) 
		printf("id:%x, len:%d, buffer:%s\n",id,len,buffer);
	else
		printf("%s\n",buffer);

}


     
int main (int argc , char **argv){

	init_config(argc,argv);

	setnonblock(net_init_udp_client());
	int i;

	//print("num %d, ID: %d, MSG: %s",num,ID,MSG);
	time_t time_start=time(NULL);

	if(num == 0) num = -5;
	while(num == -5 || num--) {


		if(net_send(ID,MSG) == -1)
		{
		 print("error ned_send");
		 break;
		}
		verb_recv();

		i=1;
		if((cps_flag || mrtg_flag) && (time(NULL)-time_start>=time_check)) {

			time_start=time(NULL);

			if(cps_flag) {
				net_send(request(CPS),"KAKOYTOTEXT");
				verb_recv();
			}
			if(mrtg_flag) {
				net_send(request(MRTG),"KAKOYTOTEXT");
				verb_recv();
			}
		}
	}


	print("exit");


	exit (0);
}
