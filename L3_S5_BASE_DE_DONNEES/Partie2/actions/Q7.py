import tkinter as tk
from tkinter import ttk

from utils import db, display


class Window(tk.Toplevel):
    def __init__(self, parent):
        super().__init__(parent)

        # Définition de la taille de la fenêtre, du titre et des
        # lignes/colonnes de l'affichage grid
        display.centerWindow(600, 250, self)
        self.title("Q7 : gérer les travaux de rénovation")
        display.defineGridDisplay(self, 2, 3)
        ttk.Label(
            self,
            text="Ajout, Modification et Suppression",
            wraplength=500,
            anchor="center",
            font=("Helvetica", "10", "bold"),
        ).grid(columnspan=3, sticky="we", row=0)

        # Contrôles gérer l'ajout, modification et suppression pour un type de
        # travaux"
        ttk.Button(self, text="Ajout", command=self.Ajout).grid(
            column=0, row=1, pady=5)
        ttk.Button(self, text="Modification", command=self.Modification).grid(
            column=1, row=1, pady=5
        )
        ttk.Button(self, text="Suppression", command=self.Suppression).grid(
            column=2, row=1, pady=5
        )

    def Ajout(self):
        window = window_ajout(self)
        window.grab_set()

    def Modification(self):
        window = window_modification(self)
        window.grab_set()

    def Suppression(self):
        window = window_suppression(self)
        window.grab_set()


class window_ajout(tk.Toplevel):
    def __init__(self, parent):
        super().__init__(parent)

        # Définition de la taille de la fenêtre, du titre et des
        # lignes/colonnes de l'affichage grid
        display.centerWindow(600, 250, self)
        self.title("Ajout")
        display.defineGridDisplay(self, 3, 3)

        # Liste des régions
        fich_sugg="data/suggestions.txt"
        list_fichiers = lire_fichier_en_liste(fich_sugg)

        # Contrôles pour l'ajout d'un type de travaux
        ttk.Label(
            self,
            text=f"Mise à jour de Base de Données:\nSi vous souhaitez avoir des suggestions des chemains de vos fichiers (de mise à jour)\nPenser à rajouter le chemin vers vos fichiers dans le fichier: '{fich_sugg}'",
            font=("Helvetica", "11", "bold"),
        ).grid(row=0, columnspan=3, pady=15)

        ttk.Label(self, text="Fichier (adresse):").grid(row=1, column=0)
        self.input = ttk.Combobox(self, values=list_fichiers)
        self.input.grid(row=1, column=1)
        self.input.bind("<<ComboboxSelected>>", self.miseAjour)
        self.input.bind("<Return>", self.miseAjour)
        ttk.Button(self, text="Valider", command=self.miseAjour).grid(
            row=1, column=2)

        # On place un label sans texte, il servira à afficher les erreurs
        self.errorLabel = ttk.Label(
            self, anchor="center", font=("Helvetica", "10", "bold")
        )
        self.errorLabel.grid(columnspan=3, row=2, sticky="we")

    def miseAjour(self, event=None):
        add_fichier = self.input.get()  # Récupération des données saisies

        # Si la saisie est vide ou incorrecte, on affiche une erreur
        if not add_fichier or not add_fichier.endswith(".sql"):
            self.errorLabel.config(
                foreground="red",
                text="Veuillez saisir une adresse de fichier au format: sql !",
            )

        else:  # Mise à jour de la base de données
            self.errorLabel.config(
                text=""
            )  # On efface l'affichage précedent (le cas échéant)

            try:
                db.updateDBfile(db.data, add_fichier)
            except Exception as e:
                self.errorLabel.config(
                    foreground="red", text="Erreur : " + repr(e))
            else:
                db.data.commit()
                self.errorLabel.config(
                    foreground="green", text="Succès mise à jour ! \n"
                )

                # On ferme la fenêtre d'ajout apres 2 secondes
                self.after(2000, self.destroy)

