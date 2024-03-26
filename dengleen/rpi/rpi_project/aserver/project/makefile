APPNAME=server_epoll
LIBPATH=`pwd`/../lib/

CFLAGS+=-I${LIBPATH}
LDFLAGS+=-L${LIBPATH}


CC=gcc

all:
	${CC} ${CFLAGS} -I ../../sqlite/install/include/ server_epoll.c -o ${APPNAME} ${LDFLAGS} -lrpi_server -L ../../sqlite/install/lib/ -lsqlite3

clean:
	rm -rf ${APPNAME}
	rm -rf test2.db

run:
	export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:/home/iot24/dengleen/ch7_project/aserver/lib && ./${APPNAME} -p 12345  
