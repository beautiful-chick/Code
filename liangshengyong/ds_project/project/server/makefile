LIBRARY_PATH := inc/
SQL_PATH	 := sqlite3_ku/include/

cc=	gcc
AR= ar

./PHONY: all socket_serv_lib

all: socket_serv_lib
	${CC} src/server_epoll.c -I$(LIBRARY_PATH) -I$(SQL_PATH) -Lsrc/ -Lsqlite3_ku/lib -lsocket_server -lsqlite3 -o src/server_epoll
	@make clean_a_o

socket_serv_lib:
	${CC} -c src/socket_server.c -I$(LIBRARY_PATH) -o src/socket_server.o
	${AR} -rsc src/libsocket_server.a src/socket_server.o

clean_a_o:
	@rm -f src/*.a src/*.o

clean:
	@rm -f src/server_epoll
