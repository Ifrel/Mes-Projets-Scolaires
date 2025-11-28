# Instructions

Pour pouvoir compiler le projet sur votre ordinateur, il vous faut installer les librairies flex et bison.

- Sur linux (ubuntu) cela se fait par la commande :
  
  sudo apt-get install flex bison
  
  (ou équivalent sur d'autres distributions)

- Sur mac, ces librairies sont installées en standard. Si ce n'est pas le cas pour vous, cela peut s'installer via homebrew (cf https://brew.sh/index_fr)

  brew install flex bison

- sur windows, je ne sais pas, à vous de chercher.

# Compte rendu

Je vous demande de rédiger votre compte rendu dans ce fichier que vous rendrez avec vos programmes sur caseine. Je vous suggère les éléments suivants:

## Grammaires utilisée pour les différentes étapes

Donnez ici les grammaires que vous avez utilisées pour résoudre votre problème.

#### Étape 1 : Grammaire de base du TD_12 Exercice E12.2

Grammaire initiale (avec récursion à gauche) :

```
A → x = A | E
E → E + T | T
T → T * F | F
F → x | a | (A)
```
---

#### Étape 2 : Élimination de la récursion à gauche
```
A  → x = A | E
E  → T E'
E' → + T E' | ε
T  → F T'
T' → * F T' | ε
F  → x | a | (A)
```
---

#### Étape 3 : Ajout du séparateur `;` pour les séquences d'assignations
```
S  → A ; S | ε
A  → x = A | E
E  → T E'
E' → + T E' | ε
T  → F T'
T' → * F T' | ε
F  → x | a | (A)
```

**Explication** :
- `S → A ; S` : Une assignation suivie d'autres assignations
- `S → ε` : Fin de la séquence (production vide)
---

#### Étape 4 : Ajout des opérateurs - (soustraction) et / (division)
```
S  → A ; S | ε
A  → x = A | E
E  → T E'
E' → + T E' | - T E' | ε
T  → F T'
T' → * F T' | / F T' | ε
F  → x | a | (A)
```

**Explication** :
- `E' → + T E' | - T E' | ε` : ajout de la soustraction au même niveau que l'addition
- `T' → * F T' | / F T' | ε` : ajout de la division au même niveau que la multiplication

---

#### Étape 5 : Ajout de l'opérateur ^ (puissance)

La puissance a une **priorité plus élevée** que * et /, donc on crée un nouveau niveau U :
```
S  → A ; S | ε
A  → x = A | E
E  → T E'
E' → + T E' | - T E' | ε
T  → U T'
T' → * U T' | / U T' | ε
U  → F U'
U' → ^ F U' | ε
F  → x | a | (A)
```

**Explication** :
- `U` représente les facteurs avec puissance
- `U' → ^ F U' | ε` : la puissance est associative à **droite** (par exemple : 2^3^2 = 2^(3^2) = 512)
```
E (addition/soustraction)
  ↓
T (multiplication/division)
  ↓
U (puissance)
  ↓
F (facteurs)
```
---

#### Étape 6 : Ajout des opérateurs unaires (sin, cos, pi)
**Contrainte importante** : Selon la spécification, `cos x + 2` doit s'évaluer comme `cos(x + 2)`, ce qui signifie que **cos et sin ont une priorité inférieure à l'addition**.
Pour cela, on introduit un nouveau niveau P entre U et F :
```
S  → A ; S | ε
A  → x = A | E
E  → T E'
E' → + T E' | - T E' | ε
T  → U T'
T' → * U T' | / U T' | ε
U  → P U'
U' → ^ P U' | ε
P  → cos A | sin A | F
F  → x | immediate | pi | (A)
```

**Explication** :
- `P → cos A | sin A | F` : les opérateurs unaires sont traités récursivement
- `F → x | immediate | pi | (A)` : ajout de la constante `pi` et remplacement de `a` par `immediate`
- La récursion dans `P → cos A` permet de traiter `cos cos x` ou `sin cos x`, ou `cos(Affectation/Expression)
  **Hiérarchie finale** :
```
S (séquence)
  ↓
A (assignation)
  ↓
E (addition/soustraction)
  ↓
T (multiplication/division)
  ↓
U (puissance)
  ↓
P (opérateurs unaires)
  ↓
F (facteurs)
```
---
## Grammaire finale complète
Voici la grammaire finale LL1 utilisée dans le projet : ajout de la regle `S' → S $ ` (fin de fichier)
```
N = { S', S, A, E, E', T, T', U, U', P, F }
T = { ;, x, =, +, -, *, /, ^, cos, sin, immediate, pi, (, ) }

R0:   S' → S $ 
R1:   S  → A ; S
R2:   S  → ε
R3:   A  → x = A
R4:   A  → E
R5:   E  → T E'
R6:   E' → + T E'
R7:   E' → - T E'
R8:   E' → ε
R9:   T  → U T'
R10:  T' → * U T'
R11:  T' → / U T'
R12:  T' → ε
R13:  U  → P U'
R14:  U' → ^ P U'
R15:  U' → ε
R16:  P  → cos A
R17:  P  → sin A
R18:  P  → F
R19:  F  → x
R20:  F  → immediate
R21:  F  → pi
R22:  F  → (A)
```
---

## Table LL1 (Ensembles Premiers et Suivants)

### Calcul des ensembles PREMIER() et Nε
```
PREMIER(S')   = { cos, sin, x, immediate, pi, ( }
PREMIER(S)    = { cos, sin, x, immediate, pi, ( }
PREMIER(A)    = { cos, sin, x, immediate, pi, ( }
PREMIER(E)    = { cos, sin, x, immediate, pi, ( }
PREMIER(E')   = { +, - }
PREMIER(T)    = { cos, sin, x, immediate, pi, ( }
PREMIER(T')   = { *, / }
PREMIER(U)    = { cos, sin, x, immediate, pi, ( }
PREMIER(U')   = { ^ }
PREMIER(P)    = { cos, sin, x, immediate, pi, ( }
PREMIER(F)    = { x, immediate, pi, ( }

Nε = { S, E', T', U' }
```

### Calcul des ensembles SUIVANT()

```
SUIVANT(S')   = {}
SUIVANT(S)    = { $ }  
SUIVANT(A)    = { ;, ), +, -, *, /, ^ }
SUIVANT(E)    = { ;, ) }
SUIVANT(E')   = { ;, ) }
SUIVANT(T)    = { ;, ), +, - }
SUIVANT(T')   = { ;, ), +, - }
SUIVANT(U)    = { ;, ), +, -, *, / }
SUIVANT(U')   = { ;, ), +, -, *, / }
SUIVANT(P)    = { ;, ), +, -, *, /, ^ }
SUIVANT(F)    = { ;, ), +, -, *, /, ^ }
```

### Table d'analyse LL1 (ici la Grammaire n'est pas LL1)

|    | cos | sin | x     | immediate | pi  | (   | )   | +   | -   | *   | /   | ^   | = | ;   | $  |
|----|-----|-----|-------|-----------|-----|-----|-----|-----|-----|-----|-----|-----|---|-----|----|
| S' | R0  | R0  | R0    | R0        | R0  | R0  |     |     |     |     |     |     |   |     | R0 |
| S  | R1  | R1  | R1    | R1        | R1  | R1  |     |     |     |     |     |     |   |     | R2 |
| A  | R4  | R4  | R3,R4 | R4        | R4  | R4  |     |     |     |     |     |     |   |     |    |
| E  | R5  | R5  | R5    | R5        | R5  | R5  |     |     |     |     |     |     |   |     |    |
| E' |     |     |       |           |     |     | R8  | R6  | R7  |     |     |     |   | R8  |    |
| T  | R9  | R9  | R9    | R9        | R9  | R9  |     |     |     |     |     |     |   |     |    |
| T' |     |     |       |           |     |     | R12 | R12 | R12 | R10 | R11 |     |   | R12 |    |
| U  | R13 | R13 | R13   | R13       | R13 | R13 |     |     |     |     |     |     |   |     |    |
| U' |     |     |       |           |     |     | R15 | R15 | R15 | R15 | R15 | R14 |   | R15 |    |
| P  | R16 | R17 | R18   | R18       | R18 | R18 |     |     |     |     |     |     |   |     |    |
| F  |     |     | R19   | R20       | R21 | R22 |     |     |     |     |     |     |   |     |    |

---


## Difficultés rencontrées

Si vous en avez eu...
- La bonne technique pour trouver la bonne grammaire

## Remarques éventuelles

Si nécessaires...
### Mode Debug (Si besoin)
pour utiliser le Mode debug, il faut :
- Ajouter `-DDEBUG` avec `gcc, clang ...`,  si on compile avec `Make` faire `make DEBUG=1`
- Ajouter dans le `Makefile`
  ```
  ifeq ($(DEBUG),1)
  CFLAGS += -DDEBUG
  endif
  ```