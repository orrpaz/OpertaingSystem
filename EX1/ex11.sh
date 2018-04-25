#!/bin/bash
# Or Paz 311250708
cd "$1" 2> /dev/null
FILE=`ls *.txt 2> /dev/null | wc -l` 
echo "Number of files in the directory that end with .txt is $FILE"
