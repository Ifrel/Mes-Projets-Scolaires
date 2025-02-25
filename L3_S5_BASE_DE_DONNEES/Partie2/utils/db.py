import os
import sqlite3
from sqlite3 import IntegrityError

import pandas

# Pointeur sur la base de données

dbfile = "data/climat_france.db"
if not os.path.isfile(dbfile):
    _ = open(dbfile, "w")  # si la BD n'existe pas
    _.close()
data = sqlite3.connect(dbfile)
data.execute("PRAGMA foreign_keys = 1")


# Fonction permettant d'exécuter toutes les requêtes sql d'un fichier
# Elles doivent être séparées par un point-virgule
def updateDBfile(data: sqlite3.Connection, file):

    # Lecture du fichier et placement des requêtes dans un tableau
    createFile = open(file, "r")
    createSql = createFile.read()
    createFile.close()
    # sqlQueries = createSql.split(";")
    #
    # # Exécution de toutes les requêtes du tableau
    cursor = data.cursor()
    # for query in sqlQueries:
    #     cursor.execute(query)

    cursor.executescript(createSql)


# Action en cas de clic sur le bouton de création de base de données
def createDB():
    try:
        # On exécute les requêtes du fichier de création
        updateDBfile(data, "data/createDB.sql")
    except Exception as e:
        print(
            "L'erreur suivante s'est produite lors de la création de la base : "
            + repr(e)
            + "."
        )
    else:
        data.commit()
        print("Base de données créée avec succès.")


# En cas de clic sur le bouton d'insertion de données
# TODO Q4 Modifier la fonction insertDB pour insérer les données dans les
# nouvelles tables
def insertDB():
    try:
        # '{}' : paramètre de la requête qui doit être interprété comme une chaine de caractères dans l'insert
        # {}   : paramètre de la requête qui doit être interprété comme un nombre dans l'insert
        # la liste de noms en 3e argument de read_csv_file correspond aux noms des colonnes dans le CSV
        # ATTENTION : les attributs dans la BD sont généralement différents des noms de colonnes dans le CSV
        # Exemple : date_mesure dans la BD et date_obs dans le CSV

        # On ajoute les anciennes régions
        read_csv_file(
            "data/csv/Communes.csv",
            ";",
            "insert into Regions values (?,?)",
            ["Code Région", "Région"],
        )

        # On ajoute les nouvelles régions
        read_csv_file(
            "data/csv/AnciennesNouvellesRegions.csv",
            ";",
            "insert into Regions values (?,?)",
            ["Nouveau Code", "Nom Officiel Région Majuscule"],
        )

        # On ajoute les départements référencés avec les anciennes régions
        read_csv_file(
            "data/csv/Communes.csv",
            ";",
            "insert into Departements (code_departement, nom_departement,code_region) values (?, ?, ?)",
            ["Code Département", "Département", "Code Région"],
        )

        # On renseigne la zone climatique des départements
        read_csv_file(
            "data/csv/ZonesClimatiques.csv",
            ";",
            "update Departements set zone_climatique = ? where code_departement = ?",
            ["zone_climatique", "code_departement"],
        )

        # On modifie les codes région des départements pour les codes des
        # nouvelles régions
        read_csv_file(
            "data/csv/AnciennesNouvellesRegions.csv",
            ";",
            "update Departements set code_region = ? where code_region = ?",
            ["Nouveau Code", "Anciens Code"],
        )

        # On supprime les anciennes régions, sauf si l'ancien code et le
        # nouveau sont identiques (pour ne pas perdre les régions qui n'ont pas
        # changé de code)
        read_csv_file(
            "data/csv/AnciennesNouvellesRegions.csv",
            ";",
            "delete from Regions where code_region = ? and ? <> ?",
            ["Anciens Code", "Anciens Code", "Nouveau Code"],
        )
        print(
            "Les erreurs UNIQUE constraint sont normales car on insère une seule fois les Regions et les Départements"
        )
        print("Insertion de mesures en cours...cela peut prendre un peu de temps")
        # On ajoute les mesures
        read_csv_file(
            "data/csv/Mesures.csv",
            ";",
            "insert into Mesures values (?, ?, ?, ?, ?)",
            ["code_insee_departement", "date_obs", "tmin", "tmax", "tmoy"],
        )

        # Loop through the Isolation.csv to perform insertions

        read_csv_file_heritage(
            "data/csv/Isolation.csv",
            ";",
            "INSERT INTO Travaux (cout_total, cout_induit, annee_travaux, type_logement, annee_construction, code_region, code_departement) VALUES (?, ?, ?, ?, ?, ?, ?)",
            [
                "cout_total_ht",
                "cout_induit_ht",
                "annee_travaux",
                "type_logement",
                "annee_construction",
                "code_region",
                "code_departement",
            ],
            "INSERT INTO Isolation (id_isolation, type_poste, type_isolant, epaisseur_isolant, surface_isolant) VALUES (?, ?, ?, ?, ?)",
            ["poste_isolation", "isolant", "epaisseur", "surface"],
        )

        read_csv_file_heritage(
            "data/csv/Chauffage.csv",
            ";",
            "INSERT INTO Travaux (cout_total, cout_induit, annee_travaux, type_logement, annee_construction, code_region, code_departement) VALUES (?, ?, ?, ?, ?, ?, ?)",
            [
                "cout_total_ht",
                "cout_induit_ht",
                "annee_travaux",
                "type_logement",
                "annee_construction",
                "code_region",
                "code_departement",
            ],
            "INSERT INTO Chauffage (id_chauffage, energie_avant_travaux, energie_installee, type_generateur, type_chaudiere) VALUES (?, ?, ?, ?, ?)",
            ["energie_chauffage_avt_travaux", "energie_chauffage_installee", "generateur", "type_chaudiere"],
        )

        read_csv_file_heritage(
            "data/csv/Photovoltaique.csv",
            ";",
            "INSERT INTO Travaux (cout_total, cout_induit, annee_travaux, type_logement, annee_construction, code_region, code_departement) VALUES (?, ?, ?, ?, ?, ?, ?)",
            [
                "cout_total_ht",
                "cout_induit_ht",
                "annee_travaux",
                "type_logement",
                "annee_construction",
                "code_region",
                "code_departement",
            ],
            "INSERT INTO Photovoltaique (id_photovoltaique, puissance_installee, type_panneaux) VALUES (?, ?, ?)",
            ["puissance_installee", "type_panneaux"],
        )

    except Exception as e:
        print(
            "L'erreur suivante s'est produite lors de l'insertion des données : "
            + repr(e)
            + "."
        )
    else:
        data.commit()
        print("Un jeu de test a été inséré dans la base avec succès.")


