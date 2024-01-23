

LIBPATH = `pwd`/../lib/
CFLAGS +=-I`pwd`/../include/
LDFLAGS +=-L${LIBPATH} -lsqlite3
NAME = server

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
	./${NAME} -p 6020 -d 1
