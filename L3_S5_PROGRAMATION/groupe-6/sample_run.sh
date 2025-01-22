#!/bin/sh

[ $# -lt 1 ] && echo "donner un nom de fichier à tester" && exit 1
[ ! -f $1 ] && echo "le fichier '$1' n'existe pas" && exit 1

./arm_simulator --gdb-port 6666 --trace-registers --trace-memory --trace-state SVC &
sleep 1
./ARM_runner --debug --host localhost --service 6666 $1
