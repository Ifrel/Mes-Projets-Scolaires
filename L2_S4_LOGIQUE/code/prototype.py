from types_custom import *
from fichier import lire_config


def anti_doublons(liste):
    """
    Renvoie une liste sans doublons
    ARGUMENTS
    - liste : liste à enlever les doublons
    """
    if liste == []:
        return []
    
    if isinstance(liste[0], list):
        liste_sans_doublons = list(set(tuple(subliste) for subliste in liste))
        liste_sans_doublons = [list(t) for t in liste_sans_doublons]
        return liste_sans_doublons

    if isinstance(liste, list):
        return list(set(liste))
    

# liste_de_listes = [[1, 2], [3, -4], [1, 2], [5, 6], [3, -4]]
# resultat = anti_doublons(liste_de_listes)
# print(resultat)

def trouver_plus_grand(liste:list)-> Tuple[int, list[int]]:
    """
    Renvoie l'element le plus grand et la liste des ses differentes position.s
    ARGUMENTS
    - liste : liste de recherche
    """

    if not liste:
        print("La liste est vide")
        exit(1)
    
    plus_grand = liste[0]  # Supposons que le premier élément est le plus grand
    position = 0  # La position initiale est 0
    liste_P = [position]
    
    for i in range(1, len(liste)):
        if liste[i] > plus_grand:
            plus_grand = liste[i]
            position = i
            liste_P = [position]
        elif liste[i] == plus_grand:
            liste_P.append(i)
    
    return plus_grand, liste_P


def integrite(contrainte: list, var_liste: list) -> bool:
    """
    Renvoie vrai si les variables dans var_liste verifies les contraintes dans contrainte
    ARGUMENTS
    - contrainte : liste contenant les contraintes
    - var_liste : liste contenant les variables
    """

    # Si la contrainte n'est pas nulle
    if contrainte == []: return False

    # si la premiere contraine n'est pas nulle
    if contrainte[0] == 0:
        for i in var_liste:
            if i > 0: return False

    n = 0
    while n < len(var_liste) and var_liste[n] < 0: # on passe si une variable est négative au début
        n += 1
    
    for i, c in enumerate(contrainte):
        for _ in range(c): # on parours la valeur de la contrainte
            if n < len(var_liste) and var_liste[n] < 0:
                return False
            n += 1

        # on veififie qu'ila ya aumoins le un separateur de contrainte
        if n < len(var_liste) and var_liste[n] > 0:
            return False
        
        # on passe si une variable est négative au début
        while n < len(var_liste) and var_liste[n] < 0: 
            n += 1
        
        # if len(var_liste) == n and len(contrainte) != i+1:
        #     return False
        
    # on verifie les dimensions
    if len(var_liste) != n: return False

    return True
                
            
def liste_varibles(dim: list, num:int, colonne:bool = False) -> list:
    """
    Renvoie une liste de toutes les variables possibles pour une dimension

    ARGUMENTS
    - dim : liste contenant les dimensions des lignes et colonnes
    - num : numero de la ligne ou de la colonne
    - col : booléen indiquant si la variable est une colonne (True) ou une ligne (False)
    """

    liste = []
    if colonne:
        for _ in range( dim[1]):
            liste.append(num)
            num+= dim[0]
    else:
        val = (num -1)*dim[0] +1
        for _ in range(dim[0]):
            liste.append(val)
            val += 1
    return liste





