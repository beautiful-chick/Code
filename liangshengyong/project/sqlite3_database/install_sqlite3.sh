#!/bin/bash

if [ ! -d "install" ]; then
wget http://main.iot-yun.club:2211/src/sqlite-autoconf-3420000.tar.gz

tar -xzvf sqlite-autoconf-3420000.tar.gz

cd sqlite-autoconf-3420000

./configure --prefix=`pwd` --disable-static

make install
else
	echo "'install' folder already exists.Skipping installation"
fi
