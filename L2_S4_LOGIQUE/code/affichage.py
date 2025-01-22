import turtle

from types_custom import *
from utils import *


def carre(taille, nombre=None, couleur="white"):
    """
    Dessine un carré avec une couleur de remplissage optionnelle et un nombre facultatif au centre.
    
    Arguments :
    taille -- longueur du côté du carré
    couleur -- couleur de remplissage du carré (par défaut : blanc)
    nombre -- nombre à afficher au centre du carré (par défaut : None)
    """
    
    # Définir la couleur du stylo
    # turtle.pencolor("black")
    turtle.pencolor("gray")
    
    # Gestion des variables d'entrée "couleur" & "nombre"
    if nombre == '#' :
        nombre = None
        
    elif nombre == 0 and isinstance(nombre, int):
        nombre = None
        turtle.pencolor("white")
        
    elif nombre < 0:
       # couleur = "gray"
        couleur = "white"
        nombre = None
        
    elif nombre >= 0:
        if isinstance(nombre, float):
            nombre = int(nombre)
        else:
            # couleur = "lightgreen"
            couleur = "black"
            nombre = None 
    else:
        print(f'{RED}ERREUR{RESET}')
        exit(1)
        
    # Définir la vitesse maximale de la tortue
    turtle.speed(0)

    # Définir la couleur de remplissage
    turtle.fillcolor(couleur)      

    # Débuter le remplissage
    turtle.begin_fill()
    
    # Dessiner le carré
    turtle.pendown()
    for _ in range(4):
        turtle.forward(taille)
        turtle.left(90)
    turtle.penup()
    
    # Finir le remplissage
    turtle.end_fill()

    # Écrire le nombre au centre du carré
    if nombre is not None:  # Vérifier si un nombre est spécifié
        turtle.forward(taille / 2)
        turtle.pencolor("black")
        turtle.write(str(nombre), align="center", font=("Times New Roman", int(taille / 2)))
        turtle.backward(taille / 2)

#----------------------------------------------------------------------------#

def afficher_aux(file, environnement, verdict=False):
    """
    Affiche un environnement de carrés en utilisant la tortue.
    
    ARGUMENTS
    - environnement : liste de listes représentant les valeurs des carrés
    """

    # Taille d'un carré
    long = len(environnement)
    l2 = len(environnement[0])
    if long > l2:
        taille_carreau = 50 if long < 12  else (30 if  long <= 16 else 32 - long/3)
    else:
        taille_carreau = 50 if l2 < 12  else (30 if  l2 <= 16 else 31 - l2/3)
    
    # Texte de Présentation
    turtle.penup()
    turtle.goto(300, 60)
    turtle.write("Paint It Black II", move=True, align="center", font=("Times New Roman", 16))
   
    
    # Ecriture du Verdict
    turtle.goto(300, 30)
    if verdict:
        turtle.pencolor("darkgreen")
        turtle.write("SATISFAISABLE", move=True, align="center", font=("Times New Roman", 16))
    else:
        turtle.pencolor("darkred")
        turtle.write("INSATISFAISABLE", move=True, align="center", font=("Times New Roman", 16))
    turtle.pencolor("black")

    turtle.goto(300, 15)
    turtle.write(file, move=True, align="center", font=("Times New Roman", 10))
    
    # Déplacement du curseur
    if long < 12:
        turtle.goto(-400, 200)
    else:
        turtle.goto(-500, 280)

    # Définir la couleur de fond
    turtle.bgcolor("white")

    # Vitesse maximale de la tortue
    turtle.speed(0)

    # Parcourir chaque ligne et chaque colonne de l'environnement
    # print("env : ", environnement)
    for ligne in environnement:
        for valeur in ligne:
            # Dessiner le carré
            carre(taille_carreau, valeur)
            turtle.forward(taille_carreau)
        
        # Retour à la ligne suivante
        turtle.backward(taille_carreau * len(ligne))
        turtle.right(90)
        turtle.forward(taille_carreau)
        turtle.left(90)
    
    # Affichage des informations sur l'historique
    turtle.goto(300, -60)

    turtle.pencolor("black")
    texte = ''' Projet INF402 (Logique) | Universite Grenoble Alpes 
                                Licence 2, 2024'''
    turtle.write(texte, move=True, align="center", font=("Times New Roman", 10))

#----------------------------------------------------------------------------#
   
