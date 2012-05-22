#define catarg(x,y)    	x ## y
#define request(ARG) 	catarg(ARG,1)
#define reply(ARG) 	catarg(ARG,2)
#define memscpy(static)  memcpy(buf+buflen,static,sizeof(static)-1);buflen+=sizeof(static)-1
#define memscmp(source,sample)   memcmp(source,sample,sizeof(sample))
#define mt_rand(s1,s2) (s1 + rand() % (s2+1-s1))
#define min(s1,s2) s1 < s2 ? s1 : s2


//CGI
#define cgiprintf(...)		printf(__VA_ARGS__)
#define cgiWriteEnd(str)	cgiTop();fwrite(str,sizeof(str)-1,1,stdout);cgiDown();
#define cgiWrite(str,len)	fwrite(str,len,1,stdout)
#define cgiWriteStr(str)	fwrite(str,sizeof(str)-1,1,stdout)

#define SERVER_ERROR    error_500();qsm_del_param("uid");uid=0;return
#define LOGOUT          cgiWriteStr("<login><status>logout</status></login>\r\n<reload><![CDATA[/]]></reload>\r\n");qsm_del_param("uid");uid=0;
#define CHAR_OK		cgiWriteStr("<char><status>ok</status></char>\r\n<reload><![CDATA[/chat.html]]></reload>\r\n");
#define CHAR_DENIED     cgiWriteStr("<char><status>denied</status></char>\r\n");
#define LOGIN_DENIED    cgiWriteStr("<login><status>denied</status></login>\r\n");

#define cookie(name,value,time) cgiHeaderCookieSetString(name, value,time, cgiScriptName, cgiServerName)


#define MRTG		0x0001001
#define CPS		0x0001002

#define ADD_MESSAGE	0x0001F00
#define ADD_PRIVATE	0x0001F08
#define GET_MESSAGE	0x0001F01
#define USER_LOGIN	0x0001F02
#define USER_LOGOUT	0x0001F03
#define USER_LIST	0x0001F04
#define USER_JOIN	0x0001F05
#define USER_PLACE_LIST	0x0001F06
#define USER_STATUS	0x0001F07
#define GUEST_LOGIN	0x0001F09
#define REG_CHECK	0x0002F00
#define REG_USER	0x0002F01
#define PRE_REG_USER	0x0002F02

#define CHAR_LIST	0x0001F10
#define CHAR_SELECT	0x0001F11
#define REG_CHAR_CHECK	0x0002F03
#define REG_CHAR	0x0002F04
#define CHAR_DEL	0x0002F05



#define ACCESS_ERROR    0xDEADC0FE
#define PLACE_ERROR 	0xB0B1BABE

#define EXIT 		0x0001FFF
#define UPTIME		0x0000FFF


