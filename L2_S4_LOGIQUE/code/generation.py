
from types_custom import *
from utils import *
from fichier import lire_config


# def possibilite_aux(var: List, tab: List, res: List, inter: List):
#     return [[0] * len(var)]


# #----------------------------------------------------------------------------#

# def possibilite(dim: List, tab: List, n: int, d: bool):
        
#     var: List = []
#     res: List = []
#     inter: List = []
    
#     d_max = dim[1] if d else dim[0]
#     for i in range(1, d_max+1):
#         if d:
#             var.append((i * d_max) + n - d_max)
#         else:
#             var.append(i + (n-1) * d_max)


#     # print(var)
#     #exit(0)


#     if len(tab) == 1:

#         if tab[0] == d_max:
#             return [var]

#         i = 0
#         while True:
#             # if i + tab[0] > len(var):
#             #     break
            
#             j = 0
#             while j < d_max:
#                 if j == i:

#                     k = j
#                     while k < tab[0]+i:
#                         inter.append(var[k])
#                         k += 1
#                     j += tab[0]+i-(i+1)
#                     if j >= len(var):
#                         break
#                 else:
#                     inter.append(-var[j])
#                 j += 1

#             i += 1

#             res.append(inter.copy())

#             if inter[-1] > 0:
#                 break
#             inter = []

#         return res

#     return possibilite_aux(var, tab, res, inter)





# #----------------------------------------------------------------------------#

# def creer_instance1(file: str) -> Tuple[dict, JEU, int]:
#     """
#     Renvoie une instance du jeu

#     ARGUMENTS
#     - file : nom de fichier qui contient la config, stdin si file == ""
#     """

#     config: dict = lire_config(file)

#     formule: JEU = JEU([])

#     dim = get_cle(config, "dim")
#     l_max = dim[0]
#     c_max = dim[1]

#     nb_var = l_max * c_max

#     lig = get_cle(config, "lignes")
#     col = get_cle(config, "colonnes")
    
#     # lignes
#     for i in range(c_max):
#         li = possibilite(dim, lig[i], i+1, False)
#         formule.append(li)

#     # colonnes
#     for i in range(l_max):
#         co = possibilite(dim, col[i], i+1, True)
#         formule.append(co)

#     # for i in range(len()):
#     #     pass
#     #COMPLETER
    
#     #retourner config jeu pour affichage
#     return config, formule, nb_var


def p_avec_1_var(liste: list, taille: int, var: int, incrementation: int) -> list[list[int]]:
    """
    Génère des combinaisons de taille fixe en insérant les éléments d'une liste fournie, 
    tout en maintenant une seule variable de référence pour remplir les espaces vides.

    Arguments :
    - liste : La liste d'entiers à utiliser pour générer les combinaisons.
    - taille : La taille des combinaisons à générer.
    - var : La valeur initiale de la variable utilisée pour remplir les espaces vides.

    Retourne :
    Une liste de combinaisons possibles, chaque combinaison étant une liste d'entiers.
    """

    var_c = var # Clonage de la variable pour la manipuler sans altérer l'originale
    resultat = []

    if liste and liste[0] != 0:

        for i in range(taille):
            possibilite = []  # Liste contenant une possibilité
            pas = 1  # Pas (distance) d'itération
            for j in range(0, taille, pas):
                if j == i:
                    for _ in range(liste[0]):
                        possibilite.append(var_c)
                        var_c += incrementation
                        pas += 1
                elif len(possibilite) < taille:
                    possibilite.append(-var_c)
                    var_c += incrementation
                    pas += 1
                
            if len(possibilite) == taille:
                    resultat.append(possibilite)
            var_c = var  # Réinitialisation de la variable pour la prochaine itération
    else:
        possibilite = []
        for i in range(taille):
            possibilite.append(-var_c)
            var_c += incrementation
        resultat.append(possibilite)

    print(resultat)
    return resultat

def rec_creer_instance(liste: list, liste_en_cours: list, taille: int, int_debut: int, total: list, incrementation: int) -> None:
    """
    Crée de manière récursive des instances en insérant des éléments d'une liste donnée, 
    en utilisant des séparateurs, pour remplir une taille spécifiée.

    Arguments :
    - liste : La liste des entiers à utiliser pour générer les instances.
    - liste_en_cours : La liste en cours de construction.
    - taille : La taille totale de l'instance à générer.
    - int_debut : La valeur initiale à utiliser pour remplir les espaces de l'instance.
    - total : La liste qui stocke toutes les instances générées.
    - incrementation : La valeur incrementée

    Retourne :
    Aucune valeur de retour explicite, mais la fonction modifie la liste "total" pour stocker les instances générées.
    """

    # Initialisation des variables
    var = int_debut
    taille_disponible = taille

    # Si la liste ne contient qu'un seul élément
    if len(liste) == 1:
        # Générer les possibilités avec un seul élément
        possibilite = p_avec_1_var(liste, taille, var, incrementation)
        for i in possibilite:
            liste_temp = liste_en_cours.copy()
            liste_temp.extend(i)
            total.append(liste_temp)
              
        return 

    # Premier élément de la liste
    element = liste[0]

    # Ajout de l'élément autant de fois que sa valeur
    for _ in range(element):
        liste_en_cours.append(var)
        var += incrementation
        taille_disponible -= 1
        
    # Si plus de place disponible, on retourne la possibilité
    if taille_disponible == 0:
        return 

    # Ajout d'un séparateur
    liste_en_cours.append(-var)
    taille_disponible -= 1

    # Si plus de place disponible, on retourne la possibilité
    if taille_disponible == 0:
        return 
    
    # Incrémentation de la variable courante
    var += incrementation

    # Liste restante
    liste_restante = liste[1:]

    # Appel récursif sur le reste de la liste
    rec_creer_instance(liste_restante, liste_en_cours, taille_disponible, var, total, incrementation)


