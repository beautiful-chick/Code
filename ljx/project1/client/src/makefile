LIBNAME=client
LIBPATH=/home/iot24/ljx/project1/client/lib
all:static_lib

static_lib:
	gcc -c get_tempature_time.c socket.c
	gcc -c database.c -I /home/iot24/ljx/install/include -L /home/iot24/ljx/install/lib
	ar -rcs lib${LIBNAME}.a *.o

clean:
	rm -r *.o
disclean:clean
	rm -f *.a
install:
	cp lib*.a ${LIBPATH}
uninstall:
	rm -f ${LIBPATH}/lib${LIBNAME}.*
