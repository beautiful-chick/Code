LIBRARY_PATH := inc/
SQL_PATH	 := sqlite3_ku/include

CC=gcc
AR=ar

./PHONY: all ds_lib socket_lib

all: ds_lib socket_lib 
	${CC}	src/socket_client.c -I$(LIBRARY_PATH) -I$(SQL_PATH) -Lsrc/ -Lsqlite3_ku/lib/ -lds18b20_project -lsocket_pack -lsqlite3  -o src/socket_client
	@make clean_a_o
ds_lib:	
	${CC}	-c	src/ds18b20_project.c -I$(LIBRARY_PATH)  -o src/ds18b20_project.o
	${AR}	-rcs src/libds18b20_project.a	src/ds18b20_project.o

socket_lib:
	${CC}	-c 	src/socket_pack.c -I$(LIBRARY_PATH) -o src/socket_pack.o
	${AR}	-rcs src/libsocket_pack.a src/socket_pack.o

clean_a_o:
	@rm -f src/*.a src/*.o
clean:
	@rm -f src/socket_client