# En cas de clic sur le bouton de suppression de la base
def deleteDB():
    try:
        updateDBfile(data, "data/deleteDB.sql")
    except Exception as e:
        print(
            "L'erreur suivante s'est produite lors de la destruction de la base : "
            + repr(e)
            + "."
        )
    else:
        data.commit()
        print("La base de données a été supprimée avec succès.")


def read_csv_file(csvFile, separator, query, columns):
    # Lecture du fichier CSV csvFile avec le séparateur separator
    # pour chaque ligne, exécution de query en la formatant avec les colonnes
    # columns
    df = pandas.read_csv(csvFile, sep=separator)
    df = df.where(pandas.notnull(df), None)

    cursor = data.cursor()
    for ix, row in df.iterrows():
        try:
            tab = []
            for i in range(len(columns)):
                # pour échapper les noms avec des apostrophes, on remplace dans
                # les chaines les ' par ''
                if isinstance(row[columns[i]], str):
                    row[columns[i]] = row[columns[i]].replace("'", "''")
                tab.append(row[columns[i]])

            # print(query)
            cursor.execute(query, tuple(tab))
        except IntegrityError as err:
            print(err)


def read_csv_file_heritage(csvFile, separator, query1, columns1, query2, columns2):
    # Loop through the Isolation.csv to perform insertions
    df = pandas.read_csv(csvFile, sep=separator)
    df = df.where(pandas.notnull(df), None)

    cursor = data.cursor()
    for ix, row in df.iterrows():
        try:
            tab = []
            for i in range(len(columns1)):
                if isinstance(row[columns1[i]], str):
                    row[columns1[i]] = row[columns1[i]].replace("'", "''")
                tab.append(row[columns1[i]])

            # print(query1)
            cursor.execute(query1, tuple(tab))

            id_travaux = cursor.lastrowid  # on compte sur le fait que SQLite incrémente automatiquement la clé primaire de 'Travaux',
            # on la récupère pur l'insérer dans la sous-table grâce à 'lastrowid'

            tab = []
            tab.append(id_travaux)
            for i in range(len(columns2)):
                if isinstance(row[columns2[i]], str):
                    row[columns2[i]] = row[columns2[i]].replace("'", "''")
                tab.append(row[columns2[i]])

            # print(query2)
            cursor.execute(query2, tuple(tab))


        except IntegrityError as err:
            # conserver le 'full' 
            data.rollback() # permet de retirer plus facilement qu'avec un TRIGGER les données dans 'Travaux' correspondant à des INSERT ratés dans une sous table, pour les UPDATES et les DELETE on utilise en revanche des triggers 
            print(err)
        else:
            data.commit()
