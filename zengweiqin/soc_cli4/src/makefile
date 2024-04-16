#动态库，静态库名字
LIBNAME=mytime
#头文件和生成的动态库和静态库路径
INSTPATH=/home/iot24/zengweiqin/soc_cli4/libku/

CC=gcc
AR=ar

all:dynamic_lib static_lib
	@make clear
	@make install

dynamic_lib:
	${CC} -shared -fPIC temp_setitimer.c -o lib${LIBNAME}.so

static_lib:
	${CC} -c temp_setitimer.c
	${AR} -rcs lib${LIBNAME}.a *.o

install:
	cp -rf lib${LIBNAME}.* ${INSTPATH}
	cp -rf *.h ${INSTPATH}

clear:
	rm -f *.o

#clean是一个单独的目标，依赖clear，所以先通过clear目标删除所有的object文件，再删除生成的库文件，因为我已经拷贝走了。
#该目标单独输入来执行。
clean:clear
	rm -f lib${LIBNAME}.*

