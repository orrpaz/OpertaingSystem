#!/bin/bash
# Or Paz 311250708
cd "$1"
for file in *; do
if [[ -d $file ]]; then
    echo "$file is a directory"
elif [[ -f $file ]]; then
    echo "$file is a file"
fi
done