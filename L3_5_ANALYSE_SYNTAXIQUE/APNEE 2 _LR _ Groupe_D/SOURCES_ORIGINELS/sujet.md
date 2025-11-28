# APNEE 2: Analyse LR : Langages des assignations
## Objectif
L'objectif de cette APNEE est de construire un programme permettant l'évaluation de séquences d'assignations. Ce programme sera obtenu en utilisant l'outil bison (qui applique une analyse LALR(1).

### Principe général

Le programme attendu doit lire une séquence d'assignations. Par exemple:

```
a = 2 + 2;
x = z = (10 - 5 - (y = 2 * 3) + (x = 2) / 10) ;
```
Une fois évalué, votre programme affichera la table des variables ainsi définies :

```
y = 6.000000
a = 4.000000
x = -0.800000
z = -0.800000
```

L'ordre des variables dans la table est sans importance. Si le mot lu n'est pas un mot du langage, le programme doit afficher sur la sortie standard (stdout) :

!!! ERREUR !!!

Les programmes créés prennent leur entrée dans l'entrée standard et affiche leur résultat dans la sortie standard. Attention, si vous souhaitez rajouter des traces, elles doivent s'écrire dans le flux d'erreur (stderr et non dans le flux de sortie standard (stdout), réservé au résultat.

### Langage spécifique à traiter

Les mots du langages consistent en une séquence d'assignations telles que définies dans la grammaire des assignations du TD appliqué 12 (LL1). Les assignations sont interprétées dans le domaine **des flottants (opérations flottantes et valeurs flottantes)**. Les opérateurs attendus sont les opérateurs binaires +, -, *, / et l'opérateur ^ (représentant la puissance: x^y = xy) auxquels vous rajouterez les opérateurs cos et sin et la constante pi. Vous veillerez à ce que les expressions cos x ou cos(x) soient reconnues (ce qui revient à traiter cos comme un opérateur unaire, dont la priorité est inférieur à la priorité de l'addition. Ainsi, cos x + 2 s'interprète comme cos(x + 2).

### Vous ne partez pas de rien
Pour faire ce programme, il faut fournir un certain nombre de services, tels que :

- faire l'analyse lexicale des éléments de la donnée (reconnaître les variables, valeurs immédiates et opérateurs);
- faire l'analyse syntaxique du langage des assignations;
- calculer les valeurs;
- maintenir une table de symboles (association variable-valeur), à afficher en fin de traitement.

L'objectif principal de ce TP est de faire les 2 étapes intermédiaires en utilisant le générateur d'analyseur bison, vous partirez d'un ensemble d'éléments pré-existants disponibles dans cette activité :

- **expr.lex** : un fichier "flex" permettant de générer l'analyseur lexical. Il est un peu différent de sa version précédente de 'APNEE 1, car nous n'avons plus à gérer de regard en avant et nous n'écrirons pas nous même l'analyseur.
- **expr.y** : un fichier "bison" qui est utilisé pour générer un analyseur syntaxique (par la méthode LALR vue en cours) de la grammaire des expressions simplifiées. Ce fichier vous donne un exemple d'analyseur en bison.
- **assign1.lex** et **assign1.y** : les fichiers flex et bison que vous devrez compléter pour réaliser la première partie.
- **assign2.lex** et **assign2.y** : les fichiers flex et bison que vous devrez compléter pour réaliser la seconde partie.
- **uthash.h**, **table_symboles.c** et **table_symboles.h** : une librairie/template C qui permet de gérer une structure d'association basée sur une table de hashage et les fichiers C en charge de l'utiliser (a priori, vous n'avez pas à les modifier).
- **Makefile**: un makefile qui permet de générer les lexers (analyseurs lexicaux) et les analyseurs syntaxiques et de compiler les sources ainsi générés pour construire les programmes principaux.


Réaliser la table des symboles en C n'est pas une activité simple et n'est pas l'objectif de cette APNEE. Aussi, vous verrez que le fichier table_symboles.c contient déjà des fonctions prédéfinies pour gérer la table des symboles.

### Travail préliminaire : prise en main de bison
Les fichiers expr.lex et expr.y permettent de construire un tout petit analyseur d'expressions. Utilisez ces fichiers pour vous familiariser avec l'outil bison. Étudiez le contenu de ces 2 fichiers, compilez le programme expr et testez le pour voir ce qu'il fait.

### Partie 1: Grammaire codant les priorité d'opérateurs
**Cette partie sera réalisées dans les fichiers assign1.y et assign1.lex**

Dans un premier temps, il vous faut construire la grammaire du langage des assignations tel que défini ci dessus. Vous prendrez bien soin de réaliser une grammaire non ambiguë et dont l'arbre d'analyse reflétera fidèlement les propriétés (priorité et associativité) de l'ensemble des opérateurs demandés. Notez la grammaire ainsi obtenue dans le fichier **README.md**. 

Traduisez cette grammaire dans le format bison dans les fichier assign1.lex et assign1.y. Vous pourrez être amenés à rajouter des "tokens" (des terminaux), à décrire dans le fichier .lex et devrez traduire les règles de grammaires dans le fichier .y. Vous augmenterez ensuite cette grammaire avec les actions nécessaires pour que les expressions soient correctement calculées et que la table des symboles soit correctement utilisée.

Si votre grammaire est correcte, il ne devrait pas y avoir de conflit détecté par l'outil bison. De plus, si la grammaire a été écrite de manière à ce que l'arbre syntaxique reflète les priorités et associativités usuelles des opérateurs, il ne devrait donc pas y avoir de problème d'ordre d'exécution des calculs.

*Il est possible de rajouter des actions à tout moment dans une règle de grammaire (et pas uniquement à la fin). Cela peut être utile par exemple pour évaluer une partie d'une expression avant d'analyser la suite. Mais attention, dans ce cas, l'action qui a été rajoutée compte dans la numérotation des expressions de la forme $1, $2, etc. utilisées dans les expressions suivantes.
Exemple: si une règle s'exprime sous la forme :

E : E PLUS T { $ $ = $1 + $3 }    
// (où $1 représente la valeur du 1er E et $3 la valeur du T)

rajouter une action changera la numérotation de la manière suivante :

E : E { printf("%f", $1); }

PLUS T { $ $ = $1 + $4 } 
// (la valeur du T est maintenant désigné par $4)*

### Partie 2: Grammaire ambiguë
Cette partie sera réalisées dans les fichiers *assign2.y* et *assign2.lex*

Vous allez maintenant utiliser une grammaire plus simple, qui ne code pas les priorités d'opérateurs, et qui, en toute rigueur, est ambiguë.

Un exemple de telle grammaire (pour les expressions) est : 

```
E -> E+E | E*E | a | i | (E)
```

Nous avons vu en cours que cette grammaire est ambiguë et qu'elle génère des conflits dans les tables LR.

Compilez la grammaire ainsi obtenue et vérifiez que vous obtenez bien des conflits.

Profitez en pour ajouter l'option -r all a la commande bison dans le Makefile pour obtenir un fichier nommé assign2.output (vous pourrez voir son contenu en exécutant l'activité et en utilisant les commandes ls et more dans le shell). Vous pourrez ainsi voir l'automate des Items et l'endroit exact où les conflits ont lieu. 

Bison peut résoudre ces conflits dans les tables de manière systématique si il connait les bonne priorité et associativité des différents opérateurs. Pour cela, en lieu et place de la ligne %token... qui déclare simplement la liste des tokens utilisés dans la grammaire, vous allez déclarer les tokens et leur priorité/associativité de la manière suivante :

- l'associativité des opérateur sera décrite en utilisant les mots clés %left ou %right (ou %nonassoc) à la place du mot clé %token;
- la priorité des opérateurs sera établie en ordonnant correctement les déclarations d'associativité. Ainsi la première directive %left déclarera un opérateur associatif à gauche dont la priorité est la plus basse.
     - par exemple, dans la grammaire des expressions ci dessus, on pourra déclarer les tokens ainsi :
    %left PLUS MOINS
    %left MULT DIV
    ce qui dit à bison que les 2 opérateurs sont associatifs à gauche et que les opérateurs additifs sont moins prioritaires que les opérateurs multiplicatifs.

Construisez l'analyseur syntaxique complet de votre langage en utilisant une grammaire plus simple et la bonne déclaration des priorités de vos opérateurs.

### Le rendu
L'objectif n'est pas de terminer à tout prix, mais de faire autant que l'on peut dans le temps imparti. Aussi, je vous demande de déposer vos programmes sur la plateforme caseine. Attention, au nommage des fichiers qui ne doit pas changer. Votre projet doit pouvoir compiler (via make) et s'exécuter en prenant son entrée dans le fichier test.txt (l'exécutable ne doit pas changer de nom). Vous ajouterez votre compte rendu (avec a minima la grammaire considérée et une explication de l'état d'avancement de votre projet) dans le fichier README.md

### ATTENTION, cette APNEE est individuelle.

**ATTENTION**: une détection de plagiat de code sera mise en place. En cas de plagiat, je ne chercherai pas à savoir qui a copié sur qui, 
mais TOUS les groupes ayant du code en commun verront leur note réduite à néant. 
Ne vous en faites pas, le simple changement de noms de variables/procédure/ordre de fonctions/etc. 
n'empêche pas la détection de plagiat.

**Si, pour une raison légitime, vous avez dû reprendre le code d'un autre étudiant pour réaliser une partie de l'activité, vous DEVEZ le dire et l'expliquer dans le fichier de compte rendu, sinon, cela sera considéré comme plagiat.**

