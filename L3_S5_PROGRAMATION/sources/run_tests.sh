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

needed=("awk" "arm-none-eabi-readelf") # "arm-none-eabi-ld" "arm-none-eabi-gcc")
declare -a missing

# sort du programme en affichant un message d'erreur, en renvoyant un code d'erreur arbitraire
die() {
  local stat=$1
  shift
  [ $stat -ne 0 ] && echo -en "${RED_BOLD}[ERREUR]${RESET} "
  echo -en "${RED}$*${RESET}\n" 
  exit $stat
}

for cmd in ${needed[@]}; do
  command -v "$cmd" &> /dev/null || missing+=("$cmd") # ajout 
done

[ ${#missing[@]} -gt 0 ] && die 1 "les commande suivantes sont non-exécutables : ${missing[@]}\nVérifier qu'elles sont installées"

usage() {
  # echo "usage : $0 [-h] [-e <etape>] [-v {col | lig}] [-l]"
  echo "usage : $0 [-h] [-e <etape>] [-v {col | lig}] [-f]"
  echo "     -h               help, affiche ce message"
  echo "     -e <etape>       de 1 à 11, effectue les tests seulement pour l'étape choisie"
  echo "     -v {col | lig}   verbose, en colonnes (recommandé) ou en lignes, affiche la différence entre le résultat attendu et obtenu, de manière lisible"
  echo "                      (fonctionne seulement pour les étapes 1 à 5) (incompatible avec '-c')"
  # echo "     -l               vérifie les fuites de mémoire"
  echo "     -f               filtre les tests pour afficher seuelement ceux qui échouent"
}

# pourquoi utiliser un fichier 'tests.run' pur chaque étape ? : pour chaque étape on peut choisir les commandes à effectuer 
# + faire des commandes complexes qui s'appliquent à un patterne seulement, typiquement, on peut séparer les fichiers causant des erreurs des autres qui produisent souvent un affichage différents

###################################################################
# Les variables utilisables par les fichiers 'tests.run' sont:    #
# - $test_file : chemin complet jusqu'au fichier de test          #
# - $fich1 : chemin complet jusqu'au fichier résultat correct     #
# - $fich2 : chemin complet jusqu'au fichier résultat à vérifier  #
# - $temp_file : fichier temporaire pour stocker des choses       #
###################################################################

# même logique qu'en C, seulement 'getopts' ne supporte pas les options longues, il faudrait utiliser 'getopt'
# ici on se contente d'options courtes ...

# while getopts 'he:v:l' OPTION; do
while getopts 'he:v:f' OPTION; do
  case "$OPTION" in
    "h") usage && die 0 "" ;;
    "e") etape_num=$OPTARG ;;
    "v") verbose=1 && diff_mode=$OPTARG;
        case "$diff_mode" in
          "col"|"lig") ;;
          *) die 1 "mode invalide pour '-v'" ;;
        esac
        ;;
    # "l") memcheck=1 ;;
    "f") filter=1 ;;
    *) usage && die 1 "" ;;
  esac
done

echo_etape_name() {
  echo -e "${BOLD}------------------------------------------------------------------------ $(echo $inst_file | awk -F'.' '{printf "Phase " $2}') Etape $num ------------------------------------------------------------------------${RESET}"
}

sed_string='/^$/d; s/ //g' # string utilisé par sed pour filtrer les sorties, ça nous permet de ne pas prendre en compte les indentations et les espaces
# ici, les variables contiennent le chemin entier jusqu'au test, sans faire cd, pour faciliter l'écriture des fichiers 'tests.run'
test_dir=tests

