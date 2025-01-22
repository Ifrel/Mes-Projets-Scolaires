
config1: dict = {
   "dim": [6, 6],
    "lignes": [
        [2,1,1],
        [1,3],
        [1,2],
        [3],
        [4],
        [1]
    ],
    "colonnes": [
        [1],
        [5],
        [2],
        [5],
        [2,1],
        [2]
    ]
}

config2: dict = {
   "dim": [4, 4],
    "lignes": [
        [3],
        [4],
        [4],
        [1,1]
    ],
    "colonnes": [
        [4],
        [3],
        [3],
        [3]
    ]
}

config3: dict = {
   "dim": [20, 20],
    "lignes": [
        [2],
        [2,1,8],
        [3,11],
        [1,4,2,4],
        [1,7,1],
        [1,1,2,2,1,1],
        [1,8,2],
        [1,4,4,1],
        [6,2,2,3],
        [11,1],
        [4,3,2,3],
        [2,3,3,1,1],
        [8,1,5,1],
        [6,3,1,2],
        [3,4,1,1],
        [5,4,1],
        [5,1,1],
        [2,2,1,1],
        [4,4,1],
        [5,5,1],
    ],
    "colonnes": [
        [1,3],
        [2,1,2,5],
        [1,1,8,2],
        [2,3,8],
        [1,3,2,5],
        [1,3,3,2,2],
        [8,6,1],
        [3,2,4,1,2],
        [4,2,2,3,3],
        [2,3,4,3,2],
        [2,6,3,5],
        [4,2,3,1,2],
        [10,1,1],
        [2,4,2,1],
        [3,2],
        [3,1,2,1],
        [3,1,1,1],
        [1,2,1,1,1],
        [2,1,7],
        [3],
    ]
}

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

    if liste and liste[0] != 0:
        resultat = []
        var_c = var  # Clonage de la variable pour la manipuler sans altérer l'originale
        
        for i in range(0, taille):
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
        resultat = []  # Si la liste est vide ou que son premier élément est nul, retourne une liste vide
        
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


def creer_instance(config: dict):
    """
    Crée une instance de jeu à partir d'une configuration donnée.

    Arguments :
    - config : Un dictionnaire contenant les informations de configuration du jeu.

    Retourne :
    Un tuple contenant la configuration du jeu, la formule générée et le nombre total de variables.
    """

    ligne_max = config["dim"][0]  # Nombre maximum de lignes
    colonne_max = config["dim"][1]  # Nombre maximum de colonnes
    LIGNE_COLONNE = []
    LIGNE_COLONNE.append(config["lignes"])  # Ajout de la Liste des contraintes de lignes
    LIGNE_COLONNE.append(config["colonnes"]) # Ajout Liste des contraintes de colonnes

    formule = []  # Liste pour stocker la formule générée
    monomes = []  # Liste temporaire pour stocker les monomes générées
    clauses = []  # liste contenant les possibilités sur une ligne ou une colonne
    all_clauses = [] # Liste contenant les possibilités de toute les lignes ou colonne

    liste_en_cours = []  # Liste temporaire pour stocker les éléments de la ligne en cours de construction
    taille = ligne_max  # Taille maximale de la ligne
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

m = 0
c,f,n = creer_instance(config1)
print("bbbbbbbbbb: ",len(f))
l = c["lignes"] 

print("all_lignes: [")
n = 1
for bloc in f:
    if n == c["dim"][0] + 1:
        l=c["colonnes"]
        m = 0
    print("lignes ", n ,l[m],":[ ")
    n2 = 1
    for poss in bloc:
        print("pos ", n2," :", end=" ")
        print(poss)
        n2 = n2 + 1
    print("]\n\n")
    
    
    
    n = n + 1
    m = m  + 1
        
print("]")

