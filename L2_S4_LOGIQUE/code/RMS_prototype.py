
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
            if i > 0:
                return False
    
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
        
    # on verifie les dimmions
    if len(var_liste) != n:
        return False
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


def poss_avec_1_var(contrainte: list, var_liste: list) -> list[list[int]]:
    """
    Renvoie une liste de toutes les possibilités pour une valeur dans contrainte

    ARGUMENTS
    - contraintes : liste contenant les contraintes
    """

    resultat = []

    # Si la contrainte n'est pas nulle
    if contrainte[0] != 0:
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

    # Si la contrainte est nulle, il suffit d'inverser le signe de tous les éléments de var_liste
    else:
        resultat.append([-v for v in var_liste])

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

    if len(contrainte) == 1:
        resultat.extend(poss_avec_1_var(contrainte, variables))
    
    else:
        
        c1 = contrainte[0]
        c2 = 1 # Separateur de contraintes
        s_contrs = contrainte[1:]
        s_var = variables[(c1 + c2):]

        res = possibilites(s_contrs, s_var)
        res_2 = poss_avec_1_var([c1], variables[:(c1 + c2)])
      
        # Ajoute toutes les possibilités de la contrainte
        for j in res:
            temp = res_2[0].copy()
            temp.extend(j)
            if integrite(contrainte, temp):
                resultat.append(temp)

        
        for i in res:
            temp = res_2[1].copy()
            temp.extend(i)
            if integrite(contrainte, temp):
                resultat.append(temp)
        



        if len(variables) > (c1):
            temp = [-variables[i] for i in range(c1 + c2)]
            temp2 = [-variables[i] for i in range(c1)]
            temp2.append(variables[c1])
            res_2 = [temp, temp2]

            res = possibilites(contrainte, s_var)
           
            
            # Ajoute toutes les possibilités de la contrainte
            for j in res:
                temp = res_2[0].copy()
                temp.extend(j)
                if integrite(contrainte, temp):
                    resultat.append(temp)

            
            for i in res:
                temp = res_2[1].copy()
                temp.extend(i)
                if integrite(contrainte, temp):
                    resultat.append(temp)
    
    return resultat



cont = [1,1,1,2]
var = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15]
pos = possibilites(cont, var)

c = 1
for i in pos:
    print(i,end="")
    print(" ", c)
    c += 1