#!/bin/sh -f
#Utility used by SetUpROOT.csh to create necessary directories
host_dir_ver=$1
host_dir=$2
if [ ! -d $host_dir_ver ]; then
	mkdir $host_dir_ver
	echo Created new directory $host_dir_ver
fi
if [ ! -d $host_dir ]; then
	mkdir $host_dir
	echo Created new directory $host_dir
fi
if [ ! -d $host_dir/include ]; then
	mkdir $host_dir/include
fi
if [ ! -h $host_dir_ver/include ]; then
	ln -s $host_dir/include $host_dir_ver/include
fi
if [ ! -d $host_dir/tutorials ]; then
	mkdir $host_dir/tutorials
fi
if [ ! -h $host_dir_ver/tutorials ]; then
	ln -s $host_dir/tutorials $host_dir_ver/tutorials
fi
if [ ! -d $host_dir/test ]; then
	mkdir $host_dir/test
fi
if [ ! -h $host_dir_ver/test ]; then
	ln -s $host_dir/test $host_dir_ver/test
fi
if [ ! -d $host_dir/man ]; then
	mkdir $host_dir/man
fi
if [ ! -h $host_dir_ver/man ]; then
	ln -s $host_dir/man $host_dir_ver/man
fi
if [ ! -d $host_dir/macros ]; then
	mkdir $host_dir/macros
fi
if [ ! -h $host_dir_ver/macros ]; then
	ln -s $host_dir/macros $host_dir_ver/macros
fi
if [ ! -d $host_dir/icons ]; then
	mkdir $host_dir/icons
fi
if [ ! -h $host_dir_ver/icons ]; then
	ln -s $host_dir/icons $host_dir_ver/icons
fi
if [ ! -d $host_dir/fonts ]; then
	mkdir $host_dir/fonts
fi
if [ ! -h $host_dir_ver/fonts ]; then
	ln -s $host_dir/fonts $host_dir_ver/fonts
fi
if [ ! -d $host_dir/etc ]; then
	mkdir $host_dir/etc
fi
if [ ! -h $host_dir_ver/etc ]; then
	ln -s $host_dir/etc $host_dir_ver/etc
fi
if [ ! -d $host_dir/build ]; then
	mkdir $host_dir/build
fi
if [ ! -h $host_dir_ver/build ]; then
	ln -s $host_dir/build $host_dir_ver/build
fi
