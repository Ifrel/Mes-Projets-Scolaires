# APNEE 1: Analyse LL1 : Langages des assignations
## Objectif
L'objectif de cette APNEE est de construire un programme permettant l'évaluation de séquences d'assignations. L'entrée du programme est un fichier texte de la forme :

```
a = 2 + 2;
x = z = (10 - 5 - (y = 2 * 3) + (x = 2) / 10) ;
```
Une fois évalué, votre programme affichera la table des variables ainsi définies :

```
y = 6.00000
a = 4.00000
x = -0.80000
z = -0.80000
```
L'ordre des variables dans la table est sans importance. Si le mot lu n'est pas un mot du langage, le programme doit afficher sur la sortie standard (stdout) :

!!! ERREUR !!!
Les programmes créés prennent leur entrée dans l'entrée standard et affiche leur résultat dans la sortie standard. Attention, si vous souhaitez rajouter des traces, elles doivent s'écrire dans le flux d'erreur (*stderr* et non dans le flux de sortie standard (stdout), réservé au résultat.

### Langage spécifique à traiter
Les mots du langages consistent en une séquence d'assignations telles que définies dans la grammaire des assignations du TD appliqué 12 (LL1). Les assignations sont interprétées dans le domaine **des flottants (opérations flottantes et valeurs flottantes)**. Les opérateurs attendus sont les opérateurs binaires +, -, *, / et l'opérateur ^ (représentant la puissance: x^y = xy) auxquels vous rajouterez les opérateurs cos et sin et la constante pi. Vous veillerez à ce que les expressions cos x ou cos(x) soient reconnues (ce qui revient à traiter cos comme un opérateur unaire, dont la priorité est inférieur à la priorité de l'addition. **Ainsi, cos x + 2 s'interprète comme cos(x + 2)**.

### Vous ne partez pas de rien
Pour faire ce programme, il faut fournir un certain nombre de services, tels que :

- faire l'analyse lexicale des éléments de la donnée (reconnaître les variables, valeurs immédiates et opérateurs);
- faire l'analyse syntaxique du langage des assignations;
- calculer les valeurs;
- maintenir une table de symboles (association variable-valeur), à afficher en fin de traitement.

L'objectif principal de ce TP est de faire les 2 étapes intermédiaires, vous partirez donc d'un ensemble d'éléments pré-existants disponibles dans cette activité :

- **expr.lex** : un fichier "**flex**" permettant de générer l'analyseur lexical. Ne le changez pas tant que ce n'est pas nécessaire.
- **tokens.h** : un fichier définissant les différents types de "tokens" (les éléments terminaux de la grammaire).
- **lookahead_lexer.c** (et .h) : une implémentation qui permet d'abstraire l'analyseur lexical généré par "**flex**", en fournissant une version permettant un regard en avant de plusieurs terminaux (ce qui nous sera utile, puisque, nous l'avons vu, la grammaire est LL2 et non LL1...). Lisez bien le fichier .h qui contient la documentation des fonctions et notamment des éléments importants en terme de gestion de la mémoire.
- **how_tokens.c**: un tout petit exemple de programme qui montre les terminaux lus par le lexer (vous ne pourrez pas l'exécuter sur caseine) .
- **assignations.c**: le squelette de programme principal qu'il va vous falloir compléter pour réaliser l'activité.
- **uthash.h**: une librairie/template C qui permet de gérer une structure d'association basée sur une table de hashage. Ne pas le toucher, il est utilisé dans **assignations.c**
- **Makefile**: un makefile minimal qui permet de générer le lexer et de compiler les sources et construire le programme assignations.

Réaliser la table des symboles en C n'est pas une activité simple et n'est pas l'objectif de cette APNEE. Aussi, vous verrez que le fichier **assignations.c** contient déjà des fonctions prédéfinies pour gérer la table des symboles.

### Analyse syntaxique
Dans un premier temps, je vous encourage à écrire la grammaire du langage demandé. Attention, c'est un peu différent de celle des TDs appliqués, car il s'agit là d'une séquence d'assignations (séparées par un ;). **Calculez les tables Premiers, Suivants et LL1 pour cette grammaire**.

Rédigez ensuite le programme d'analyse selon le principe d'analyse descendante récursive vu en TD (une méthode pour implanter une analyse LL1 sans gérer soit même la pile.

A ce stade, le programme devrait simplement terminer sans erreur (si le texte lut est dans la grammaire), sans faire de calcul ni gérer de symboles. N'hésitez pas à rajouter des traces pour vérifier le déroulement de l'analyse syntaxique de l'entrée.

**ATTENTION** : toutes vos trace doivent être dirigées vers le fichier d'erreur standard (**stderr**), pas dans la sortie qui sera utilisée uniquement pour la table des symboles.

### Ajout du calcul des expressions
Dans un second temps, **ET SANS CHANGER LA STRUCTURE DU PROGRAMME**, ajouter ce qu'il faut pour que les calculs soient réalisés correctement.

**NOTA** : la grammaire donnée en TD gère correctement les priorités d'opérateurs, mais elle ne gère plus correctement leur associativité. En effet, la plupart des opérateurs se calculent de gauche à droite (associatifs à gauche), mais nous avons dû transformer la grammaire initiale qui était récursive à gauche et l'arbre syntaxique ne capture plus cette associativité.

Il vous faut donc faire attention que les calculs se fassent bien de gauche à droite !

Pour comprendre le problème, dessinez un arbre pour l'expression a = 10 - 5 - 2 et regardez comment vous pouvez faire le calcul pour bien obtenir la valeur 3. N'oubliez pas que l'arbre doit être fait selon la grammaire sans récursivité à gauche et que cet arbre correspond à l'arbre d'appel des fonctions dans votre programme.

### Le rendu
L'objectif n'est pas de terminer à tout prix, mais de faire autant que l'on peut dans le temps imparti. Aussi, je vous demande de déposer vos programmes sur la plateforme caseine. Attention, au nommage des fichiers qui ne doit pas changer. Votre projet doit pouvoir compiler (via make) et s'exécuter en prenant son entrée dans un fichier passé en paramètre (l'exécutable ne doit pas changer de nom, le squelette fourni est déjà conforme à cette exigence). **Vous ajouterez votre compte rendu (avec a minima la grammaire considérée et une explication de l'état d'avancement de votre projet) dans le fichier README.md**

**ATTENTION, cette APNEE est individuelle**.

**ATTENTION**: une détection de plagiat de code sera mise en place. En cas de plagiat, je ne chercherai pas à savoir qui a copié sur qui, 
mais TOUS les groupes ayant du code en commun verront leur note réduite à néant. 

Ne vous en faites pas, le simple changement de noms de variables/procédure/ordre de fonctions/etc. 
n'empêche pas la détection de plagiat.
Si, pour une raison légitime, vous avez dû reprendre le code d'un autre étudiant pour réaliser une partie de l'activité, vous DEVEZ le dire et l'expliquer dans le fichier de compte rendu, sinon, cela sera considéré comme plagiat.