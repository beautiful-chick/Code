#********************************************************************************
#      Copyright:  (C) 2023 LingYun IoT System Studio
#                  All rights reserved.
#
#       Filename:  Makefile
#    Description:  This file used compile all the source code to static library
#
#        Version:  1.0.0(11/08/23)
#         Author:  Guo Wenxue <guowenxue@gmail.com>
#      ChangeLog:  1, Release initial version on "11/08/23 16:18:43"
#
#*******************************************************************************

PWD=$(shell pwd )

BUILD_ARCH=$(shell uname -m)
ifneq ($(findstring $(BUILD_ARCH), "x86_64" "i386"),)
    CROSS_COMPILE?=arm-linux-gnueabihf-
endif

LIBNAME=$(shell basename ${PWD} )
TOPDIR=$(shell dirname ${PWD} )
CFLAGS+=-D_GNU_SOURCE

all: clean
	@rm -f *.o
	@${CROSS_COMPILE}gcc ${CFLAGS} -I${TOPDIR} -c *.c
	${CROSS_COMPILE}ar -rcs  lib${LIBNAME}.a *.o

clean:
	@rm -f *.o
	@rm -f *.a

distclean:
	@make clean
