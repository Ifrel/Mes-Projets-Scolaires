import tkinter as tk
from datetime import datetime
from tkinter import ttk

from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
from matplotlib.figure import Figure

from utils import db, display


class Window(tk.Toplevel):
    def __init__(self, parent):
        super().__init__(parent)

        # Définition de la taille de la fenêtre, du titre et des
        # lignes/colonnes de l'affichage grid
        display.centerWindow(1000, 600, self)
        self.title(
            "Q6 : Records de températures historiques pour la zone H1 en 2018")
        display.defineGridDisplay(self, 2, 1)
        # ttk.Label(
        #     self,
        #     text=(
        #         "On souhaite tracer un graphique pour comparer les températures des départements de la zone "
        #         "climatique H1 en 2018 avec les records de températures historiques enregistrés dans notre base "
        #         "de données pour l’ensemble du pays, pour chaque jour de l’année.\n\n"
        #         "Pour l’ensemble de cet exercice, seules les données de la colonne temperature_moy_mesure de "
        #         "la table Mesures seront prises en compte.\n\n"
        #         "On souhaite afficher ces données sur le même graphique, avec les 4 courbes suivantes :\n"
        #         "    - Les records de fraîcheur historiques pour chaque jour de l’année (toutes années confondues, toutes zones climatiques confondues).\n"
        #         "    - Les records de chaleur historiques pour chaque jour de l’année (toutes années confondues, toutes zones climatiques confondues).\n"
        #         "    - Les températures du département le plus froid de la zone H1 pour chaque jour de l’année 2018.\n"
        #         "    - Les températures du département le plus chaud de la zone H1 pour chaque jour de l’année 2018.\n\n"
        #         "Les départements les plus froids et les plus chauds de la zone H1 sont ceux pour lesquels la "
        #         "moyenne de leurs températures sur l’année 2018 est respectivement la plus basse et la plus élevée.\n\n"
        #         "Pour tracer le graphique, basez-vous sur le code fourni en exemple dans F4. Attention, seule la "
        #         "requête SQL doit être modifiée dans le code que vous reprendrez de F4. Vous ne devez pas modifier "
        #         "le code de génération du graphique.\n\n"
        #         "Indication : travaillez indépendamment sur chaque courbe demandée. Le plus difficile sera de rassembler "
        #         "les données nécessaires pour tracer les 4 courbes dans une même requête."
        #     ),
        #     wraplength=700,
        #     anchor="center",
        #     font=('Helvetica', '10', 'bold')
        # ).grid(sticky="we", row=0)

        # Pour générer le graphique à partir des données obtenues à partir de
        # la requête SQL
        query = """
                WITH
                    -- Records historiques de fraîcheur pour chaque jour de l'année
                    RecordsFroid AS (
                        SELECT
                            date_mesure AS jour,
                            MIN(temperature_moy_mesure) AS record_froid
                        FROM Mesures
                        GROUP BY jour
                    ),

                    -- Records historiques de chaleur pour chaque jour de l'année
                    RecordsChaleur AS (
                        SELECT
                            date_mesure AS jour,
                            MAX(temperature_moy_mesure) AS record_chaud
                        FROM Mesures
                        GROUP BY jour
                    ),

                    -- Moyenne des températures en 2018 pour chaque département de la zone H1
                    Moyennes2018H1 AS (
                        SELECT
                            code_departement,
                            AVG(temperature_moy_mesure) AS moyenne_annee
                        FROM Mesures
                        WHERE strftime('%Y', date_mesure) = '2018'
                        AND code_departement IN (
                            SELECT code_departement FROM Departements WHERE zone_climatique = 'H1'
                        )
                        GROUP BY code_departement
                    ),

                    -- Département le plus froid de la zone H1 en 2018
                    DeptFroid AS (
                        SELECT code_departement
                        FROM Moyennes2018H1
                        ORDER BY moyenne_annee ASC
                        LIMIT 1
                    ),

                    -- Département le plus chaud de la zone H1 en 2018
                    DeptChaud AS (
                        SELECT code_departement
                        FROM Moyennes2018H1
                        ORDER BY moyenne_annee DESC
                        LIMIT 1
                    ),

                    -- Températures journalières pour le département le plus froid
                    TempFroid2018 AS (
                        SELECT
                            date_mesure AS jour,
                            AVG(temperature_moy_mesure) AS temp_froid
                        FROM Mesures
                        WHERE strftime('%Y', date_mesure) = '2018'
                        AND code_departement = (SELECT code_departement FROM DeptFroid)
                        GROUP BY jour
                    ),

                    -- Températures journalières pour le département le plus chaud
                    TempChaud2018 AS (
                        SELECT
                            date_mesure AS jour,
                            AVG(temperature_moy_mesure) AS temp_chaud
                        FROM Mesures
                        WHERE strftime('%Y', date_mesure) = '2018'
                        AND code_departement = (SELECT code_departement FROM DeptChaud)
                        GROUP BY jour
                    )

                -- Final : Regroupement des données
                SELECT
                    rf.jour,
                    rf.record_froid,
                    rc.record_chaud,
                    tf.temp_froid,
                    tc.temp_chaud
                FROM RecordsFroid rf
                LEFT JOIN RecordsChaleur rc ON rf.jour = rc.jour
                LEFT JOIN TempFroid2018 tf ON rf.jour = tf.jour
                LEFT JOIN TempChaud2018 tc ON rf.jour = tc.jour
                WHERE strftime('%Y', rf.jour) = '2018'
                ORDER BY rf.jour;

            """

        # Extraction des données et affichage dans le tableau
        result = []
        try:
            cursor = db.data.cursor()
            result = cursor.execute(query)
        except Exception as e:
            print("Erreur : " + repr(e))

        # Extraction et préparation des valeurs à mettre sur le graphique
        graph1 = []
        graph2 = []
        graph3 = []
        graph4 = []
        tabx = []
        f = open("data_graph.txt", "w")
        for row in result:
            f.write(str(row) + "\n")
            tabx.append(row[0])
            graph1.append(row[1])
            graph2.append(row[2])
            graph3.append(row[3])
            graph4.append(row[4])

        # Formatage des dates pour l'affichage sur l'axe x
        datetime_dates = [datetime.strptime(date, "%Y-%m-%d") for date in tabx]

        # Ajout de la figure et du subplot qui contiendront le graphique
        fig = Figure(figsize=(15, 8), dpi=100)
        plot1 = fig.add_subplot(111)

        # Affichage des courbes
        plot1.plot(
            range(len(datetime_dates)),
            graph1,
            color="#00FFFF",
            label="records, fraîcheur historiques, (toutes années et zones climatiques confondues)",
        )
        plot1.plot(
            range(len(datetime_dates)),
            graph2,
            color="#FF8300",
            label="records, chauds historiques, (toutes années et zones climatiques confondues)",
        )
        plot1.plot(
            range(len(datetime_dates)),
            graph3,
            color="#0000FF",
            label="températures moyennes les plus froides, zone H1, par département, 2018",
        )
        plot1.plot(
            range(len(datetime_dates)),
            graph4,
            color="#FF0000",
            label="températures moyennes les plus chaudes, zone H1, par département, 2018",
        )

        # Configuration de l'axe x pour n'afficher que le premier jour de
        # chaque mois
        xticks = [i for i, date in enumerate(datetime_dates) if date.day == 1]
        xticklabels = [
            date.strftime("%Y-%m-%d") for date in datetime_dates if date.day == 1
        ]
        plot1.set_xticks(xticks)
        plot1.set_xticklabels(xticklabels, rotation=45)
        plot1.legend()

        # Affichage du graphique
        canvas = FigureCanvasTkAgg(fig, master=self)
        canvas.draw()
        canvas.get_tk_widget().pack()
