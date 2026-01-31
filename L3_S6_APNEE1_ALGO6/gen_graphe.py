import sys
import random

def usage():
    print("Usage: python3 gen_graphe.py <nb_sommets> <densité_arcs> <fichier_sortie> [force_solution]")
    print("Ex: python3 gen_graphe.py 1000 3 tests/test_1000.txt 1")
    sys.exit(1)

if len(sys.argv) < 4:
    usage()

nb_sommets = int(sys.argv[1])
densite = float(sys.argv[2]) # Nombre moyen d'arcs par sommet (approx)
output_file = sys.argv[3]
force_solution = len(sys.argv) > 4 and sys.argv[4] == "1"

arcs = []
arc_id = 1

# Si on veut forcer une solution, on crée d'abord un couplage parfait
# On relie i avec i+1 pour i pair
sommets = list(range(1, nb_sommets + 1))
random.shuffle(sommets)

if force_solution and nb_sommets % 2 == 0:
    for i in range(0, nb_sommets, 2):
        u = sommets[i]
        v = sommets[i+1]
        arcs.append(f"{arc_id}/{u}+{v}/->1")
        arc_id += 1

# Ajout d'arcs aléatoires (bruit)
nb_arcs_random = int(nb_sommets * densite)
for _ in range(nb_arcs_random):
    u = random.randint(1, nb_sommets)
    v = random.randint(1, nb_sommets)
    if u != v:
        arcs.append(f"{arc_id}/{u}+{v}/->1")
        arc_id += 1

# Écriture du fichier
try:
    with open(output_file, 'w') as f:
        f.write(f"{nb_sommets}\n")
        # Mélanger les arcs pour ne pas faciliter la tâche avec l'ordre de lecture
        random.shuffle(arcs)
        for arc in arcs:
            f.write(f"{arc}\n")
    print(f"Généré: {output_file} ({nb_sommets} sommets, {len(arcs)} arcs)")
except IOError as e:
    print(f"Erreur d'écriture: {e}")
