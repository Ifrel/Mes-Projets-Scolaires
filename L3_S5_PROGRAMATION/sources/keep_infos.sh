#!/usr/bin/env bash

RESET="\033[0m"
RST2="${RESET}${RESET}"
BOLD="\033[1m"
RED="\033[91m"
RED_BOLD="\033[91;1m"
GREEN="\033[92m"
GREEN_BOLD="\033[92;1m"
YELLOW="\033[93m"
YELLOW_BOLD="\033[93;1m"

die() {
  local stat=$1
  shift
  [ $stat -ne 0 ] && echo -en "${RED_BOLD}[ERREUR]${RESET} "
  echo -en "${RED}$*${RESET}\n" 
  exit $stat
}

usage() {
  echo "usage : $0 [-h] [-d <dump>] [-e <etape> ] -f <file>"
  echo "         -h : help"
  echo "         -d : produit un dump de nom <dump>"
  echo "         -e : ne traite que les informations pertinentes pour l'étape <etape>"
}

[ $# -lt 1 ] && usage && die 1 ""

[ ! -e custom_readelf ] && die 1 "'./custom_readelf' non trouvé dans le répertoire courant"


while getopts 'hd:e:f:' OPTION; do
  case "$OPTION" in
    "h") usage && die 0 "" ;;
    "d") dmp=1 && dump=$OPTARG ;;
    "e") et=1 && etape=$OPTARG ;;
    "f") f=$OPTARG ;;
    *) usage && die 1 "" ;;
  esac
done

[ -z "$dmp" ] && dump=$(basename $f).dump
[ ! -e $f ] && echo "no such file '$f'" && exit 0

sep() {
  echo "----------------------------------------------------------------------------------" &>> $dump
}

[[ -n "$et" ]] && [[ $etape -eq 6 ]] && ./custom_readelf -h $f &> $dump
[[ -n "$et" ]] && [[ $etape -eq 6 ]] && sep
[[ -n "$et" ]] && [[ $etape -eq 6 ]] && ./custom_readelf -S $f &>> $dump
[[ -n "$et" ]] && [[ $etape -eq 6 ]] && sep
[[ -n "$et" ]] && [[ $etape -ge 6 ]] && ./custom_readelf -s $f &>> $dump
[[ -n "$et" ]] && [[ $etape -ge 7 ]] && sep
[[ -n "$et" ]] && [[ $etape -ge 7 ]] && ./custom_readelf -r $f &>> $dump
[[ -n "$et" ]] && [[ $etape -ge 7 ]] && sep
[[ -n "$et" ]] && [[ $etape -ge 7 ]] && ./custom_readelf -x .rel.text $f &>> $dump
[[ -n "$et" ]] && [[ $etape -ge 7 ]] && sep
[[ -n "$et" ]] && [[ $etape -ge 7 ]] && ./custom_readelf -x .rel.data $f &>> $dump
[[ -n "$et" ]] && [[ $etape -ge 7 ]] && sep
[[ -n "$et" ]] && [[ $etape -ge 7 ]] && ./custom_readelf -x .text $f &>> $dump
[[ -n "$et" ]] && [[ $etape -ge 7 ]] && sep
[[ -n "$et" ]] && [[ $etape -ge 7 ]] && ./custom_readelf -x .data $f &>> $dump
[[ -n "$et" ]] && [[ $etape -ge 7 ]] && sep
[[ -n "$et" ]] && [[ $etape -ge 7 ]] && ./custom_readelf -x .symtab $f &>> $dump
[[ -n "$et" ]] && [[ $etape -ge 7 ]] && sep
[[ -n "$et" ]] && [[ $etape -ge 7 ]] && ./custom_readelf -x .shstrtab $f &>> $dump
[[ -n "$et" ]] && [[ $etape -ge 7 ]] && sep
[[ -n "$et" ]] && [[ $etape -ge 7 ]] && ./custom_readelf -x .strtab $f &>> $dump
[[ -n "$et" ]] && [[ $etape -ge 7 ]] && sep
