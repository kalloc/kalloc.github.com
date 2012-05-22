#include <stdio.h>
#include <stdlib.h>
#define NET
#define DEFINE
#include "var.h"



char buffer[BLOCK];
char soabuf[BLOCK];
char Ns2[BLOCK],Ns1[BLOCK],Mx[BLOCK];
char cNs1[BLOCK];
char cNs2[BLOCK];
char cHost[BLOCK];
char ad[]=AD;
char srv[]=SRV;
int ttl_a,ttl_soa;
int counter=0;
int soalen=0;
int Serial = 2007071414;
int ttl;
int lcNs1;
int lcNs2;
int lcHost;
int lNs1;
int lNs2;
int lMx;

unsigned int len;



time_t time_start,now_start,now_end,now;
FILE * fp = 0;

//struct in_addr ip_fwd,ip_ns1,ip_ns2;


struct hDns_st {
    short flag;
    short countQuery;
    short countReply;
    short authReply;
    short addiReply;
    unsigned char size;
    char name[256];
    short type;
    short class;
} hDns;






int uptime() {
    return (int)(time(NULL)-time_start);
}


void stat_mrtg() {
	int l;
	now_end = time(NULL);
	l=(int)(now_end-now_start);
	if(l==0) l=1;
	snprintf(buffer,BLOCK-1,"%d %d",(int)now_end,(int)(counter/l));
	net_send(0x2020,buffer,strlen(buffer));
	now_start=now_end;
	counter=0;
	return ;
}


int write_name(char *in,char *out) {

    int start,end;
    start=0;
    end=0;
    for(;in[end]!=0;) {
	if(in[end]=='.') {
	    out[start]=end-start;
	    start=end+1;
	} else {
	    out[end+1]=in[end];
	}
	end++;
    }
    out[start]=end-start;
    out[end+1]=0;
    return end+2;
}


void restart_handler (int signum)
{
    if(fp != 0) fclose(fp);
    fp=0;
}


void  make_soa() {

    int len;

    len=write_name(Ns1,soabuf+soalen);
    soalen+=len;

    len=write_name(Mx,soabuf+soalen);
    soalen+=len;

    *(int*)(soabuf+soalen)=htonl(Serial);
    soalen+=4;

    *(int*)(soabuf+soalen)=htonl(60*60*24);
    soalen+=4;

    *(int*)(soabuf+soalen)=htonl(60*60*2);
    soalen+=4;

    *(int*)(soabuf+soalen)=htonl(60*60*24*7);
    soalen+=4;

    *(int*)(soabuf+soalen)=htonl(60*60*24);
    soalen+=4;


}

void loger(const char *fmt, ...) {
    static char buf[4096];
    static int len;
    va_list ap;
    va_start(ap,fmt);
    len=vsnprintf(buf,4096,fmt,ap);
    buf[len]='\n';
    buf[len+1]=0;
    if(fp==0) {
	fp=fopen("/var/log/dnsfwd.log","w");
    }
    fwrite(buf,1,len,fp);
    fflush(fp);
    va_end(ap);
}


