#define catarg(x,y)    	x ## y
#define request(ARG) 	catarg(ARG,1)
#define reply(ARG) 	catarg(ARG,2)

#define MRTG		0x0001001
#define CPS		0x0001002

#define ADD_MESSAGE	0x0001F00
#define GET_MESSAGE	0x0001F01
#define USER_LOGIN	0x0001F02
#define USER_LOGOUT	0x0001F03
#define USER_LIST	0x0001F04
#define USER_JOIN	0x0001F05
#define USER_LIST_PLACE	0x0001F06
#define USER_STATUS	0x0001F07

#define REG_CHECK	0x0002F00

#define ACCESS_ERROR    0xDEADC0FE
#define PLACE_ERROR 	0xB0B1BABE

#define EXIT 		0x0001FFF
#define UPTIME		0x0000FFF


