#!/bin/sh

# Check if there are two arguments
if [ $# -eq 2 ]; then
   if ! [ -d "$2" ]; then
     /bin/echo "$2 is not a directory."
     exit 1
   fi
else
   /bin/echo "Usage: $0 <executable> <folder with tests>"
   exit 1
fi

for i in `ls $2/*.txt`; do
    # run the executable to produce the result
    ./$1 $i
done