# def variables_obligatoires(contraintes:list[list], liste_variables:list[list], dimenssion:list) -> list:
    """
    Renvoie une liste de liste de toutes les variables obligatoires pour une contrainte [contraintes_sur_les_lignes, contraintes_sur_les_colonnes]

    ARGUMENTS
    - contraintes : liste contenant les contraintes lignes & colonnes
    - liste_variables : liste contenant les variables lignes & colonnes
    - dimenssion : liste contenant les dimensions des lignes et colonnes
    """

    # Stockage des résultats
    resultat_total = []

    # Séparation des données
    contrainte_lignes = contraintes[0]
    contrainte_colonnes = contraintes[1]
    var_liste_lignes = liste_variables[0]
    var_liste_colonnes = liste_variables[1]
    dim_lignes = dimenssion[0]
    dim_colonnes = dimenssion[1]

    # PHASE 1: RECHERCHE DES VARIABLES OBLIGATOIRES
    dim = dim_lignes
    liste_contrainte = contrainte_lignes
    liste = var_liste_lignes

    for _ in range(2): # on parcours les contraintes anisi que les variables corresspondent (lignes ou colonnes)
        resultat_sous_total = [] # resultat toutes les lignes ou colonnes
        for l, contrainte in enumerate(liste_contrainte):
            resultat = [] # resultat sur un ligne ou colonne
            var_liste = liste[l]

            # 1 cas: la contrainte contient un seul element
            if len(contrainte) == 1 and contrainte[0] == 0:
                # if contrainte[0] > dim//2:
                #     val = contrainte[0] # valeur de la contraines
                #     decalage = dim - val
                #     for i in range(decalage, val):
                #         resultat.append(var_liste[i])
                
                if contrainte[0] == 0:
                    resultat.extend([- i for i in var_liste])

            # 2 cas: la contrainte contient deux elements
            # elif len(contrainte) == 2 and  (sum(contrainte) +1 > dim//2) and (contrainte[0] >= dim//2 or contrainte[1] >= dim//2):
                # val1 = contrainte[0]
                # val2 = contrainte[1]
                # dist = sum(contrainte) + 1
                # decalage = dim - dist

                # if val1 > val2 and decalage != 0:
                #     for i in range(decalage, val1):
                #         resultat.append(var_liste[i])

                # elif val1 < val2 and decalage != 0:
                #     n = dim - val2
                #     n2 = dim - decalage
                #     for i in range(n, n2):
                #         resultat.append(var_liste[i])

                # else:
                    # resultat = var_liste.copy()
                    # resultat[val1] = - resultat[val1]
            
            # 3 cas: la contrainte contient trois elements
            elif (sum(contrainte) + len(contrainte) -1) == dim:
                n = 0
                for i in contrainte:
                    for j in range(i):
                        resultat.append(var_liste[n])
                        n += 1
                    if n < dim:
                        resultat.append(-var_liste[n])
                        n += 1

            else:
                if sum(contrainte) > dim//2:
                    maxe, pos = trouver_plus_grand(contrainte)
                    decal = dim - (sum(contrainte) + len(contrainte) -1)
                    if decal < maxe:
                        iteraration = maxe - decal
                        pos_var = 2*pos + decal
                        for i in range(iteraration):
                            resultat.append(var_liste[pos_var + i])
                    

            
            resultat_sous_total.append(resultat)
        resultat_total.append(resultat_sous_total)

        # Changement de donneées pour passer aux colonnes
        dim = dim_colonnes
        liste_contrainte = contrainte_colonnes
        liste = var_liste_colonnes

       
    # PHASE 2: COMPLEMENT DES VARIABLES OBLIGATOIRES PAR RAPPORT AUX LIGNES & COLONNES
    for i, lin in enumerate(resultat_total[0]):
        for j, col in enumerate(resultat_total[1]):
            e = var_liste_colonnes[j][i]
            if e in col and e not in lin:
                lin.append(e)
            elif -e in col and -e not in lin:
                lin.append(-e)
            lin.sort()

    for i, col in enumerate(resultat_total[1]):
        for j, lin in enumerate(resultat_total[0]):
            e = var_liste_lignes[j][i]
            if e in lin  and e not in col:
                col.append(e)
            elif -e in lin and -e not in col:
                col.append(-e)
            col.sort()

    return resultat_total


