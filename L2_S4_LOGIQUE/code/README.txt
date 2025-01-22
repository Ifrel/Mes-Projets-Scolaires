################################
# Paint it black II (nonogram) #
################################

D'hérin Arthur
Makoundika Rinel
Cheikh Anis

# [[red]] à compléter [[red]]

__ notes __ :

il reste peut être à certains endroit des expressions du genre 'JEU(expression)' où 'JEU' est un type contstruit et 'expression' est une liste, utile seulement pour le type checking optionnel, inutile pour l'intrepréteur

__ structure du projet __ :

-   un dossier bin/ contenant l'exécutable de bddc (https://www-verimag.imag.fr/~raymond/home/tools/bddc/) -> calculatrice logique (utile pour faire des vérifications) 
    et de cryptominisat (https://github.com/msoos/cryptominisat) -> sat solver
-   un dossier tests/ contenant des tests, un dossier tests/alea pour génénerer des tests aléatoires
-   un dossier demo/ qui contient des images et des tentatives pour la soutenance
