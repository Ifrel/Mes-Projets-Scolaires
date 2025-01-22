
from types_custom import *
from utils import *

#############
import time
#############


def distrib_aux(f: FILE, l: List[List[int]], var: dict, prof: int, res: List, inter: List) -> None:
    """
    Fonction auxiliaire

    Distribue récursivement une ligne / colonne 

    ARGUMENTS
    - l : ligne / colonne avec les sous-listes à distribuer
    - prof : indice / profondeur pour parourir les sous-listes
    - res : liste qu'on remlis avec chaque étape de la distribution
    - inter : liste temp qui représente une étape de la distribution
    """
    # dans cette fonction on utilise les listes 'res' et 'inter' 
    # (et donc leur adresse -> donc pas locales)
    # pour avoir accès à leur valeur quelle que soit l'instance de la fonction qui les utilise
    # 'res' est le résultat final de l'exécution pour une seule ligne / colonne
    # 'inter' correspond à une étape de la distribution
    # si la profondeur / l'indice == la longueur de l
    # on atteint la dernière sous liste, autrement dit ce qui correspond aux feuilles de l'arbre
    # des possibilités
    if prof + 1 == len(l):
        # on a donc pour chaque élément de la dernière liste à générer chaque étape correspondante
        # (a, b, ... feuilles[i]), (a, b, ... feuilles[i + 1]) (a, b, ... feuilles[i + 2]) .....
        for i in range(len(l[prof])):
            v=False
            if l[prof][i] not in var:
                v=True
                var[l[prof][i]] = 0
                inter.append(l[prof][i])
            # on copie la liste avec '[*liste]' pour ne pas perdre les valeurs
            # car append ne fait que pointer vers la liste
            # res.append(inter.copy())
            # res.append([*inter])
            for j in range(len(inter)):
                f.write(f'{inter[j]} ')
            f.write("0\n")
            # on retire le dernier élément pour pouvoir acceuillir le dernier élément suivant (garder même taille)
            if v:
                del var[inter[-1]]
                inter.pop()
        return
    
    while l != [] and l[prof] == []:
        prof += 1
        if prof + 1 == len(l):
            if l[prof] == []:
                # res.append([*inter])
                for i in range(len(inter)):
                    f.write(f'{inter[i]} ')
                f.write("0\n")
                return

            for i in range(len(l[prof])):
                inter.append(l[prof][i])
                # res.append([*inter])
                for j in range(len(inter)):
                    f.write(f'{inter[j]} ')
                f.write("0\n")
                inter.pop()
            return

    # pour chaque élément d'une sous-liste, ajouter un littéral dans ce qui correspond à une étape de la distribution
    # (revient à parourir un arbre à len(l[prof]) fils )
    if l == []: return
    for i in range(len(l[prof])):
        v=False
        if l[prof][i] not in var:
            v=True
            var[l[prof][i]] = 0
            inter.append(l[prof][i])

    
        distrib_aux(f, l, var, prof+1, res, inter)
        # supprimer le dernier élément de la liste quand on dépile
        # (revient à supprimer une possibilité qui a été écoulée pour la remplacer plus tard par la suivante, tout ça en gardant le même taille (car taille d'une étape = nb de sous-listes))
        if v:
            del var[inter[-1]]
            inter.pop()

#----------------------------------------------------------------------------#

def distrib(f: FILE, l: List[List[int]]) -> FNC:
    """
    Renvoie une fnc (temporaire) à partir de la distribution d'une ligne / colonne
    
    ARGUMENTS
    - l : ligne / colonne à distribuer (liste de listes)
    """
    
    # profondeur qui sert aussi d'indice 
    # (on peut considérer que le i-ième niveau de profondeur correspond à la i-ième sous liste de l)
    prof: int = 0

    res: FNC = FNC([])
    # liste résulat intermédiaire
    inter: List = []

    var: dict = {}
    
    # si dans la ligne / colonne, il n'y a qu'une seule possibilité,
    # on peut ajouter toutes les variables à la liste dans leur clause séparée
    if len(l) == 1:
        for i in range(len(l[0])):
            # res.append([l[0][i]])
            f.write(f'{l[0][i]} 0\n')
        # return res
        return
    
    # autrement il faut faire la distribution de la ligne / colonne de manière récursive
    distrib_aux(f, l, var, prof, res, inter)

    return res

