#!/usr/bin/env bash
#Generates a file containing random lines from $source_file
#Size of the generated file is roughly the second argument in megabytes
#Adjust size by calculating how many bytes per line your source file has and
#divide 1000000 by that
#Args:
#$1 - the file from which to pick lines
#$2 - the size of the generated file in megabytes
#$3 - the name of the generated file

source_file=$1
out_file=$3
lines_per_MB=50000
if [ -f $out_file ]; then
        rm $out_file
fi
touch $out_file
for ((i = 0; i < $2; ++i)); do
        #1MB = 1000000B, 20B/line ==> 50000 lines per MB
        shuf -n $lines_per_MB $source_file >> $out_file
done