def creer_jeu(fichier: str) -> Tuple[dict, JEU, int]:
    """
    Crée une instance de jeu à partir d'une configuration donnée.

    Arguments :
    - config : Un dictionnaire contenant les informations de configuration du jeu.

    Retourne :
    Un tuple contenant la configuration du jeu, la formule générée et le nombre total de variables.
    """

    config = lire_config(fichier)

    print("------------------------------")

    ligne_max = config["dim"][1]  # Nombre maximum de lignes
    colonne_max = config["dim"][0]  # Nombre maximum de colonnes
    LIGNE_COLONNE = []
    LIGNE_COLONNE.append(config["lignes"])  # Ajout de la Liste des contraintes de lignes
    LIGNE_COLONNE.append(config["colonnes"]) # Ajout Liste des contraintes de colonnes

    formule = []  # Liste pour stocker la formule générée
    monomes = []  # Liste temporaire pour stocker les monomes générées
    clauses = []  # liste contenant les possibilités sur une ligne ou une colonne
    all_clauses = [] # Liste contenant les possibilités de toute les lignes ou colonne

    liste_en_cours = []  # Liste temporaire pour stocker les éléments de la ligne en cours de construction
    taille = ligne_max  # Taille maximale de la ligne
    # taille = colonne_max
    var_courante = 1  # Variable de début pour la construction de la ligne
    var_suivante = 1  # Variable de début pour la construction de la ligne suivante
    incrementation = 1 # Increment
    var_courante_colonne = 1 # Increment pour les liste en colonne
    

    # Parcours des contraintes de lignes et les colonnes
    drapeau = False
    for bloc_contraite in LIGNE_COLONNE:
        
        # Parcours de ssous bloc de contraines
        for line in bloc_contraite:
            # Choix du varible en fonction de la ligne ou de la colonne
            var_courante = var_courante_colonne if drapeau else var_courante
            var_suivante = var_courante_colonne if drapeau else var_courante

            long = 1 if len(line) == 1 else len(line) + 1  # Longueur de la contrainte de ligne
           
           # Création de toutes les possibilités
            for i in range(long):
                if i == 0:
                    # Création de la première possibilité de ligne sans séparateurs
                    rec_creer_instance(line, liste_en_cours, taille, var_courante, monomes, incrementation)
                    clauses.extend(monomes)
                
                else:
                    # Création de possibilités de lignes avec séparateurs
                    liste_en_cours = []
                    monomes = []
                    for _ in range(i):
                        liste_en_cours.append(-var_courante)
                        var_courante += incrementation
                        taille -= 1
                    
                    # Création des autes possibilités de ligne avec séparateurs
                    rec_creer_instance(line, liste_en_cours, taille, var_courante, monomes, incrementation)
                    clauses.extend(monomes)
                    
                    var_courante = var_suivante  # Réinitialisation de la variable de début
                    taille += i  # Réajustement de la taille de la ligne
                
                liste_en_cours = []  # Réinitialisation de la liste temporaire pour la ligne
                monomes = []  # Réinitialisation de la liste temporaire pour la formule
            
            var_suivante += ligne_max  # Mise à jour de la variable de début pour la prochaine ligne
            var_courante = var_suivante  # Mise à jour de la variable de début pour la prochaine ligne

            # Ajout de toutes les possibilité d'une ligne ou colonne
            all_clauses.append(clauses)
            clauses = []
            var_courante_colonne += 1
        
        # Passage au colones
        var_courante = 1
        var_suivante = 1
        var_courante_colonne = 1
        incrementation = ligne_max
        drapeau = True
        
    formule.extend(all_clauses)
    
        
    # Retour de la configuration du jeu, de la formule générée et du nombre total de variables
    return config, formule, ligne_max*colonne_max

# l1 = [2,1]
# l2 = [6]
# l3 = [3]

# dim = [6,8]
# #possibilite(dim, l2, 2, False)
# #print(possibilite(dim, l2, 2, False))
# r = possibilite(dim, l2, 3, True)
# print(r)