void net_loop(int i,short sh,void *vd) {

	unsigned char chunk_len;


		if ((id=net_recv(i)) == 0 || id < 0 || (len=net_len()) <= 0)  {
			return ;
		}

		counter++;


		rawnet_clear();


		rawnet_get(net_box(),len);
		hDns.flag=ntohs(rawnet_get_short());
//		printf("0x%x\n",id);
		if(id == 0x4754524d) {
		    stat_mrtg();
		    return;		
		}
		
		hDns.countQuery=ntohs(rawnet_get_short());
		hDns.countReply=ntohs(rawnet_get_short());
		hDns.authReply=ntohs(rawnet_get_short());
		hDns.addiReply=ntohs(rawnet_get_short());
    		hDns.size=0;

		chunk_len=rawnet_get_byte();
		for(;hDns.size < len-10;) {
			if(chunk_len == 0) break;
			else if(chunk_len > 63)  {
    			    print("chunk_len > 63, type:0x%x, ID:0x%x",hDns.type,htons(id));		
			    return;
			} else if(hDns.size+chunk_len>256) {
				print("hDns.size+chunk_len>256 > 256, type:0x%x, ID:0x%x",hDns.type,htons(id));		
				return;
			}
        	        rawnet_get_ptr((hDns.name+hDns.size),chunk_len);
        		hDns.size+=chunk_len;
			hDns.name[hDns.size]='.';
        		hDns.size+=1;
			chunk_len=rawnet_get_byte();
		}
		hDns.name[hDns.size-1]=0;
		bzero(&cHost,BLOCK);
	        lcHost=write_name(hDns.name,cHost);

//		hDns.size-=1;
		

		
		hDns.type=ntohs(rawnet_get_short());
		hDns.class=rawnet_get_short();
		loger("%s [%s:%d], %s [%d], C:0x%d, T:0x%d, ID:0x%04x(%d), flag:%d, cQR:[%d:%d:%d:%d]\r\n",(char *)get_ns_name(i),(char *)get_addr(),get_port(),hDns.name,hDns.size,hDns.class,hDns.type,ntohs(id),ntohs(id),hDns.flag,hDns.countQuery,hDns.countReply,hDns.authReply,hDns.addiReply);
		rawnet_clear();

		switch(hDns.type) {


			case 1:
//				print("Type: A request");
				rawnet_add_short(0x0085); 
				rawnet_add_short(0x0100); 
				rawnet_add_short(0x0100); 
				rawnet_add_short(0); 
				rawnet_add_short(0);
				rawnet_add_ptr(cHost,lcHost);
				rawnet_add_short(0x0100); // Type A
				rawnet_add_short(0x0100); // Class IN
				rawnet_add_short(0x0cc0); // c00c ?? wtf ?

				if(strstr(hDns.name,"conference.")) {
				    rawnet_add_short(0x0500);
				    rawnet_add_short(hDns.class); // Class IN
				    rawnet_add_int(ttl_a); // 10 Min TTL
    				    rawnet_add_short(htons(sizeof(srv)-1)); // ans len
    				    rawnet_add_ptr(srv,sizeof(srv)-1);
				    net_send(id,rawnet_box(),rawnet_len());
				    
				} else {
				    rawnet_add_short(0x0100); // Type A
				    rawnet_add_short(hDns.class); // Class IN
				    rawnet_add_int(ttl_a); // 10 Min TTL
				    rawnet_add_short(0x0400); // Class IN
				    rawnet_add_int(ip_fwd.s_addr);  // ip
				}
				net_send(id,rawnet_box(),rawnet_len());
			
				break;	
			case 33:
//				print("Type: SRV request");
				rawnet_add_short(0x0085); 
				rawnet_add_short(0x0100); 
				rawnet_add_short(0x0100); 
				rawnet_add_short(0); 
				rawnet_add_short(0);
				//query
				rawnet_add_ptr(cHost,lcHost);
				rawnet_add_short(0x2100); // Type SRV
				rawnet_add_short(0x0100); // Class IN
				rawnet_add_short(0x0cc0); // c00c ?? wtf ?
				rawnet_add_short(0x2100); // Type SRV
				rawnet_add_short(hDns.class); // Class IN
				rawnet_add_int(ttl_a); //  TTL
				rawnet_add_short(htons(sizeof(srv)+5)); // ans len
				rawnet_add_short(0x0500); // Priority 1
				rawnet_add_short(0x0000); // Weight 0
				if(strstr(hDns.name,"_jabber._tcp") || strstr(hDns.name,"_xmpp-server._tcp")) {
				    rawnet_add_short(0x9514); // Port 5269
				} else if(strstr(hDns.name,"_xmpp-client._tcp")) {
				    rawnet_add_short(0x6614);  //Port 5222
				} else {
				    rawnet_add_short(0x9a02);
				}
				
    				rawnet_add_ptr(srv,sizeof(srv)-1);
				net_send(id,rawnet_box(),rawnet_len());
			
				break;	


			case 6:
//				print("Type: SOA request");
				rawnet_add_short(0x0085); 
				rawnet_add_short(0x0100); 
				rawnet_add_short(0x0100); 
				rawnet_add_short(0); 
				rawnet_add_short(0);
				//query
				rawnet_add_ptr(cHost,lcHost);
				rawnet_add_short(0x0600); // Type SOA
				rawnet_add_short(0x0100); // Class IN
				//answer
				rawnet_add_short(0x0cc0); // c00c ?? wtf ?
				rawnet_add_short(0x0600); // Type SOA
				rawnet_add_short(hDns.class); // Class IN
				rawnet_add_int(ttl_soa); //  TTL
				rawnet_add_short(htons(soalen)); // SOA len
				rawnet_add_ptr(soabuf,soalen);

				net_send(id,rawnet_box(),rawnet_len());
			
				break;	
			case 16:
//				print("Type: TXT request");
				rawnet_add_short(0x0085); 
				rawnet_add_short(0x0100); 
				rawnet_add_short(0x0100); 
				rawnet_add_short(0); 
				rawnet_add_short(0);
				//query
				rawnet_add_ptr(cHost,lcHost);
				rawnet_add_short(0x1000); // Type SOA
				rawnet_add_short(0x0100); // Class IN
				//answer
				rawnet_add_short(0x0cc0); // c00c ?? wtf ?
				rawnet_add_short(0x1000); // Type SOA
				rawnet_add_short(hDns.class); // Class IN
				rawnet_add_int(ttl_a); //  TTL
				rawnet_add_short(htons(sizeof(ad))); // ans len
				rawnet_add_byte((char)(sizeof(ad)-1)); // TXT len
    				rawnet_add_ptr(ad,sizeof(ad)-1);
				net_send(id,rawnet_box(),rawnet_len());
			
				break;	
			case 2:
//				print("Type: NS request");
				rawnet_add_short(0x0085); 
				rawnet_add_short(0x0100); 
				rawnet_add_short(0x0200); 
				rawnet_add_short(0); 
				rawnet_add_short(0x0200);
				//query
				rawnet_add_ptr(cHost,lcHost);
				rawnet_add_short(0x0200); // Type NS
				rawnet_add_short(0x0100); // Class IN

				//ns1 ns
				rawnet_add_short(0x0cc0); // c00c ?? wtf ?
				rawnet_add_short(0x0200); // Type NS
				rawnet_add_short(0x0100); // Class IN
				rawnet_add_int(ttl_soa); 
				rawnet_add_short(htons(lcNs1));  
				rawnet_add_ptr(cNs1,lcNs1);

				//ns2 ns
				rawnet_add_short(0x0cc0); // c00c ?? wtf ?
				rawnet_add_short(0x0200); // Type NS
				rawnet_add_short(0x0100); // Class IN
				rawnet_add_int(ttl_soa); 
				rawnet_add_short(htons(lcNs2));  
				rawnet_add_ptr(cNs2,lcNs2);

				//ns1 a
				rawnet_add_ptr(cNs1,lcNs1);
				rawnet_add_short(0x0100); // Type A
				rawnet_add_short(0x0100); // Class IN
				rawnet_add_int(ttl_a); 
				rawnet_add_short(0x0400); // Class IN
				rawnet_add_int(ip_ns1.s_addr);  // ip

				//ns2 a
				rawnet_add_ptr(cNs2,lcNs2);
				rawnet_add_short(0x0100); // Type A
				rawnet_add_short(0x0100); // Class IN
				rawnet_add_int(ttl_a); 
				rawnet_add_short(0x0400); // Class IN
				rawnet_add_int(ip_ns2.s_addr);  // ip

				net_send(id,rawnet_box(),rawnet_len());
	
				break;	


        		default:

//				print("Type: Other request");
				rawnet_add_short(0x0085); 
				rawnet_add_short(0x0100); 
				rawnet_add_short(0); 
				rawnet_add_short(0x0100); 
				rawnet_add_short(0);
				rawnet_add_ptr(cHost,lcHost);
				rawnet_add_short(htons(hDns.type)); // 
				rawnet_add_short(0x0100); // Class IN
				rawnet_add_short(0x0cc0); // c00c ?? wtf ?
				rawnet_add_short(0x0600); // Type SOA
				rawnet_add_short(0x0100); // Class IN
				rawnet_add_int(ttl_soa); 
				rawnet_add_short(htons(soalen)); // SOA len
				rawnet_add_ptr(soabuf,soalen);
				net_send(id,rawnet_box(),rawnet_len());
	
				break;	



	}


}





     
int main (int argc , char **argv){
	time_start=time(NULL);

	if(argc < 5) { printf("%s hostname  ip1 ip2 fwdip\n",argv[0]);exit(1);}

	now_start = time(NULL);

	snprintf(Ns2,1024,"ns2.%s",argv[1]);
	snprintf(Ns1,1024,"ns1.%s",argv[1]);
	snprintf(Mx,1024,"mail.%s",argv[1]);

	inet_aton(argv[4],&ip_fwd);
	inet_aton(argv[2],&ip_ns1);
	inet_aton(argv[3],&ip_ns2);
	lNs1=strlen(Ns1);
	lNs2=strlen(Ns2);
	lMx=strlen(Mx);
        lcNs1=write_name(Ns1,cNs1);
        lcNs2=write_name(Ns2,cNs2);

	ttl_a=htonl(60*10);
	ttl_soa=htonl(60*60*24);

	make_soa();
	
	struct sigaction new_action, old_action;
	new_action.sa_handler = restart_handler;
	sigemptyset (&new_action.sa_mask);
	new_action.sa_flags = 0;
	sigaction (SIGHUP, NULL, &old_action);
	if (old_action.sa_handler != SIG_IGN)
    	    sigaction (SIGHUP, &new_action, NULL);

	if(fork() == 0) {
	    close(stdin);
	    close(stdout);
	    close(stderr);
	    net_init_server(net_loop);
	}
	exit (0);
}
