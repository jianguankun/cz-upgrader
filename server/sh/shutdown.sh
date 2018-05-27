#!/bin/sh
pid=`sed -n '1p' gameinfo.pid`
kill -9 $pid