def variables_obligatoires(contraintes:list[list], liste_variables:list[list], dimenssion:list) -> list:
    """
    Renvoie une liste de liste de toutes les variables obligatoires pour une contrainte [lignes, colonnes]

    ARGUMENTS
    - contraintes : liste contenant les contraintes lignes & colonnes
    - liste_variables : liste contenant les variables lignes & colonnes
    - dimenssion : liste contenant les dimensions des lignes et colonnes
    """

    # Stockage des résultats
    resultat_total = []

    # Séparation des données
    contrainte_lignes = contraintes[0]
    contrainte_colonnes = contraintes[1]
    var_liste_lignes = liste_variables[0]
    var_liste_colonnes = liste_variables[1]
    dim_lignes = dimenssion[0]
    dim_colonnes = dimenssion[1]

    # PHASE 1: RECHERCHE DES VARIABLES OBLIGATOIRES
    ligne = True
    for _ in range(2):
        dim = dim_lignes if ligne else dim_colonnes
        liste_contrainte = contrainte_lignes if ligne else contrainte_colonnes
        liste = var_liste_lignes if ligne else var_liste_colonnes

        # on parcours les contraintes anisi que les variables corresspondent (lignes ou colonnes)
        resultat_sous_total = [] # resultat toutes les lignes ou colonnes
        for num, contrainte in enumerate(liste_contrainte):
            resultat = [] # resultat sur un ligne ou colonne
            var_liste = liste[num]

            # 1er cas: la contrainte 0
            if len(contrainte) == 1 and contrainte[0] == 0:
                resultat.extend([- i for i in var_liste])
            
            # 2ème cas: la contrainte complete bien  la dimension
            elif (sum(contrainte) + len(contrainte) -1) == dim:
                n = 0
                for i in contrainte:
                    for j in range(i):
                        resultat.append(var_liste[n])
                        n += 1
                    if n < dim:
                        resultat.append(-var_liste[n])
                        n += 1

            else: # 3ème cas: la contrainte contient aumoins 2 (ne verifiant pas le 2eme cas)
                if sum(contrainte) > dim//2:
                    maxe, liste_position = trouver_plus_grand(contrainte) # la contrainte la plus grande
                    decal = dim - (sum(contrainte) + len(contrainte) -1) # calcul du décalage (nbre de variables restante une toutes les contraine utlise)
                    if decal < maxe:
                        iteraration = maxe - decal # calcul du nbre de variables obligatoires
                        for position in liste_position: # Pour chaque position de la plus grande contrainte                    
                            position_var = sum(contrainte[:position]) + len(contrainte[:position]) + decal # calcul de la position des variables a prendre
                            for i in range(iteraration):
                                resultat.append(var_liste[position_var + i])

            resultat_sous_total.append(resultat)
        resultat_total.append(resultat_sous_total)

        # Changement de donneées pour passer aux colonnes
        ligne = False

       
    # PHASE 2: COMPLEMENT DES VARIABLES OBLIGATOIRES PAR RAPPORT AUX LIGNES & COLONNES
    long1 = len(resultat_total[0])
    long2 = len(resultat_total[1])

    for i in range(long1):
        for j in range(long2):
            lin = resultat_total[0][i]
            col = resultat_total[1][j]
            e = var_liste_colonnes[j][i]
            if e in col and e not in lin:
                lin.append(e)
            elif -e in col and -e not in lin:
                lin.append(-e)
            lin.sort()

    for i in range(long2):
        for j in range(long1):
            lin = resultat_total[0][j]
            col = resultat_total[1][i]
            e = var_liste_lignes[j][i]
            if e in lin  and e not in col:
                col.append(e)
            elif -e in lin and -e not in col:
                col.append(-e)
            col.sort()

    return resultat_total




l = [
        [5],
        [1, 1],
        [2, 1],
        [1, 3],
        [0],
        [4],
        [1],
        [4],
        [0],
        [1, 3],
        [1, 1, 1],
        [3, 1],
        [0]
    ]

l2 = [
        [1, 1, 1, 1, 3],
        [1, 1, 1, 1, 1],
        [4, 3 ,3],
        [1, 1, 1, 1, 1],
        [4, 3]
     ]


dim = [5,13]
v1=[]
v2=[]
for i, c in enumerate(l):
    var_liste = liste_varibles(dim, i+1)
    v1.append(var_liste)

for i, c in enumerate(l2):
    var_liste = liste_varibles(dim, i+1, True)
    v2.append(var_liste)


# res = variables_obligatoires([l,l2], [v1,v2], dim)





