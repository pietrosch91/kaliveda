#!/bin/sh
# Set up QTCreator project files:
# fill kaliveda.files with all source files under bzr control
# fill kaliveda.includes with output of root-config --incdir and all
#   directories under bzr control

bzr ls -R -V -k file | \egrep '\.h$|\.cpp$|\.c$|\.sh$|\.csh$|\.m4$|\.cxx$|\.C$|\.cmake$|\.in$|CMakeLists.txt' > kaliveda.files

echo '.' > kaliveda.includes

bzr ls -R -V -k directory >> kaliveda.includes

# to fix a bug in QtCreator 3.0, copy all ROOT headers used by KV sources to local directory
echo '.root-headers' >> kaliveda.includes

mkdir -p .root-headers

HEADERLIST=`\grep '#include [",<][R,T].*\.h[",>]' */*/*.h | awk '{print $2}' | sed 's/["<]//' | sed 's/[">]//' | sort | uniq`;
for h in $HEADERLIST; do
	cp -u `root-config --incdir`/$h .root-headers/;
done

HEADERLIST=`\grep '#include [",<][R,T].*\.h[",>]' */*/*.cpp | awk '{print $2}' | sed 's/["<]//' | sed 's/[">]//' | sort | uniq`;
for h in $HEADERLIST; do
	cp -u `root-config --incdir`/$h .root-headers/;
done

HEADERLIST=`\grep '#include [",<][R,T].*\.h[",>]' execs/*.c* | awk '{print $2}' | sed 's/["<]//' | sed 's/[">]//' | sort | uniq`;
for h in $HEADERLIST; do
	cp -u `root-config --incdir`/$h .root-headers/;
done

