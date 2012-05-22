CC	= gcc -I/usr/local/include -I ./include/   -DBUILD 
CPFLAGS	= -O3 
LFLAGS	= -L/usr/local/lib   -levent
#CFLAGS	= -Wall -g -pg  -O0 
CFLAGS	= -Wall  -O3

none:
	@echo
	@echo "make <target>"
	@echo
	@echo  "Targets:"
	@echo  "     all  - Compile all"
	@echo

clean:
	rm -f *.o dnsfwd

network.o:
	${CC} -I ./network/ ${CFLAGS} ${LFLAGS} -c fwd/network.c


dns_fwd: network.o  
	${CC} -I ./network/ ${CFLAGS} ${LFLAGS} fwd/main.c network.o -o dnsfwd 




all: clean dns_fwd
