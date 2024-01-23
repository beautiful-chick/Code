

LIBPATH = `pwd`/../lib/
CFLAGS +=-I`pwd`/../include/
LDFLAGS +=-L${LIBPATH} -lsqlite3
NAME = client

all:clean create_o
	gcc `pwd`/*.o -o ${NAME} ${CFLAGS} ${LDFLAGS}

create_o:
	gcc -c `pwd`/*.c ${CFLAGS} ${LDFLAGS}

clean:
	rm -f ${NAME}

distclean:clean
	rm -f *.o
	rm -f *.db

run:
	./${NAME} -i 127.0.0.1 -p 6020 -t 2
