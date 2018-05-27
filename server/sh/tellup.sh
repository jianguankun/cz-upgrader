#!/bin/sh
self_dir=$(cd "$(dirname "$0")"; pwd)

function split
{
    OLD_IFS="$IFS"
    IFS="."
    array=($1)
    IFS="$OLD_IFS"
    echo ${array[*]}
}

function version_cmp
{ 
    v1arr=($1)
    v2arr=($2)
    v1len=${#v1arr[@]}
    v2len=${#v2arr[@]}
    minlen=$v1len
    if [ $v1len -gt $v2len ]; then
        minlen=$v2len
    fi

    for((i = 0; i < $minlen; i++))
    do
        #echo ${v1arr[i]} - ${v2arr[i]}
        if [ ${v1arr[i]} -gt ${v2arr[i]} ]; then 
            echo 1 && return
        elif [ ${v1arr[i]} -lt ${v2arr[i]} ]; then 
            echo -1 && return
        fi
    done

    if [ $v1len -gt $v2len ]; then
        echo 1 && return
    elif [ $v1len -lt $v2len ]; then
        echo -1 && return
    fi

    echo 0 && return
}

if [ ! -n "$1" ]; then
    for file in ${self_dir}/*.xml
    do  
        filename=`basename $file  .xml`  
        #echo $filename
        curr=`split $filename`
        cmp=`version_cmp "${curr[*]}" "${maxver[*]}"`
        if [ $cmp -eq 1 ]; then
            maxver=$curr
            maxfile=`basename $file`  
        fi
    done
else
    maxfile=$1
fi

if [ ! -f "./config" ]; then
    echo Error.Config file not find.
    exit 0
fi

name=`sed '/^project_name=/!d;s/.*=//' config`
url=`sed '/^tellup_url=/!d;s/.*=//' config`

if [ ! -n "$name" ] || [ ! -n "$url" ]; then 
    echo Error. PROJECT_NAME or TELLUP_URL value is invalid.
    exit 0
fi

echo $name
echo $url
curl ${url}/api/hallupgrade.jsp?valueStr=$maxfile
