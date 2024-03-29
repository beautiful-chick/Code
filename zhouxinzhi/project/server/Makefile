INCLUDE_PATH=`pwd`/../include

OBJ_NAME=server

LIB_PATH=`pwd`/../lib

CC=gcc
AR=ar

all:

run:
	${CC} ${OBJ_NAME}.c -o ${OBJ_NAME}.o -I ${INCLUDE_PATH} -L ${LIB_PATH}  -lmylib -ltemp -lsqlite3   
	export LD_LIBRARY_PATH=/home/iot24/zhouxinzhi/project/lib/ && ./${OBJ_NAME}.o -p 8989 -d 0 
