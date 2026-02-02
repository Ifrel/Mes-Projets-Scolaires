#  L3 Informatique, ALGO6, Algorithmique et modélisation
## APNEE 2 : Plus Longue Sous-Séquence Commune (PLSSC)
Ce travail est à faire individuellement. Le rendu (un par personne), est à faire dans l’activité VPL de Caseine qui évaluera automatiquement votre travail sur des jeux de tests. Vous devez rendre votre travail avant 18h sur Caseine.

Contact en cas de problème durant l’apnée envoyez un message sur Caseine à Florent Bouchez Tichadou.

Note: comme pour l’Apnée 1, des erreurs peuvent subsister, en particulier sur le VPL de rendu. Nous faisons notre possible pour les minimiser.

### Objectifs
L’objectif de cette Apnée est d’implémenter l’algorithme de recherche de la PLSSC utilisant la programmation dynamique, d’évaluer ses performances de en comparaison avec un algorithme naïf de recherche de motif, et de tester ces algorithmes.

### Ressources fournies
- [`RecherchePLSSC.java`./RecherchePLSSC.java], programme à compléter;
- [exemple.test], un exemple simple.

### Partie 1. Compréhension du programme fourni et implémentation
Le programme Java [RecherchePLSSC.java] contient un squelette d’implémentation à compléter : il faudra écrire une implémentation “naïve” et une implementation basée sur la programmation dynamique. La version naïve peut-être, au choix, une version d’énumération de toutes les sous-séquences possibles, ou une version récursive “pure” (sans mémoïsation).

Le programme prend en argument une option suivie d’une liste de fichiers, les deux premières lignes de chacun de ces fichiers sont les chaînes dont il faut chercher la PLSSC.

Lisez et complétez le programme RecherchePLSSC.java.

Vous pouvez compiler ce programme à l’aide de la commande suivante :

    javac RecherchePLSSC.java
Le programme s’exécute ensuite par la commande :

    java RecherchePLSSC <option> <fichier1> [<fichier2> ...]
Le traitement de l’option devra être rajouté au programme fourni. option peut être l’une des trois possibilités suivantes :

- -n : pour exécuter l’algorithme naïf
- -p : pour exécuter l’algorithme utilisant la programmation dynamique
- -a : pour exécuter les deux consécutivement (all) : naïf puis prog. dyn.

Le comportement de votre programme devra être le suivant: pour chaque fichier de test donné sur la ligne de commande, afficher:

```bash
PLSSC_x: <une plus longue sous-chaine>
Time_x: <longueur de S1>  <longueur de S2>  <le temps d'exécution>
```
avec _x qui dépend de l’algorithme utilisé: _n pour naïf et _p pour prog. dyn (et les deux à la suite pour l’option -a).

### Partie 2. Tests
Nous vous fournissons un exemple simple pour tester vos algorithmes. Le format de ce fichier est le suivant :

    Ligne1: première chaine
    Ligne2: deuxième chaine
    Ligne3: longueur d'une PLSSC
    Ligne4: une PLSSC
Lors du rendu, vos algorithmes seront testés sur des exemples plus complexes et mettant en évidence des cas particuliers. Il est recommandé de vous créer vos propres exemples afin de tester/valider vos algorithmes.

Par ailleurs, par vous même des exemples afin de Il vous est donc demandé de créer par vous même des exemples afin de tester/valider vos algorithmes. Les fichiers devront suivre le format ci-dessus, avec XX la longueur de S1 et YY la longueur de S2. Les longueurs choisies devront être adaptées à chaque situation (suffisamment grandes pour exhiber un comportement intéressant, tout en restant raisonnables en termes de consommation de ressources).

- slow_XX_YY.test : exemple pour lequel l’algorithme naïf prend 100x plus de temps que celui de prog. dyn.
- best_XX_YY.test : exemple qui montre un meilleur cas d’exécution (en termes de complexité) pour la prog. dyn.
- worse_XX_YY.test : exemple qui montre un pire cas d’exécution (en termes de complexité) pour la prog. dyn.
- greedy_XX_YY.test : exemple qui doit piéger un algorithme glouton : un algorithme glouton tenu secret doit se tromper, i.e., renvoyer une SSC qui n’est pas la plus longue.

(Note: les PLSSC ne sont pas forcément uniques, plusieurs “plus longues” solutions peuvent exister.)

### Partie 3. Préparation du quick 1
Cette Apnée sera évalué automatiquement sur Caseine, mais une partie du Quick 1 portera également sur ce sujet. Pour vous préparer, nous vous conseillons de réfléchir également aux questions suivantes:

- Quel est le coût dans le pire cas des deux algorithmes que vous avez implémentés ? À quel type de données correspond ce pire cas ?

- Sur un même graphique, tracez le temps d’exécution en fonction de la taille des chaînes pour les 2 algorithmes (naïf, programmation dynamique). Que peut-on conclure de ce graphique ?