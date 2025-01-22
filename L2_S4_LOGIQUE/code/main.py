#!/usr/bin/env python
import sys
import time

from types_custom import *
from utils import *
from fichier import *
# from generation import *
from prototype import *
from transformation import avoir_fnc
from affichage import afficher

argv = sys.argv
argc = len(argv)
solver = chemin_solver("cryptominisat5")

def usage():
    print(f'{argv[0]} [ -h | --help ] pour avoir ce message')
    print()
    print(f'Usage : {argv[0]} [ <nom_fichier> | RIEN ]\t(RIEN -> stdin)')
    exit(1)


def main() -> None:
    nom_fichier: str = ""

    if argc > 2:
        usage()
    elif argc == 2:
        if argv[1] in ("-h", "--help"):
            usage()
        nom_fichier = argv[1]
    elif argc == 1:
        nom_fichier = ""
    else:
        print("ERREUR")
        exit(1)

    #####################################################################


    formule: JEU
    config: dict[str, List]
    nb_var: int

    print("Création d'une instance du jeu (formule) ...")
    t0 = t1 = time.time()
    config, formule, nb_var = creer_jeu(nom_fichier)
    for i in formule:
        print()
        for j in i:
            print("\t",j)

    if (nom_fichier != ""):
        t2 = time.time()
        print(f'  | Fini en {round(t2 - t1, 5)} s')
    print(f'Instance créée, nb de variables = {nb_var:_}')
    print("----------------------------------------")
    #####################################################################
    
    nb_clauses: int
    fichier_dimacs: str
    if nom_fichier == "":
        fichier_dimacs = "out.dimacs"
    else:
        fichier_dimacs = name_dimacs(nom_fichier)
    print("Conversion en FNC + écriture de la formule ...")
    t1 = time.time()
    nb_clauses = avoir_fnc(config, fichier_dimacs, formule)
    t2 = time.time()
    print(f'  | Fini en {round(t2 - t1, 5)} s')
    print(f'Conversion + écriture effectuées, nb de clauses = {nb_clauses:_}')
    #####################################################################

    print("Recherche d'un modèle ...")
    t1 = time.time()
    modele = modele_fichier(fichier_dimacs, solver)
    t2 = time.time()
    print(f'  | Fini en {round(t2 - t1, 5)} s')
    #####################################################################

    print("Affichage de la solution ...\n")
    afficher(fichier_dimacs, config, modele)
    print(f" ---> Fini en {round(t2 - t0, 5)} s (Total)")
    #####################################################################

    print(f'{GREEN}#############')
    print("#  Terminé  #")
    print(f'#############{RESET}')



if __name__ == "__main__":
    main()
