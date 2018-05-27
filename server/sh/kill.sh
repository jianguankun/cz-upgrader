#/bin/sh
port=$1
port=${port:?"No listen port specify."}
pid=`netstat -tnlp | grep $port | awk '{print $7}' | cut -d/ -f1`
pid=${pid:?"No process using port ${port}"}
kill -9 $pid
echo "Process ${pid} using listen port ${port} is killed."
