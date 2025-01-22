#!/usr/bin/env bash

reset="\033[0m"
bleu="\033[34m"
cyan="\033[36m"


# Script pour formater des fichiers Python avec support des arguments

# Vérification des outils nécessaires
TOOLS=("black" "isort" "autopep8")
MISSING_TOOLS=()

for TOOL in "${TOOLS[@]}"; do
    if ! command -v "$TOOL" &> /dev/null; then
        MISSING_TOOLS+=("$TOOL")
    fi
done

if [ ${#MISSING_TOOLS[@]} -ne 0 ]; then
    echo -e "Les outils suivants sont manquants : ${MISSING_TOOLS[*]}"
    echo -e "Installez-les tous avec : $cyan sudo apt install $bleu ${MISSING_TOOLS[*]} $reset  ou bien seulement (exemple): $cyan sudo apt install $bleu python3-autopep8 $reset"
    exit 1
fi

# Fonction pour afficher l'utilisation
usage() {
    echo -e "$cyan Usage : $0 [fichier1.py fichier2.py ... | all]"
    echo -e "$cyan    fichier(s).py   : Spécifie un ou plusieurs fichiers Python à formater."
    echo -e "$cyan    all             : Formate tous les fichiers Python récursivement dans le répertoire courant."
    echo -e "$cyan    Sans argument   : Affiche ce message d'aide. $reset"
    exit 1
}

# Fonction pour formater un fichier Python
format_file() {
    local FILE=$1

    # 1. Tri des imports avec isort
    isort "$FILE" &> /dev/null 

    # 2. Application de Black pour un formatage global
    black "$FILE" &> /dev/null

    # 3. Ajustement final avec autopep8
    autopep8 --in-place "$FILE" &> /dev/null 

}

# Gestion des arguments
if [ $# -eq 0 ]; then
    # Pas d'arguments
    usage
else
    if [ "$1" == "all" ]; then
        # Formater tous les fichiers Python dans le répertoire courant (récursivement)
        echo "Recherche des fichiers Python dans le répertoire courant..."
        PYTHON_FILES=$(find "$(pwd)" -type f -name "*.py")

        if [ -z "$PYTHON_FILES" ]; then
            echo "Aucun fichier Python trouvé."
            exit 0
        fi


        for FILE in $PYTHON_FILES; do
            format_file "$FILE"            
        done

    else
        # Formater les fichiers spécifiés en arguments
        for FILE in "$@"; do
            if [ -f "$FILE" ]; then
                format_file "$FILE"
            else
                echo "⚠️  Fichier non trouvé : $FILE"
            fi
        done
    fi
fi
