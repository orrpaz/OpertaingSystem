#!/bin/bash
# Or Paz 311250708
sum=0
cat $2 |( while read LINE
do
		if [[ $LINE = $1* ]]; then
		echo $LINE
		balance=$(echo $LINE | awk '{print $3}')
		sum=$[$sum + $balance]
	fi
done
echo "Total balance:" $sum)
