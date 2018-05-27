#!/bin/sh
for file in /data/www/*
do
    if test -d $file; then
        target=$file/hall/
        if test -d $target; then
            echo "Publish shell file to : "$target  "[OK]."
            cp tellup.sh $target
	else
            echo "Publish shell file to : "$target  "[Fail] dir not exist."
	fi
    fi
done
