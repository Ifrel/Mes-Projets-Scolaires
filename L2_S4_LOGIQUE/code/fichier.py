import sys
import os
import random

from types_custom import *
from utils import *


#variable accessible par les fichier qui importent ce module -> une seule définition
argv = sys.argv
argc = len(argv)

# def taille(l: List) -> int:
#     """
#     Vérifie chaque liste / colonne pour s'assurer que les contraintes de format sont respéctées

#     ARGUMENTS
#     - l : liste ligne ou colonne
#     """
    
#     # A COMPLETER
#     return 0

# #----------------------------------------------------------------------------#

def lire_config(file: str) -> dict:
    """
    Lit verifie et renvoie la config du jeu depuis un fichier ou stdin

    ARGUMENTS
    - file : fichier où lire la config ou "" pour stdin
    """

    def clear_previous_line(n: int = 1):
        for _ in range(n):
            sys.stdout.write('\x1b[1A')  # Move up one line
            sys.stdout.write('\x1b[2K')   # Erase the line
            
    config: dict

    # Lecture de la config du jeu depuis un fichier
    if file != "":
        f = ouvrir_fichier(file, "r")
        config = json.load(f)
        f.close()
    
        dim = get_cle(config, "dim")
        liste_lignes = get_cle(config, "lignes")
        liste_colonnes = get_cle(config, "colonnes")

        # vérifier l'integité de la dimension
        if len(dim)!= 2:
            err_exit("dimenssions incorrectes")
                
        # vérifier que les lignes et les colonnes ne sont pas vides
        for i in range(dim[1]):
            if liste_lignes[i] == []:
                err_exit("config du jeu : ligne vide")

        for j in range(dim[0]):
            if liste_colonnes[j] == []:
                err_exit("config du jeu : colonne vide")
        
        # vérifier les dimensions au niveau de la ligne
        if len(liste_lignes) != dim[1]:
            err_exit(f"dim_colonne != Nbr_ligne: {dim[1]} != {len(liste_lignes)}")
        
        # vérifier les dimensions au niveau de la colonne
        if len(liste_colonnes)!= dim[0]:
            err_exit(f"dim_lingne != Nbr_colonne: {dim[0]} != {len(liste_colonnes)}")
        
        # vérifier que les valeurs des lignes et des colonnes ne depassent pas les dimensions
        for i in  liste_lignes:
            if (sum(i) + len(i) - 1) > dim[0]:
                err_exit(f"la valeur de la configuration de {i} depasse {dim[0]}")
        
        for j in liste_colonnes:
            if (sum(j) + len(j) - 1) > dim[1]:
                err_exit(f"la valeur de la configuration de {j} depasse {dim[1]}")


    # Lecture de la config du jeu depuis stdin
    else:
        config = {"dim": [0, 0], "lignes": [], "colonnes": []}
        
        dim_lig: int
        dim_col: int

        print("Choisir les dimensions du jeu [Nbr_ligne, Nbr_colonnes] :")
        d = True
        while d:
            dim_lig = input("Dimenssion  des lignes : ")
            dim_col = input("Dimenssion des colonnes : ")
            if dim_lig.isdigit() and dim_col.isdigit():
                config["dim"][0] = int(dim_lig)
                config["dim"][1] = int(dim_col)
                d = False
            else:
                clear_previous_line(3)
                print(f"[\033[31mentrée '{dim_lig}' '{dim_col}' invalide\033[0m ] Choisir les dimensions du jeu [Nbr_ligne, Nbr_colonnes] :")
        clear_previous_line(3)
        
        reponse = input("Valeurs des lignes et Colonnes:\n\t[1]: Saisie aleatoire\n\t[2]: Saisie manuelle\n> ")
        while reponse not in  ("1", "2"):
            clear_previous_line(4)
            reponse = input("[\033[31merreur\033[0m] Valeurs des lignes et Colonnes:\n\t[1]: Saisie aleatoire\n\t[2]: Saisie manuelle\n> ")
        clear_previous_line(4)

        dim_lig = config["dim"][0]
        dim_col = config["dim"][1]

        if reponse == "1": # saisie aleatoire
            # pour les lignes
            for _ in range(dim_col):
                ligne = []
                for j in range(random.randint(1, dim_lig)):
                    li = []
                    li.append(random.randint(0, dim_lig))
                    if not (li[0] == 0 and len(ligne) > 0 ) and 0 not in ligne: # si la ligne contient déja 0
                        if (sum(li) + sum(ligne) + len(li) + len(ligne) - 1) <= dim_lig: # si la valeur a ajouter fait cause un debordement de dimension
                            ligne.extend(li)
                config["lignes"].append(ligne)
            
            # pour les colonnes
            for _ in range(dim_lig):
                colonne = []
                for _ in range(random.randint(1, dim_col)):                 
                    lc = []
                    lc.append(random.randint(0, dim_col))
                    if not (lc[0] == 0 and len(colonne) > 0 ) and 0 not in colonne: # si la colonne contient déja 0
                        if (sum(lc) + sum(colonne) + len(lc) + len(colonne) - 1) <= dim_col: # si la valeur a ajouter fait cause un debordement de dimension
                            colonne.extend(lc)
                config["colonnes"].append(colonne)
                
        else: # saisie manuelle, Lire les lignes & colonnes
            
            # Lire les lignes
            nb_lig = dim_col
            conf = config["lignes"]
            aff = "ligne"
            dim = dim_lig

            for _ in range(2):
                for i in range(nb_lig):
                    liste = []
                    d = True
                    p = False
                    while d:
                        liste_aux= input(f"liste d'élément {aff} {i + 1} : ")
                        for val in liste_aux.split():
                            if val.isdigit():
                                liste.append(int(val))
                                d = False
                                p = True
                            else:
                                d = True     
                                p = False
                                print("\033[31mvaleur invalide : \033[0m", val)
                                liste = []
                                break
                        
                        if p:
                            if (len(liste)+sum(liste) - 1) <= dim:
                                conf.append(liste)
                            else:
                                print("\033[31mimpossible de configurer le jeu: la somme les éléments de la liste dépasse ", dim, " (dimenssion)\033[0m")
                                d = True
                                liste = []

                
                # Lire les colonnes
                nb_lig = dim_lig
                conf = config["colonnes"]
                aff = "colonne"
                dim = dim_col
            

    return config