class window_modification(tk.Toplevel):
    def __init__(self, parent):
        super().__init__(parent)

        # Définition de la taille de la fenêtre, du titre et des
        # lignes/colonnes de l'affichage grid
        display.centerWindow(600, 400, self)
        self.title("Modification")
        display.defineGridDisplay(self, 4, 3)

        # Contrôles pour l'ajout d'un type de travaux
        ttk.Label(
            self,
            text='Modification de Base de Données:\nTapez votre requete SQL de modification ci-dessous !',
            font=("Helvetica", "11", "bold"),
        ).grid(row=0, columnspan=3, pady=15)

        # Créer un widget Text pour une saisie multi-lignes
        self.input = tk.Text(self, wrap="word", height=8, width=50, bg="lightyellow", font=("Arial", 12))
        self.input.grid(row=1, column=1, padx=10, pady=10)

        # Bouton de modification
        ttk.Button(self, text="Valider", command=self.modification).grid(row=2, column=1)

        # On place un label sans texte, il servira à afficher les erreurs
        self.errorLabel = ttk.Label(
            self, anchor="center", font=("Helvetica", "10", "bold")
        )
        self.errorLabel.grid(columnspan=3, row=3, sticky="we")


    def modification(self, event=None):
        # Récupération des données saisies
        requete = self.input.get("1.0", tk.END).strip()  # "1.0" indique le début (ligne 1, caractère 0)

        # Si la saisie est vide ou incorrecte, on affiche une erreur
        if not requete :
            self.errorLabel.config(
                foreground="red",
                text="Veuillez saisir une requette sql !",
            )

        else:  
             # On efface l'affichage précedent (le cas échéant)
            self.errorLabel.config( text="" ) 

            try:
               db.data.cursor().execute(requete)
                    
            except Exception as e:
                self.errorLabel.config(
                    foreground="red", text="Erreur : " + repr(e))
            else:
                db.data.commit()
                self.errorLabel.config(
                    foreground="green", text="Succès modification ! \n"
                )

                # On ferme la fenêtre d'ajout apres 2 secondes
                self.after(2000, self.destroy)

class window_suppression(tk.Toplevel):
    def __init__(self, parent):
        super().__init__(parent)

        # Définition de la taille de la fenêtre, du titre et des
        # lignes/colonnes de l'affichage grid
        display.centerWindow(600, 250, self)
        self.title("Suppression (table)")
        display.defineGridDisplay(self, 3, 3)

        # Récupération des noms des tables
        table = lire_fichier_en_liste("data/tables.txt")

        # Contrôles pour l'ajout d'un type de travaux
        ttk.Label(
            self,
            text='Supression d\'une table:\nChoisez la table à supprimer dans la base de données\ndans le menu déroulant ci-dessous !',
            font=("Helvetica", "11", "bold"),
        ).grid(row=0, columnspan=3, pady=15)

        self.input = ttk.Combobox(self, values=table)
        self.input.grid(row=1, column=1)
        self.input.bind("<<ComboboxSelected>>", self.suppression)
        self.input.bind("<Return>", self.suppression)
      
        # On place un label sans texte, il servira à afficher les erreurs
        self.errorLabel = ttk.Label(
            self, anchor="center", font=("Helvetica", "10", "bold")
        )
        self.errorLabel.grid(columnspan=3, row=2, sticky="we")

    def suppression(self, event=None):
        # Récupération des données saisies
        table = self.input.get()

        # Si la saisie est vide ou incorrecte, on affiche une erreur
        if not table :
            self.errorLabel.config(
                foreground="red",
                text="Veuillez choisir une table!",
            )

        else:  
             # On efface l'affichage précedent (le cas échéant)
            self.errorLabel.config( text="" )

            requete = f"DROP TABLE {table};"

            try:
               db.data.cursor().execute(requete)
                    
            except Exception as e:
                self.errorLabel.config(
                    foreground="red", text="Erreur : " + repr(e))
            else:
                db.data.commit()
                self.errorLabel.config(
                    foreground="green", text="Succès suppression ! \n"
                )

                # On ferme la fenêtre d'ajout apres 2 secondes
                self.after(2000, self.destroy)

def lire_fichier_en_liste(nom_fichier):
    """
    Lit un fichier et retourne une liste contenant ses lignes.

    :param nom_fichier: Chemin du fichier à lire.
    :return: Liste des lignes du fichier (sans les sauts de ligne).
    """
    try:
        with open(nom_fichier, "r", encoding="utf-8") as fichier:
            lignes = [
                ligne.strip() for ligne in fichier
            ]  # Suppression des espaces inutiles et des sauts de ligne
        fichier.close()  # Fermeture du fichier
        return lignes
    except FileNotFoundError:
        print(f"Erreur : Le fichier '{nom_fichier}' est introuvable.")
        return []
    except Exception as e:
        print(f"Une erreur s'est produite : {e}")
        return []
