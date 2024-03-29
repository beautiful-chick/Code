LIB_NAME=mylib

INST_PATH=`pwd`/../lib

INCLUDE_PATH=`pwd`/../include

CC=gcc
AR=ar


all: dynamic_lib static_lib install clean
	
gcc: dynamic_lib static_lib  

dynamic_lib:
	${CC} -shared -fPIC temp.c -I ${INCLUDE_PATH} -o libtemp.so
	${CC} -shared -fPIC *.c -I ${INCLUDE_PATH} -o lib${LIB_NAME}.so
			  
static_lib:
	${CC} -c temp.c -I ${INCLUDE_PATH}
	${AR} -rcs libtemp.a temp.o 
	${CC} -c *.c -I ${INCLUDE_PATH}
	${AR} -rcs lib${LIB_NAME}.a *.o

install:
	cp -rf *.so ${INST_PATH}
	cp -rf *.a ${INST_PATH}

clear:
	rm -f *.o

clean: clear
	rm -f libtemp.so
	rm -f libtemp.a
	rm -f lib${LIB_NAME}.so
	rm -f lib${LIB_NAME}.a

