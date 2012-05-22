CC=gcc
LIBS= -lmysqlclient -lfcgi -lMagick++
CFLAGS=-O0 -g


all: build

count.o:
	@${CC} ${CFLAGS} ${LIBS} count.cpp -c
	@echo compile count.cpp
click.o:
	@${CC} ${CFLAGS} ${LIBS} click.cpp -c
	@echo compile click.cpp
functions.o:
	@${CC} ${CFLAGS} ${LIBS} functions.cpp -c
	@echo compile functions.cpp

build: count.o click.o functions.o
	@${CC} ${CFLAGS} ${LIBS} count.o functions.o -o count.fcgi
	@echo build counter
	@${CC} ${CFLAGS} ${LIBS} click.o functions.o -o click.fcgi
	@echo build click

clean:
	@echo clean *.o and *.fcgi files
	@rm -f *.o rm -f *.fcgi
