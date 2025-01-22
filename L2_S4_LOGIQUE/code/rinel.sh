#!/bin/bash

if [ $# -eq 2 ]; then
    
    for ((i=$1; i<=$2; i++)); do
        echo "________Program $i _____________"
        /usr/bin/python3 main.py tests/jeu_$i.json >> results.txt
    done;
    exit 0
fi

if [ $# -eq 1 ]; then
    echo "________Program $1 _____________"
    /usr/bin/python3 main.py tests/jeu_$1.json > results.txt
    exit 0
fi

echo "Usage: $0 <nombre du test> ou <debut nombre de test> <fin nombre de test>"
exit 1

    

