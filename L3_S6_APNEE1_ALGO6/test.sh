#!/bin/bash

# Compilation (au cas où)
javac *.java

# Couleurs pour l'affichage
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m' # No Color

run_test() {
    file=$1
    echo -n "Test sur $file ... "
    
    # Mesure du temps d'exécution
    start_time=$(date +%s%N)
    
    # Exécution et vérification
    # On capture la sortie standard et on la passe à verif_sortie.py
    # On stocke aussi la sortie pour détecter "Couplage impossible" ou "non trouve"
    output=$(java EssaiGraphe "$file")
    exit_code=$?
    
    end_time=$(date +%s%N)
    duration=$(( (end_time - start_time) / 1000000 )) # en millisecondes

    # Vérification avec le script Python fourni
    # Attention: verif_sortie.py lit depuis stdin
    verification=$(echo "$output" | python3 verif_sortie.py 2>&1)
    verif_code=$?

    if [ $verif_code -eq 0 ]; then
        # Le script python a validé (ou c'était un cas valide "non trouvé")
        # Analysons si c'est une réussite totale ou un cas négatif valide
        if echo "$output" | grep -q "Couplage trouve"; then
            echo -e "${GREEN}SUCCÈS${NC} (Couplage valide trouvé) - ${duration}ms"
        elif echo "$output" | grep -q "Couplage impossible"; then
            echo -e "${GREEN}SUCCÈS${NC} (Détecté comme impossible) - ${duration}ms"
        elif echo "$output" | grep -q "Couplage non trouve"; then
             echo -e "${GREEN}SUCCÈS${NC} (Pas de couplage trouvé) - ${duration}ms"
        else
             echo -e "${RED}ÉCHEC${NC} (Sortie inattendue: $output) - ${duration}ms"
        fi
    else
        echo -e "${RED}ÉCHEC${NC} - ${duration}ms"
        echo "Erreur détectée par verif_sortie.py :"
        echo "$verification"
    fi
}

echo "=== Lancement des tests fournis (dossier exemples) ==="
for f in exemples/*; do
    [ -e "$f" ] || continue
    run_test "$f"
done

if [ -d "tests" ]; then
    echo ""
    echo "=== Lancement des tests générés (dossier tests) ==="
    for f in tests/*.txt; do
        [ -e "$f" ] || continue
        run_test "$f"
    done
else
    echo ""
    echo "Pas de dossier 'tests/'. Lancez 'make gen-tests' pour créer de grands graphes."
fi
