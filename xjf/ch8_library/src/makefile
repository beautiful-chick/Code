#定义变量指定生成静态库和动态库的名字
LIBNAME=mycrypto

#定义一个变量指定库文件和头文件的安装路径
INSTPATH=../lib/

#定义编译器，如果今后交叉编译器的话，只需要在这里改成交叉编译器即可。
CC=gcc
AR=ar 

#定义总目标
all: dynamic_lib static_lib
	@make clear
	@make install

#dynamic_lib目标用来编译生成动态库，它是all目标的一个依赖
dynamic_lib:
	${CC} -shared -fPIC *.c -o lib${LIBNAME}.so

#static_lib目标用来编译生成静态库，它是all目标的一个依赖
static_lib:
	${CC} -c *.c
	${AR} -rcs lib${LIBNAME}.a *.o

#install是一个单独的目标，用来将编译生成的库文件和头文件拷贝到相应的安装路径下。
install:
	cp -rf lib${LIBNAME}.* ${INSTPATH}
	cp -rf *.h ${INSTPATH}

#uninstall是一个单独的目标，用来在安装路径下删除之前安装的库文件和头文件。
#该目标没有被总的目标all依赖或执行，所以默认该目标不会被执行，如果想执行该目标，使用make uninstall命令
uninstall:
	rm -f ${INSTPATH}lib${LIBNAME}.*
	rm -f ${INSTPATH}*.h

#clear是一个单独的目标，用来将编译生成的object临时文件删除。
clear:
	rm -f *.o

#clean是一个单独的目标，它依赖clear目标，所以先通过clear目标删除所有object临时文件，之后再删除编译产生的库文件
#该目标没有被总的目标all依赖或执行，所以默认该目标不会被执行，如果想执行该目标，使用make clean命令
clean: clear
	rm -f lib${LIBNAME}.*