for etape in `ls -v $test_dir/tests.?.*.run`; do ####################### POUR CHAQUE SOUS-DOSSIER D'ETAPE #################################################################################################
    num=$(echo $etape | awk -F'.' '{print $3}')
    [ -n "$etape_num" -a "$etape_num" != "$num" ] && continue # quand l'option '-e' est utilisée, filtre les autres sous-dossiers
    # etape=$test_dir/$etape
    inst_file=$etape # fichier d'instructions 'tests.run' où chaque ligne correspond à un patterne qui décrit des fichiers tests, ce qui permet : - des traitements différents par patterne, des commande plus complexes, 
    # etape=$(dirname $etape)
    [ ! -e $inst_file ] && continue

    echo_etape_name

    while read patterne fich1 fich2 cmd; do # POUR CHAQUE LIGNE/INSTRUCTION CORRESPONDDANT A UN PATTERNE #######################################################################################
      fich1=$test_dir/$fich1 
      fich2=$test_dir/$fich2
      temp_file=$test_dir/temp_file.delete
      for test_file in `ls $test_dir | awk "$patterne"`; do # POUR CHAQUE FICHIER DU SOUS-DOSSIER QUI MATCH LE PATTERNE #################################################################################
        test_file=$test_dir/$test_file
        touch $fich1 $fich2 $temp_file # les fichiers sont créés par redirection, sauf qu'une sortie vide ne crée pas de fichier, donc on 'touch' pour permettre de faire 'diff' dans tous les cas
        tn=$(echo $test_file | awk -F'.' '{print $(NF-1)}')
        ([ "$test_file" = "$inst_file" ] ||
        [[ "$test_file" =~ tests.*.*.run ]] ||
        [ "$test_file" = "$fich1" ] || 
        [ "$test_file" = "$fich2" ] ||
        [ "$test_file" = "$temp_file" ] ||
        ([ $num -le 5 -a $num -ge 1 -a $num -ne 4 ] && [[ "$test_file" =~ .*.dump ]]) ||
        ([ $num -le 11 -a $num -ge 6 -o $num -eq 4 ] && [[ "$test_file" =~ .*.data ]]) ||
        [[ "$test_file" =~ .*.dump && $tn -ne $num && $tn -gt 0 ]]) && continue # les fichiers 'tests.run' ne doivent pas être traités, on passe au prochain fichier du répertoire étape courant
        ret=0
        ret=$(eval "$cmd")
        succes=0
        diff $fich1 $fich2 &> /dev/null && succes=1
        [ -n "$filter" -a "$succes" -eq 1 ] && continue
        [ $succes -eq 1 ] && echo -en "${GREEN_BOLD}PASS${RST2}" || echo -en "${RED_BOLD}FAIL${RST2}"
        bname=$(basename $test_file | awk -F'.' '{$NF=""; for (i = 1; i < (NF-1); i++) {printf("%s.", $i)}; print $(NF-1) }')
        echo -en "${BOLD} - $bname${RESET}"
        [ $ret -eq 139 ] && echo -en " - ${YELLOW_BOLD}SEGFAULT${RST2}" # 139 = segfault
        [ $ret -eq 136 ] && echo -en " - ${YELLOW_BOLD}SIGFPE (div par 0)${RST2}"
        [ -n "$verbose" ] && echo -en " |\n" || echo
        if [ -n "$verbose" -a $succes -eq 0 ]; then
          for ((i = 1; i <= ${#bname}; i++)); do
            echo -en "-"
          done
          # '- PASS' ou '- FAIL' = même longueur
          echo -en "--------"
          [ $ret -eq 139 ] && echo -en "-----------"
          [ $ret -eq 136 ] && echo -en "---------------------"
          echo
        fi
        if [ -n "$verbose" ]; then 
          if [ "$diff_mode" = "col" ]; then 
              [ $succes -eq 0 ] && diff -y --color $fich1 $fich2 # 'diff -y' affiche apparement le contenu même quand les deux fichiers sont identiques, en tout cas ça peut arriver 
            else 
              diff --color $fich1 $fich2 # permet un affichage lisible de ce qui fait echouer le test, même si on récupérait la sortie du premier 'diff', ce serait illisible 
          fi
          echo "-----------------------------------------------------------------------------------------------------------------------------------------------------------------"
        fi
        rm $fich1 $fich2 $temp_file &> /dev/null # même raison d'usage que pour le 'touch', on peut tomber sur un ancien contenu si une redirection ne transmet rien
      done
      rm $fich1 $fich2 $temp_file &> /dev/null
    done < $inst_file
    # [ -z "$memcheck" ] && echo -e "\n (utiliser ${BOLD}-l${RESET} pour vérifier les fuites de mémoire)"
done
exit 0