def init_envt(config: dict, modele: list):
    """
    Crée une grande liste nommée "envt" à partir de la configuration et du modèle.
    
    ARGUMENTS
    - config : dict ---> contient les donnees de la configuration du Jeu, les ligne et les colonnes
    - modele : list ---> le modele trouvé par rapport à la configuration (si il existe)
    
    RETOUR
    - envt : list de list ---> contient des listes correspondant à toutes les lignes / colonnes de la configuration 
    - verdict : bool ---> True si le modele existe, False sinon
    """
    
    # DIM = config["dim"]
    DIM = get_cle(config, "dim")
    # LIGNES = config["lignes"] # liste de toutes les lignes
    LIGNES = get_cle(config, "lignes")
    # COLONNES = config["colonnes"] # liste de toutes les colonnes
    COLONNES =  get_cle(config, "colonnes")

    # Recherche de la taille maximale des lignes et des colonnes
    taille_max_col = max(len(colonne) for colonne in COLONNES)
    taille_max_lig = max(len(ligne) for ligne in LIGNES)
    
    envt = []
    
    # PREMIERE PARTIE: Creation de nouvelles lignes a partir des colonnes       
    # Extraction des donnees
    NOUV_LIGNES = []
    for _ in range(taille_max_col):
        NOUV_LIGNES.append([])
    
    
    for colonne in COLONNES:      
        diff_taille_col = taille_max_col - len(colonne) # nombres des elements manquant dans colonne pour avoir la taille max
        for _ in range(diff_taille_col) :
            colonne.insert(0,'#')
        
        # Creation des listes pour chaque élément k element de "colonne" dans COLONNES
        for k in range(taille_max_col):
            NOUV_LIGNES[k].append(float(colonne[k]) if colonne[k] != '#' else colonne[k])
            
    
    # Extension des nouvelles lignes dans envt[]
    debut_ligne = [0 * taille_max_lig for i in range(taille_max_lig)]
    for new_line in NOUV_LIGNES:
        new_line = debut_ligne + new_line
        envt.append(new_line)
       
    
    
    # DEUXIEME PARTIE: Gestion sur les lignes : ajout du modèle
    long_modele = len(modele)
    # verdict = True if (long_modele == DIM[0] * DIM[1]) else False
    verdict = True if modele != [] else False
        
    i = 0 # Identifiant de variable (ici 1ere case)
    for l in LIGNES: 
        diff_taille_lig = taille_max_lig - len(l)
        
        l = [float(entier) for entier in l] # Transformation des entiers en réels

        for _ in range(len(COLONNES)):
            if long_modele == 0:
                l.append('#')
            else:
                l.append(modele[i])
            i += 1
        
        # Ajout de '#' pour combler les probables trous au debut de la 'ligne' par rapport à la plus longue ligne
        for _ in range(diff_taille_lig) :
            l.insert(0,'#')
        
        # Ajout de la liste dans la grande liste 'envt'
        envt.append(l)             
        
    return envt, verdict

#----------------------------------------------------------------------------#

def afficher(file: str, config: dict, modele: list):   
    # Désactiver l'animation pour améliorer les performances
    turtle.Screen().tracer(0)
    
    # Initialisation de l'environment
    envt, verdict = init_envt(config, modele)
    
    # Dessiner l'environnement
    afficher_aux(file, envt, verdict)

    # Cacher la tortue (optionnel)
    turtle.hideturtle()

    # Rafraîchir l'écran une fois tous les dessins effectués
    turtle.Screen().update()

    # Garder la fenêtre ouverte jusqu'à la fermeture manuelle
    turtle.done()


# modele1: List[int] = [1, 2, -3, -4, -5, 6, -7, 8, -9, 10, 11, 12, -13, 14, -15, 16, 17, -18, -19, 20, 21, 22, -23, -24, -25, 26, 27, 28, 29, -30, -31, -32, -33, 34, -35, -36]
# config1: dict = {
#    "dim": [6, 6],
#     "lignes": [
#         [2,1],
#         [1,3],
#         [1,2],
#         [3],
#         [4],
#         [1]
#     ],
#     "colonnes": [
#         [1],
#         [5],
#         [2],
#         [5],
#         [2,1],
#         [2]
#     ]
# }
#
# modele2: List[int] = [1, 2, 3, -4, 5, 6, 7, 8, 9, 10, 11, 12, 13, -14, -15, 16]
# config2: dict = {
#    "dim": [4, 4],
#     "lignes": [
#         [3],
#         [4],
#         [4],
#         [1,1]
#     ],
#     "colonnes": [
#         [4],
#         [3],
#         [3],
#         [3]
#     ]
# }
#
# modele4: List[int] = []
# for i in range(1,401):
#     modele4.append(i)
# config4: dict = {
#    "dim": [20, 20],
#     "lignes": [
#         [2],
#         [2,1,8],
#         [3,11],
#         [1,4,2,4],
#         [1,7,1],
#         [1,1,2,2,1,1],
#         [1,8,2],
#         [1,4,4,1],
#         [6,2,2,3],
#         [11,1],
#         [4,3,2,3],
#         [2,3,3,1,1],
#         [8,1,5,1],
#         [6,3,1,2],
#         [3,4,1,1],
#         [5,4,1],
#         [5,1,1],
#         [2,2,1,1],
#         [4,4,1],
#         [5,5,1],
#     ],
#     "colonnes": [
#         [1,3],
#         [2,1,2,5],
#         [1,1,8,2],
#         [2,3,8],
#         [1,3,2,5],
#         [1,3,3,2,2],
#         [8,6,1],
#         [3,2,4,1,2],
#         [4,2,2,3,3],
#         [2,3,4,3,2],
#         [2,6,3,5],
#         [4,2,3,1,2],
#         [10,1,1],
#         [2,4,2,1],
#         [3,2],
#         [3,1,2,1],
#         [3,1,1,1],
#         [1,2,1,1,1],
#         [2,1,7],
#         [3],
#     ]
# }
#
#
# #afficher(config1, modele1)
