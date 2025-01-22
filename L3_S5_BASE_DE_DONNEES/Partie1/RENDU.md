# I - Normalisation

## 1 - Dépendances fonctionnelles (Communes, ZonesClimatiques et Mesures).

### COMMUNES(Commune, Département, Région, Statut, Altitude_Moyenne, Superficie, Population, Code_Commune, Code_Canton, Code_Arrondissement, Code_Département, Code_Région)
    
    Code_Région -> Région
    Région -> Code_Région

    Code_Département -> (Département, Code_Région, Région)
    Département -> (Code_Département, Code_Région, Région)

    Code_Département, Code_Commune -> (Département, Code_Région, Région, Commune, Statut, Altitude_Moyenne, Superficie, Population)

    Département, Code_Commune -> (Code_Département, Code_Région, Région, Commune, Statut, Altitude_Moyenne, Superficie, Population)

### MESURES(date_obs, code_insee_departement, departement, tmin, tmax, tmoy)

    code_insee_departement -> departement
    departement -> code_insee_departement
    
    date_obs, code_insee_departement -> departement, tmin, tmax, tmoy 
    date_obs, departement -> code_insee_departement, tmin, tmax, tmoy 

### ZONES_CLIMATIQUES(code_departement, nom_departement, zone_climatique)
    code_departement -> nom_departement, zone_climatique
    nom_departement -> code_departement, zone_climatique
    nom_departement, zone_climatique -> code_departement
    code_departement, zone_climatique -> nom_departement
    

    
     
## 2 - Donner la ou les cles de chaque relation.
### COMMUNES(...)
    {Code_Département, Code_Commune}⁺ = { Code_Département, Code_Arrondissement, Code_Canton , Code_Commune, Département, Code_Région, Région, Commune, Statut, Altitude_Moyenne, Superficie, Population }

    les fermetures déterminent tous les attributs

### MESURES(...)
    {code_insee_departement, date_obs}⁺ = { departement, Code_insee_departement, date_obs, tmin, tmax, tmoy }

    {departement, date_obs}⁺ = { code_insee_departement, departement, date_obs, tmin, tmax, tmoy }

    les fermetures déterminent tous les attributs

### ZONES_CLIMATIQUES(...)
    {code_departement}⁺ = { code_departement, nom_departement, zone_climatique }
    {nom_departement}⁺ = { nom_departement, code_departement, zone_climatique }

    les fermetures déterminent tous les attributs 



## 3 - Donner la forme normale de chaque relation.
### COMMUNES(...)
    1NF validée
    2NF non validée : il y'a des attributs qui ne dépendent que d'une partie de la clé (par exemple `Département` ne dépend que `Code_Département`)

### MESURES(...)
    1NF validée
    2NF non validée : il y'a des attributs qui ne dépendent que d'une partie de la clé (`Département` dépend de `code_insee_departement` seulement)

### ZONES_CLIMATIQUES(...)
    1NF validée
    2NF validée
    3NF validée
    BCNF validée


## 4 - Proposer des décompositions en BCNF si nécessaire.

### COMMUNES(...)
    Régions(code_reg, nom_reg)    
    Départements(code_dep, nom_dep, code_reg)
    Communes(code_com, nom_com, code_cant, code_dep, code_arr, alt_moy, stat, superf, pop)
    
    
### MESURES(...)
    Départements(code_dep, nom_dep)
    Mesures(date_obs, code_dep, tmin, tmax, tmoy)
    

### ZONES_CLIMATIQUES(...)
    ZonesClimatiques(code_dep, nom_dep, zone_clima)

## Si on rassemble les tables

DONC ON A :
    Régions(code_reg, nom_reg)
        avec {code_reg}⁺ -> ...

    Départements(code_dep, nom_dep, code_reg)
        avec {code_dep}⁺ -> ...

    Mesures(date_obs, code_dep, tmin, tmax, tmoy)
        avec {date_obs, code_dep}⁺ -> ...

    Communes(code_com, code_dep, nom_com, code_cant, code_arr, alt_moy, stat, superf, pop)
        avec {code_com, code_dep}⁺ -> ...

    ZonesClimatiques(code_dep, nom_dep, zone_clima)
        avec {code_dep}⁺ -> ...
        

    Désormais, toutes les relations sont en BCNF

    (pour la suite on peut très bien supprimer la table 'ZonesClimatiques' et rajouter un attribut 'zone_clima' directement dans 'Départements' 
        (avec un type énumeré puisqu'on a seulement 'H1' 'H2' et 'H3' comme valeurs possibles)
    )

 ## UML 

Règles de nommage : on a directement les mêmes noms que dans les relations, dans les classes -> sans ambiguité au sein de la classe, court, explicite
Les noms d'attributs sont donc juste des versions raccourcies des attributs de base, où les ' ' sont remplacés par des '_'

