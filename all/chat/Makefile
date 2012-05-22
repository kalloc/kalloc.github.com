CC	= gcc -I/usr/local/include -I ./include/   -DBUILD -DDEBUG
CPFLAGS	= -O3 
LFLAGS	= -L/usr/local/lib  
LPFLAGS	= -L/usr/local/lib 
#CFLAGS	= -Wall -g -pg  -O0 
CFLAGS	= -Wall  -O2
COMMON	= chat_fastcgi/chat_classes.c chat_fastcgi/chat_common.c

none:
	@echo
	@echo "make <target>"
	@echo
	@echo  "Targets:"
	@echo  "     all  - Compile all"
	@echo  "     chatd - Compile chatd"
	@echo

clean:
	rm -f *.o chatd chat.fcgi

read_conf.o:
	${CC} -I ./network/ ${CFLAGS} ${LFLAGS} -c common/read_conf.c 

debug.o:
	${CC} -I ./network/ ${CFLAGS} ${LFLAGS} -c common/debug.c

hash.o:
	${CC} -I ./network/ ${CFLAGS} ${LFLAGS} -c common/hash.c

network.o:
	${CC} -I ./network/ ${CFLAGS} ${LFLAGS} -c network/network.c

mail.o:
	${CC} -I ./network/ ${CFLAGS} ${LFLAGS} -c common/mail.c
check.o:
	${CC} -I ./network/ ${CFLAGS} ${LFLAGS} -c common/check.c 
verify_link.o:
	${CC} -I ./network/ ${CFLAGS} ${LFLAGS} -c common/verify_link.c 
aes.o:
	${CC} -I ./network/ ${CFLAGS} ${LFLAGS} -c common/aes.c  


chatd: network.o  hash.o debug.o read_conf.o check.o
	${CC} -I ./network/ ${CFLAGS} ${LFLAGS} chat_daemon/chatd_main.c chat_daemon/chat_users.c  chat_daemon/chat_mysql.c check.o hash.o debug.o read_conf.o network.o -o chatd -lm   -lmysqlclient  -lpcre

chat_fcgi:  hash.o debug.o network.o  check.o
	${CC} -I ./network/ ${CFLAGS} ${LFLAGS} chat_fastcgi/chat_fcgi_main.c chat_fastcgi/chat_check.c ${COMMON}  check.o  network.o hash.o debug.o   -o chat.fcgi  -lm  -lqtp -lfcgi -lpcre

cap_fcgi: check.o 
	${CC} -I ./network/ ${CFLAGS} ${LPFLAGS} chat_fastcgi/chat_fcgi_captcha.c check.o -o captcha.fcgi   -lm  -lqtp -lfcgi  -lgd

reg_fcgi:  debug.o network.o mail.o check.o verify_link.o aes.o hash.o
	${CC} -I ./network/ ${CFLAGS} ${LFLAGS} chat_fastcgi/chat_fcgi_reg.c  chat_fastcgi/chat_check.c ${COMMON}  check.o  mail.o network.o hash.o debug.o  verify_link.o aes.o -o reg.fcgi  -lm  -lqtp -lfcgi  -lpcre -lcrypt




all: chatd chat_fcgi reg_fcgi
