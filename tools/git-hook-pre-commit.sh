#!/bin/bash
# Automatically reformat any committed code using astyle

ASTYLE_PARAMETERS="--suffix=none --options=./tools/astyle.rc"
FILE_PATTERN="\.(c|cpp|cxx|C|h)$"

FILES=`git diff-index --cached -r --name-only HEAD --diff-filter=ACMR |  egrep $FILE_PATTERN`

if [ "x$FILES" != "x" ]; then

   ASTYLE=$(which astyle)
   if [ $? -ne 0 ]; then
	   echo "git pre-commit hook:"
	   echo "astyle not installed. Unable to check source file format policy."
	   echo "You must install astyle if you want to commit your changes"
	   exit 1
   fi

   for file in $FILES; do
      if [ "x$file" != "xKVIndra/analysis/KVSelector.cpp" ]; then
	      $ASTYLE $ASTYLE_PARAMETERS $file
         git add $file
      else
         echo "Formatting disabled for KVSelector.cpp"
      fi
   done

fi
