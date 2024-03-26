APPNAME=project
LIBPATH=`pwd`/../lib/

CFLAGS+=-I${LIBPATH}
LDFLAGS+=-L${LIBPATH}


CC=gcc

all:
	${CC} ${CFLAGS} -I ../../sqlite/install/include/ project.c -o ${APPNAME} ${LDFLAGS} -lrpi_client -L ../../sqlite/install/lib/ -lsqlite3

clean:
	rm -rf ${APPNAME}
	rm -rf test2.db

run:
	export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:/home/iot24/dengleen/ch7_project/aclient/lib && ./${APPNAME} -i 192.168.2.40 -p 12345 -t 5 
