#!/usr/bin/env bash

RESET="\033[0m"
RED="\033[91m"
RED_BOLD="\033[91;1m"
GREEN="\033[92m"

needed=("automake" "autoreconf")
declare -a missing

die() {
  local stat=$1
  shift
  echo -e "${RED_BOLD}[ERREUR]${RESET} $*" 
  exit $stat
}

for cmd in ${needed[@]}; do
  command -v "$cmd" &> /dev/null || missing+=("$cmd") 
done

[ ${#missing[@]} -gt 0 ] && die 1 "les commande suivantes sont non-exécutables : ${missing[@]}\nVérifier qu'elles sont installées"

[ -f Makefile ] && make distclean
autoreconf -vif 
for dir in Examples_*; do autoreconf -vif $dir; done
CFLAGS='' ./configure
