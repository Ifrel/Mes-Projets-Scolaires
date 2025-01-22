#!/usr/bin/env bash

make 
./reimplantation Examples_loader/example4.o --debug
./sample_run.sh resultat
arm-none-eabi-ld --section-start .text=0x58 --section-start .data=0x1000 -o prog ./Examples_loader/example4.o -EB

echo -e "\033[92m---------------- RESULTAT ----------------"
# arm-none-eabi-objdump -S ./resultat
arm-none-eabi-objdump -D ./resultat
echo -e "\033[0m"
echo 
echo 
echo 
echo
echo
echo -e "\033[93m---------------- ATTENDU ----------------"

# arm-none-eabi-objdump -S ./prog
arm-none-eabi-objdump -D ./prog

echo -e "\033[0m"
