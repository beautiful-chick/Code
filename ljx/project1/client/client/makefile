APPNAME=client
IP=127.0.0.1
PORT=8899
TIME=5
INCPATH=/home/iot24/ljx/project1/client/src/
LIBPATH=/home/iot24/ljx/project1/client/lib/
CFLAGS+=-I ${INCPATH}
LDFAGS+=-L ${LIBPATH}
CC=gcc
all:
	  ${CC} main_client.c ${CFLAGS} ${LDFAGS} -l${APPNAME} -lsqlite3 -o ${APPNAME}
clean:
	rm -f ${APPNAME}
run:
	./${APPNAME} ${IP} ${PORT} ${TIME}