def poss_avec_1_var(contrainte: list, var_liste: list) -> list[list[int]]:
    """
    Renvoie une liste de toutes les possibilités pour une valeur dans contrainte

    ARGUMENTS
    - contraintes : liste contenant les contraintes
    """

    resultat = []

    # Si la contrainte est nulle, il suffit d'inverser le signe de tous les éléments de var_liste
    if contrainte[0] == 0:
        resultat.append([-v for v in var_liste])
        return resultat
    
    # Si la contrainte n'est pas nulle
    for k in range(len(var_liste)):
        poss = []
        n = k
        # Vérifie que la contrainte peut être satisfaite avec les éléments restants dans var_liste
        if (n + contrainte[0]) <= len(var_liste):
            # Ajoute les éléments négatifs avant la séquence de la contrainte
            for i in range(n):
                poss.append(-var_liste[i])

            # Ajoute la séquence de la contrainte
            for _ in range(contrainte[0]):
                poss.append(var_liste[n])
                n += 1

            # Ajoute les éléments négatifs après la séquence de la contrainte
            for _ in range(len(var_liste) - contrainte[0] - k):
                poss.append(-var_liste[n])
                n += 1

            resultat.append(poss)

    return resultat
 

def possibilites(contrainte: list, variables: list) -> list[list]:    
    """
    Renvoie une liste de toutes les possibilités pour une contrainte

    ARGUMENTS
    - contraintes : liste contenant les contraintes
    - variables : liste contenant les variables

    RETOUR
    - liste de toutes les possibilités par rapport a la contrainte une contrainte
    """
    
    resultat = []

    # Si la contrainte n'a qu'un seul element
    if len(contrainte) == 1:
        resultat.extend(poss_avec_1_var(contrainte, variables))
        return resultat
    
    # sinon
    c1 = contrainte[0]
    c2 = 1 # Separateur de contraintes
    s_contrs = contrainte[1:]
    s_vars = variables[(c1 + c2):]

    res = possibilites(s_contrs, s_vars)
    res_2 = poss_avec_1_var([c1], variables[:(c1 + c2)])

    
    # Ajoute toutes les possibilités de la contrainte
    for j in res:
        temp = res_2[0].copy()
        temp.extend(j)
        if temp not in res and integrite(contrainte, temp):
            resultat.append(temp)


    decalage = len(variables) - (sum(contrainte) + len(contrainte) - 1)
    for i in range(1, decalage + 1):
        res = possibilites(contrainte, variables[i:])
        temp = [-variables[j] for j in range(i)]
        
        # Ajoute toutes les possibilités de la contrainte
        for k in res:
            temp2 = temp.copy()
            temp2.extend(k)
            if temp2 not in resultat and integrite(contrainte, temp2):
                resultat.append(temp2)
                    

    return resultat


def creer_jeu(fichier: str) -> Tuple[dict, JEU, int]:
    # Récupère la configuration du jeu
    config = lire_config(fichier)

    resultat = []
    dim = config["dim"]
    lignes = config["lignes"]
    colonnes = config["colonnes"]

    # Création des variables
    vars_lignes = []
    vars_colonnes = []
    
    for num in range(1, dim[1] + 1):
        var_liste = liste_varibles(dim, num)
        vars_lignes.append(var_liste)

    for num in range(1, dim[0] + 1):
        var_liste = liste_varibles(dim, num, True)
        vars_colonnes.append(var_liste)

    # Création des variables Obligatoies 
    vars_obligatoires = variables_obligatoires([lignes, colonnes], [vars_lignes, vars_colonnes], dim)
    
    v=0
    for i in vars_obligatoires:
        n=1
        print()
        for j in i:
            print(2*v*"\t",j, " -> ",n)
            n += 1
        v += 1


    # RECHERCHE DES POSSIBILITES POUR CHAQUE CONTRAINTE
    ligne = True
    for _ in range(2):
        dimenssion = dim[1] if ligne else dim[0]
        for i in range(dimenssion):
            contrainte = lignes[i] if ligne else colonnes[i]
            var_liste = vars_lignes[i] if ligne else vars_colonnes[i]
            vars_obl = vars_obligatoires[0][i] if ligne else vars_obligatoires[1][i]
            res = possibilites(contrainte, var_liste)
            
            # Réduction des possibilités
            res_reduites = []
            for p in res:
                if all(j in p for j in vars_obl):
                    res_reduites.append(p)
            if res_reduites == []:
                res_reduites.append(p)

            resultat.append(res_reduites)
        ligne = False # passage aux col
    
    return config, resultat, dim[0]*dim[1]




# conf, res, nb_var = creer_jeu("tests/jeu_4.json")

# for i in res:
#     n=1
#     print()
#     for j in i:
#         print("\t",j, " -> ",n)
#         n += 1