#----------------------------------------------------------------------------#

def nb_clauses_attendu(l: JEU) -> int:
    nb: int = 0
    for i in range(len(l)):
        nb1: int = 1
        if len(l[i]) == 0: nb1 = 0
        for j in range(len(l[i])):
            nb1 *= len(l[i][j])
        nb += nb1
    return nb

#----------------------------------------------------------------------------#

def avoir_fnc(config: dict, fichier: str, l: JEU) -> Tuple[FNC, int]:
    """
    Renvoie une Forme Normale Conjonctive à partir d'une formule 'JEU'

    ARGUMENTS
    - l : liste qui représente une formule de type JEU
    RETOUR
    - fnc : la forme normale conjonctive qui résulte de la transformation
    - nb_clauses : le nombre de clauses dans cette fnc
    """

    n: int = nb_clauses_attendu(l)
    nb_clauses_max: int = 90_000_000
    if n > nb_clauses_max:
        err_exit(f'la formule va produire environ\n{n:_} clauses\nalors que {nb_clauses_max:_} est le max', "/!\ ATTENTION", YELLOW)

    f = ouvrir_fichier(fichier, "w")
    dim = get_cle(config, "dim")
    f.write(f'p cnf {dim[0]*dim[1]} {n}\n')

    fnc: FNC = FNC([])
    nb_clauses: int = 0
    # pour chaque ligne / colonne dans l
    for i in range(len(l)):
        # on distribue la ligne / colonne
        distrib(f, l[i])
        # la distribution étant une liste de listes, on ajoute une à une les clauses
        # à la liste finale
        # for clause in dis:
        #     # il y a donc une clause en plus à chaque itération
        #     # (valeur finale utile pour le sat-solver)
        #     nb_clauses +=1
        #     fnc.append(clause)
    f.close()

    return n


# A SUPPRIMER :


# wow = 3 
# test = [[[1 , 2 , 3 , -4] , [-1 , 2 , 3 , 4]], [[5 , 6 , 7 , 8]], [[9 , 10 , 11 , 12]], [[13 , -14 , 15 , -16] , [13 , -14 , -15 , 16] , [-13 , 14 , -15 , 16]], [[1 , 5 , 9 , 13]], [[2 , 6 , 10 , -14] , [-2 , 6 , 10 , 14]], [[3 , 7 , 11 , -15] , [-3 , 7 , 11 , 15]], [[4 , 8 , 12 , -16] , [-4 , 8 , 12 , 16]]]
#
# print("formule de base : ")
# # print(test[wow])
# print(test)
# print("--------")
# # r = distrib(test[wow])
# t1 = time.time()
# confifg_test = {"dim": [4, 4]}
# r, _ = avoir_fnc(confifg_test, "delete.dimacs", test)
# t2 = time.time()
# print(r)
# print(f"TIME :  {t2 - t1}")
#
#
# test2 = [[9,8], [1,2,3], ["a", "b", "c", "d"], ["#"], [], [], [], [11, 15], [], [5]]
# # test3 = [[5], [], []]
# # test4 = [[], [], [4]]
# # test5 = [[4], [3], [2], [7]]
# # test6 = [[5,2], [], [-2, 5, 3], [], [], [10,2], [3]]
# test7 = [[4, 6], [22], ["abcv", 1245.5], [0, 0.25], ["rms"], [74, 22, 18]]
# #
# ic("formule de base : ")
# ic(test2)
# ic("-------------")
# f = distrib(test2)
# #
# ic(f)
# ic("nb_clauses : ", len(f))
# #
