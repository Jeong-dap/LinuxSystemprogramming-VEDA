#!/bin/bash

if [ $# -ne 1 ]; then
	echo "Usage: $0 filename"
	exit 1
fi

scp ./$1 njd990603@172.20.33.119:/home/njd990603/
