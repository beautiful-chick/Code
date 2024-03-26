LIBNAME=rpi_server
INSTPATH=`pwd`/../lib/

CC=gcc
AR=ar

all: dynameic_lib static_lib
	@make clear
	@make install

dynameic_lib:
	${CC} -shared -fPIC *.c -I ../../sqlite/install/include/ -o lib${LIBNAME}.so

static_lib:
	${CC} -c *.c -I ../../sqlite/install/include/
	${AR} -rcs lib${LIBNAME}.a *.o

install:
	cp -rf lib${LIBNAME}.* ${INSTPATH}
	cp -rf *.h ${INSTPATH}

uninstall:
	rm -f ${INSTPATH}/lib${LIBNAME}.*
	rm -f ${INSTPATH}/*.h

clear:
	rm -f *.o

clean: clear
	rm -f lib${LIBNAME}.*
