from typing import Tuple, List, Any, IO, NewType

# formule de base obtenue à partir d'une lecture d'un fichier de données (json)
JEU = NewType("JEU", List[List[List[int]]])
#                    ^^^  ^^^  ^^^
#                    |    |    | une possibilité pour une ligne ou une colonne (monôme) ex : [x1 * -x2 * x3]
#                    |    | une ligne ou une colonne
#                    | l'ensemble des lignes et des colonnes

# forme normale conjonctive
FNC = NewType("FNC", List[List[int]])
#                    ^^^  ^^^
#                    |    | une clause
#                    | ensemble de clauses (forme normale conjonctive)

FILE = NewType("FILE", IO)
