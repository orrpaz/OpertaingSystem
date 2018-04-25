#!/bin/bash
# Or Paz 311250708
if [ $# -ne 1 ]; then
    echo "error: only one argument is allowed"

elif [ ! -f "$1"  ]; then
	echo "error: there is no such file"
else
mkdir -p ./safe_rm_dir
cp "$1" ./safe_rm_dir
rm "$1"
echo "done!"
fi
