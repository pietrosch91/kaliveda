#!/bin/sh -f
#Utility used by SetUpKaliVeda.csh to create necessary directories
host_dir_ver=$1
host_dir=$2
if [ ! -d $host_dir_ver ]; then
	mkdir $host_dir_ver
	echo Created new directory $host_dir_ver
	chmod g+w $host_dir_ver
fi
if [ ! -d $host_dir ]; then
	mkdir $host_dir
	echo Created new directory $host_dir
	chmod g+w $host_dir
fi
if [ ! -d $host_dir/include ]; then
	mkdir $host_dir/include
	chmod g+w $host_dir/include
fi
if [ ! -h $host_dir_ver/include ]; then
	ln -s $host_dir/include $host_dir_ver/include
fi
if [ ! -d $host_dir/tools ]; then
	mkdir $host_dir/tools
	chmod g+w $host_dir/tools
fi
if [ ! -h $host_dir_ver/tools ]; then
	ln -s $host_dir/tools $host_dir_ver/tools
fi
if [ ! -d $host_dir/KVFiles ]; then
	mkdir $host_dir/KVFiles
	chmod g+w $host_dir/KVFiles
fi
if [ ! -h $host_dir_ver/KVFiles ]; then
	ln -s $host_dir/KVFiles $host_dir_ver/KVFiles
fi