#----------------------------------------------------------------------------#

def ecrire_clauses(file: str, l: FNC, nb_var: int, nb_clauses: int) -> None:
    """
    Ecrit l'ensemble (fnc) des clauses dans un fichier

    ARGUMENTS
    - file : nom de fichier où écrire les clauses
    - l : la liste de listes qui représente une fnc
    - nb_var : nombre de variables de la formule 
    - nb_clauses : nombre de clauses dans la formule
    """

    if nb_clauses == 0:
        err_exit(f'aucune clause à écrire dans \'{file}\'')

    # f = open(file, "w")
    f = ouvrir_fichier(file, "w")

    # première ligne du fichier : p cnf NOMBRE_VARIABLES NOMBRE_CLAUSES
    f.write(f'p cnf {nb_var} {nb_clauses}\n')

    #écriture des clauses
    for i in range(nb_clauses):
        for j in range(len(l[i])):
            f.write(f'{l[i][j]} ')
        f.write("0\n")

    f.close()

#----------------------------------------------------------------------------#

def modele_fichier(dimacs_file: str, solver: str) -> List[int]:
    """
    Renvoie un modèle si on utilise un sat solver externe

    ARGUMENTS
    - dimacs_file : fichier au format dimacs où est stocké le problème
    - solver : nom du sat solver pour la résolution (doit exister sur le système)
    """

    if not est_exec(solver):
         err_exit(f'solver \'{solver}\' inexistant ou inaccessible ou non exécutable')

    #équivalent à exécuter le sat solver en ligne de commande et en même temps ouvrir le résultat de l'exécution en tant que fichier
    sat_file = os.popen(f'{solver} {dimacs_file}', "r")

    modele: List[int] = []

    for ligne in sat_file:

        if ligne.startswith("s "):
            s = ligne.split(" ")[1].strip()
            if s == "UNSATISFIABLE":
                sat_file.close()
                return []
        # toute ligne qui commence par "v " contient une partie du modèle
        if ligne.startswith("v "):
            l = ligne.split(" ")
            #len(l) - 1 pour ne pas récupérer le 0 de fin
            for i in range(1, len(l) - 1):
                modele.append(int(l[i]))

    sat_file.close()
    return modele

