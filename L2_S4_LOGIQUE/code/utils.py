import os
import sys
import json
import shutil
import platform
# import gc
from pathlib import Path

from types_custom import *


argv = sys.argv
argc = len(argv)
RESET="\033[0;0m"
GREEN="\033[0;32m"
RED="\033[0;31m"
YELLOW="\033[0;93m"

# gc.enable()

def err_exit(err: str, type: str = "ERREUR", col: str = RED) -> None:
    """
    Termine l'exécution du programme en indiquant pour quelle erreur

    ARGUMENTS
    - err : l'erreur en question
    """
    print(f' --> {col}{type}{RESET} : {err}\n')
    exit(1)

#----------------------------------------------------------------------------#

def pth(ch: str) -> str:
    """
    Retourne un chemin cohérent par rapport à l'OS courant sous forme de string (plus simple) 
    """

    # usage de Path à cause de la différence entre les chemins de fichier Linux et Windows :(
    # conflit entre '/' et ('\\' ou '\')
    return str(Path(ch))

#----------------------------------------------------------------------------#

def basename(ch: str) -> str:
    """
    Renvoie le nom du fichier séparé de son chemin d'accès

    ARGUMENTS
    - ch : chemin absolu où se situe le fichier
    """

    # nom du fichier sans son chemin absolu ou relatif
    return str(Path(ch).name)

#----------------------------------------------------------------------------#

def name_dimacs(file: str) -> str:
    """
    Renvoie la partie significative du nom de fichier avec une extension ".dimacs"

    ARGUMENTS
    - file : nom du fichier auquel on va substituer son extension par ".dimacs" 
    """

    # split crée une liste d'éléments séparés par "."
    # ex : "fichier.txt" -> ["fichier", "txt"] on prend le premier élément auquel on concatène l'extension dimacs
    # -> fichier.dimacs
    return basename(file).split(".")[0] + ".dimacs"

#----------------------------------------------------------------------------#

def fichier_existe(file: str) -> bool:
    """
    Retourne vrai si le fichier existe, faux sinon

    ARGUMENTS
    - file : fichier dont on teste l'existence
    """
    
    file_pth: str = pth(file)
    return os.path.exists(file_pth) and os.path.isfile(file_pth)

#----------------------------------------------------------------------------#

def est_exec(file: str) -> bool:
    """
    Retourne vrai si le fichier est exécutable, faux sinon

    ARGUMENTS
    - file : fichier dont on veut connaître la possibilité de l'exécuter
    """

    file_pth: str = pth(file)
    # le fichier existe et est bien un fichier (pas un dossier) ou est dans le PATH (commande linux)
    return (fichier_existe(file_pth) and os.access(file_pth, os.X_OK)) or (shutil.which(file_pth) != None)

#----------------------------------------------------------------------------#

def ouvrir_fichier(file: str, mode: str):
    """
    Ouverture de fichier avec gestion d'erreurs

    ARGUMENTS
    - file : le fichier à ouvrir
    """

    if mode == "r" and not fichier_existe(file):
        err_exit(f'\'{pth(file)}\' inexistant')
    return open(pth(file), mode)

#----------------------------------------------------------------------------#

def json_load(file: str) -> dict:
    """
    Ouverture de fichier JSON avec gestion d'erreurs

    ARGUMENTS
    - file : fichier json à ouvrir
    """

    f = ouvrir_fichier(file, "r")
    try:
        d = json.load(f)
    except json.decoder.JSONDecodeError as err:
        err_exit(f'JSON : impossible de décoder \'{file}\' : {err}')
    f.close()
    return d


#----------------------------------------------------------------------------#

def get_cle(d: dict, cle: Any) -> Any:
    """
    Récupérer une clé d'un dictionnaire avec gestion d'erreurs

    ARGUMENTS
    - d : dictionnaire qui est censé contenir la clé
    - cle : cle supposément contenue dans le dictionnaire
    """

    try:
        valeur = d[cle]
    except KeyError:
        err_exit(f'cle \'{cle}\' absente du dictionnaire')
    return valeur
    
#----------------------------------------------------------------------------#

def print_fnc(l: FNC) -> None:
    """
    Affiche une formule sous Forme Normale Conjonctive avec ses connecteurs

    ARGUMENTS
    - l : formule à afficher
    """

    if l == []: err_exit(f'impossible d\'afficher la formule sous forme de fnc : formule mal formée')
    print("(", end="")

    # i = clause
    for i in range(len(l)):
        if l[i] == []: err_exit(f'impossible d\'afficher la formule sous forme de fnc : formule mal formée')
        print("(", end="")
        
        # j = k
        for j in range(len(l[i])):
            print(l[i][j], end="")

            if j < len(l[i]) - 1:
                print(" + ", end="")

        print(")", end="")
        
        if i < len(l) - 1:
            print(" . ", end="")

    print(")")

#----------------------------------------------------------------------------#

def print_jeu(l: JEU) -> None:
    """
    Affiche une formule de type JEU avec ses connecteurs au bon endroit

    ARGUMENTS
    - l : formule à afficher
    """

    if l == []: err_exit(f'impossible d\'afficher la formule sous forme de jeu : formule mal formée')
    print("(", end="")
    
    # i = ligne / colonne
    for i in range(len(l)):
        if l[i] == []: err_exit(f'impossible d\'afficher la formule sous forme de jeu : formule mal formée')
        print("(", end="")
        
        # j = possibilité d'une ligne / colonne
        for j in range(len(l[i])):
            if l[i][j] == []: err_exit(f'impossible d\'afficher la formule sous forme de jeu : formule mal formée')
            print("(", end="")

            # k = litteral
            for k in range(len(l[i][j])):
                print(l[i][j][k], end="")

                if k < len(l[i][j]) - 1:
                    print(" . ", end="")

            print(")", end="")

            if j < len(l[i]) - 1:
                print(" + ", end="")

        print(")", end="")

        if i < len(l) - 1:
            print(" . ", end="")

    print(")")

#----------------------------------------------------------------------------#

def chemin_solver(solver: str) -> str:
    """
    Renvoie le chemin d'un solver en fonction de l'OS

    ARGUMENTS
    - solver : solver dont on veut obtenir le chemin sur le système de fichiers
    """

    s: str
    p: str

    s = platform.system()
    if s == "Linux":
        p = "bin/linux/" + solver
    elif s == "Windows":
        p = "bin/windows/" + solver + ".exe"
    else:
        err_exit(f'pas de version du solver \'{solver}\' pour la platforme \'{s}\'')


    if not fichier_existe(pth(p)):
        err_exit(f'fichier \'{pth(p)}\' inexistant')

    return pth(p)
