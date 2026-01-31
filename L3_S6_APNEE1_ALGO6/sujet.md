# L3 Informatique, ALGO6, APNEE1 : Couplage

Ce travail est à faire individuellement. Le rendu (un par personne), est à faire dans l'activité VPL de Caseine qui évaluera automatiquement votre travail sur des jeux de tests. 


## 1. Introduction
Dans cette apnée, vous allez résoudre le problème du couplage parfait. Par rapport au problème étudié en TD2, on s'intéressera ici au cas de la recherche du couplage parfait dans un graphe quelconque et non uniquement pour les graphes bipartis.

Pour mémoire, ce problème peut se formuler de la manière suivante :

Soit un graphe G=(S, A) composé d'un ensemble de n sommets S, et d'un ensemble d'arêtes A, partie de S×S.

Problème : existe-t-il un couplage parfait C, c'est-à-dire un sous-ensemble de A tel que tout sommet de S est couvert par exactement une arête de C.

Exemple
Données du problème :

```txt
n = 8
S = {a, b, c, d, e, f, g, h}
A = {(a, h), (b, e), (c, f), (c, g), (c, h), (d, e), (d, g)}
Solution : oui, il existe un couplage parfait, on peut prendre par exemple l'ensemble :

C = {(a, h), (b, e), (c, f), (d, g)}
```

## 2. Implémentation d'un type Graphe en Java
Nous allons présenter ici une implémentation incomplète d'un type Graphe muni de ses opérations de base (opérations sur le graphe et entrées/sorties) sous la forme d'une classe Java.

### 2.1. Classe Graphe
La classe Java nommée Graphe permet de gérer une structure de données de type graphe aux arcs étiquetés. Nous supposerons dans la suite que les sommets d'un graphe sont de type int et que les étiquettes associées aux arcs sont des éléments de type Etiquette.

À noter que les sommets du graphe sont décalés de 1 par rapport à leur représentation textuelle (de 0 à n-1 plutôt que de 1 à n).


Le code constitue un début d'implémentation de la classe Graphe. Les structures de données déclarées correspondent à une représentation interne du graphe sous forme de listes d'adjacence (le graphe est un tableau de listes chaînées d'arcs).

Les fonctions de lecture depuis un flux et de conversion en représentation textuelle vous sont fournies (en utilisant le format précisé ci-dessous).

Les primitives de consultation suivantes sont également fournies :

- nombre de sommets du graphe
- nombre d'arcs du graphe
- existence d'un arc entre deux sommets
- étiquette associée à un arc
- ensemble des successeurs d'un sommet (retourné sous forme de tableau)
- ensemble des arcs du graphe (retourné sous forme de tableau).

En utilisant les types et primitives de la liste précédente (fournis par votre classe Graphe), vous pouvez écrire de nouvelles méthodes, par 

exemple :
- Degre: donnant le degré d'un sommet
- ArcsIndependants: précisant si deux arcs en entrée sont indépendants
- EstCouplage: (un couplage est un sous-ensemble d'arcs tel que chaque sommet du graphe soit l'extrémité d'au plus un arc de l'ensemble)
Pour tester vos méthodes vous devez utiliser (en le modifiant) le programme principal fourni (EssaiGraphe.java).

### 2.2. Format choisi pour la représentation textuelle
Le texte de description du graphe comporte, dans l'ordre, les éléments suivants (séparés par un nombre quelconque d'espaces, tabulations ou retours à la ligne) :

le nombre de sommets du graphe (un entier ; les sommets seront numérotés de 1 à cet entier)
un ensemble d'arcs, chaque arc étant décrit par un mot (sans espace) de la forme :
no de l'arc / no sommet origine + no sommet extremite /-> étiquette de l'arc

Exemple pour l'arc n°5, du sommet 12 au sommet 14, et de poids 1:

5/12+14/->1


Représentation textuelle pour l'exemple de l'introduction (7 arcs) :
```txt
8
1/1+8/->1
2/2+5/->1
3/3+6/->1
4/3+7/->1
5/3+8/->1
6/4+5/->1
7/4+7/->1
```
On trouvera dans le répertoire exemples/ de l'archive fournie des exemples de graphes stockés dans des fichiers selon ce format. Dans ces exemples, les étiquettes des arcs sont des entiers.

`Remarque` : dans cette apnée, le poids des arêtes (étiquette) peut être ignoré, même si on utilise une représentation générique de graphes pondérés.

## 3. Consignes pour le comportement de votre programme
Votre programme devra respecter le comportement ci-dessous. Les messages doivent être affichés sur la sortie standard. L'affichage de messages de debug est toléré (en quantité raisonnable) tant que le dernier message est celui demandé. Ce comportement est nécessaire pour la vérification sur Caseine.

Si des propriétés simples du graphe rendent impossible l'existence d'un couplage parfait (par exemple, nombre de sommets impair), afficher :
`Couplage impossible`

S'il n'y a pas de couplage parfait après recherche, afficher (sans accent) : `Couplage non trouve`

Si un couplage parfait existe, afficher (sans accent) la liste des numéros des arêtes du couplage :
`Couplage trouve : 1,2,3,7`

## Note : 
pour vérifier la conformité de votre programme, utilisez le script Python verif_sortie.py fourni comme ci-dessous. Caseine effectue également cette vérification.

java Couplage exemples/fichier-de-graphe | ./verif_sortie.py

## 4. Une solution
À défaut d'algorithme efficace, une solution envisageable est d'essayer toutes les possibilités. Cela revient à énumérer l'ensemble des parties de cardinal n/2 de A afin de déterminer s'il en existe une qui constitue un couplage.

Dans notre cas, cela revient à énumérer les parties de l'ensemble d'arcs dont le cardinal est égal à la moitié du nombre de sommets. Si l'une de ces parties est un couplage, alors l'instance admet une solution.

Remarque : cet algorithme est correct mais inefficace ; il peut servir de référence pour les essais ultérieurs.

## 5. Travail à effectuer
Implémentez la solution ci-dessus en vous servant de la classe Graphe.

Les quelques exemples de graphes fournis constituent une base initiale de tests, mais sont insuffisants pour valider complétement votre algorithme. Caseine utilise une base de tests plus conséquente. Il est donc recommandé de créer vos propres instances de graphes.

Vous devez rendre sur Caseine les sources Java nécessaires au fonctionnement du programme dans

l'activité VPL dédiée.

## 6. Évaluation
Cette Apnée sera évalué automatiquement sur Caseine, mais une partie du Quick 1 portera également sur ce sujet. Pour vous préparer, nous vous conseillons de réfléchir également aux questions suivantes:

Y-a-t'il des graphes pour lequel ce problème est facile ou au contraire difficile ?
Étant donné n et A, combien existe-t-il de parties de cardinal n/2 dans A ?
Quelle est la complexité de l'algorithme?