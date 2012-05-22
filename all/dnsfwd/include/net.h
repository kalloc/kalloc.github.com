
void rawnet_clear();
void rawnet_add_char(char *ptr);
void rawnet_add_int(int ptr);
void rawnet_get(char *ptr,int len);
int rawnet_get_int();
char *rawnet_get_char();
int rawnet_len();
void rawnet_get_reset();
void *rawnet_box();
int net_send(int id, char *buf,int len);
char *net_box();
int net_init_server(void (*handle)(int, short, void *));
int net_id();
int net_recv();
int net_len();
char rawnet_get_byte();
short int rawnet_get_short();
void rawnet_get_ptr(char *,int);	
char * get_ns_name(int);
char * get_addr();
int  get_port();
void rawnet_add_char(char *);
void rawnet_add_byte(char);
void rawnet_add_int(int);
void rawnet_add_ptr(char *,int);
void rawnet_add_short(short int);
//EOF

