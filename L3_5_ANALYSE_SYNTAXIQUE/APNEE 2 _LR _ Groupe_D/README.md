# Petit mémo de compilation

Installer flex et bison avant de lancer un `make` :
- Linux (ubuntu) : `sudo apt-get install flex bison` (ou équivalent)
- Mac : normalement déjà là, sinon `brew install flex bison`
- Windows : pas testé, à chercher suivant l’environnement

Compilation et tests rapides :
```
make
./tests/run_tests.sh   # lance les jeux d’essai fournis (Python 3 requis)
```


# Compte rendu (à remplir)

Ci-dessous ce que j’ai retenu pour les grammaires utilisées.

## assign1 (grammaire non ambiguë, priorités encodées directement)
```
Programme        -> ListeInstr
ListeInstr       -> ListeInstr Instruction
ListeInstr       -> ε
Instruction      -> ExprAffectation ;
ExprAffectation  -> VAR = ExprAffectation
ExprAffectation  -> Expression
Expression       -> ExpressionSomme
ExpressionSomme  -> ExpressionSomme + Terme
ExpressionSomme  -> ExpressionSomme - Terme
ExpressionSomme  -> Terme
Terme            -> Terme * Puissance
Terme            -> Terme / Puissance
Terme            -> Terme % Puissance
Terme            -> Puissance
Puissance        -> Unaire ^ Puissance          // ^ droite-associatif
Puissance        -> Unaire       
Unaire           -> - Unaire
Unaire           -> + Unaire
Unaire           -> Primaire
Primaire         -> FLOTTANT
Primaire         -> pi
Primaire         -> VAR
Primaire         -> (ExprAffectation)
Primaire         -> cos ExpressionSomme
Primaire         -> sin ExpressionSomme
```

## assign2 (grammaire volontairement ambiguë, priorités imposées par bison)
```
Programme        -> ListeInstr
ListeInstr       -> ListeInstr Instruction
ListeInstr       -> ε
Instruction      -> ExprAffectation ;
ExprAffectation  -> VAR = ExprAffectation
ExprAffectation  -> Expression
Expression       -> Expression + Expression
Expression       -> Expression - Expression
Expression       -> Expression * Expression
Expression       -> Expression / Expression
Expression       -> Expression % Expression
Expression       -> Expression ^ Expression
Expression       -> cos Expression
Expression       -> sin Expression
Expression       -> + Expression
Expression       -> - Expression
Expression       -> ( Expression )
Expression       -> FLOTTANT
Expression       -> pi
Expression       -> VAR
```
Priorités dans `assign2.y` (de la plus faible à la plus forte) :
`%right ASSIGN`, `%right COS SIN`, `%left PLUS MINUS`, `%left MULT DIV MODULO`, `%right EXPON`, `%precedence UPLUS UMINUS`.

## Difficultés (mon ressenti)

- Pour que `cos x + 2` soit lu comme `cos(x+2)`, j’ai mis cos/sin avec la plus faible priorité et un argument qui est une expression complète.
- La version ambiguë oblige à bien régler les `%left/%right` pour obtenir le même comportement que la grammaire non ambiguë.

## Remarques
