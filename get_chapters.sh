#!/bin/bash
set -ex

URL="http://compilers.iecc.com/crenshaw/"

for i in $(seq 1 16); do  

	file="tutor"$i".txt"
	mkdir -p $i
	cd $i

	if [ ! -f $file ];
	then
		wget $URL""$i
	fi

	cd ..
echo $i; done;
