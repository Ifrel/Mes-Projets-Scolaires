CREATE TABLE Departements (
    code_departement TEXT,
    nom_departement TEXT,
    code_region INTEGER,
    zone_climatique TEXT,
    CONSTRAINT pk_departements PRIMARY KEY (code_departement),
    CONSTRAINT fk_region FOREIGN KEY (code_region) REFERENCES Regions(code_region) ON DELETE CASCADE
);

CREATE TABLE Regions (
    code_region INTEGER,
    nom_region TEXT,
    CONSTRAINT pk_regions PRIMARY KEY (code_region) 
);

CREATE TABLE Mesures (
    code_departement TEXT,
    date_mesure DATE,
    temperature_min_mesure FLOAT,
    temperature_max_mesure FLOAT,
    temperature_moy_mesure FLOAT,
    CONSTRAINT pk_mesures PRIMARY KEY (code_departement, date_mesure),
    CONSTRAINT fk_mesures FOREIGN KEY (code_departement) REFERENCES Departements(code_departement) ON DELETE CASCADE
);

--TODO Q4 Ajouter les créations des nouvelles tables

-- On choisit de créer 4 tables plutôt que 3 (Isolation, Chauffage, Photovoltaique) possibles grâce au 'full', 
-- on préfère ne pas recopier les attributs de 'Travaux' dans chaque 'sous-relation' pour réduire l'espace occupé
-- une jointure est donc nécessaire pour accéder au contenu de Isolation/Chauffage/Photovoltaique


-- ON CONSIDERE QUE LES ATTRIBUTS QUI SONT D'UN TYPE ENUMERE NE PEUVENT PAS ETRE NULL.
-- ON SE BASE SUR LE MODELE UML ET ON IGNORE TOUTES LES VALEURS ANORMALES DU GENRE " Chaudiere." DANS type_chaudiere, ETC.

CREATE TABLE Travaux (
  id_travaux INTEGER PRIMARY KEY, -- devrait normalement s'auto-incrémenter automatiquement si on ne passe rien en insertion
  cout_total FLOAT,
  cout_induit FLOAT,
  annee_travaux TEXT,
  type_logement TEXT,
  annee_construction TEXT,
  code_region INTEGER,
  code_departement TEXT,
  CONSTRAINT fk1_travaux FOREIGN KEY(code_region) REFERENCES Regions(code_region) ON DELETE CASCADE,
  CONSTRAINT fk2_travaux FOREIGN KEY(code_departement) REFERENCES Departements(code_departement) ON DELETE CASCADE
);

--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

CREATE TABLE Isolation (
  id_isolation INTEGER PRIMARY KEY,
  type_poste TEXT NOT NULL,
  type_isolant TEXT NOT NULL,
  epaisseur_isolant INTEGER,
  surface_isolant FLOAT,
  CONSTRAINT fk_isolation FOREIGN KEY(id_isolation) REFERENCES Travaux(id_travaux) ON DELETE CASCADE,
  CONSTRAINT c1_isolation CHECK(type_poste = 'COMBLES PERDUES' OR type_poste = 'ITI' OR type_poste = 'ITE' OR type_poste = 'RAMPANTS' OR type_poste = 'SARKING' OR type_poste = 'TOITURE TERRASSE' OR type_poste = 'PLANCHER BAS'),
  CONSTRAINT c2_isolation CHECK(type_isolant = 'AUTRES' OR type_isolant = 'LAINE VEGETALE' OR type_isolant = 'LAINE MINERALE' OR type_isolant = 'PLASTIQUES')
);

CREATE TRIGGER UpIsol
  AFTER UPDATE ON Travaux
  BEGIN
    UPDATE Isolation SET id_isolation = NEW.id_travaux WHERE id_isolation = OLD.id_travaux;
  END;

CREATE TRIGGER DelTravIsol
  AFTER DELETE ON Travaux
  BEGIN
    DELETE FROM Isolation WHERE id_isolation = OLD.id_travaux;
  END;

CREATE TRIGGER DelIsolTrav
  AFTER DELETE ON Isolation
  BEGIN
    DELETE FROM Travaux WHERE id_travaux = OLD.id_isolation;
  END;

--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

CREATE TABLE Chauffage (
  id_chauffage INTEGER PRIMARY KEY,
  energie_avant_travaux TEXT NOT NULL,
  energie_installee TEXT NOT NULL,
  type_generateur TEXT NOT NULL,
  type_chaudiere TEXT NOT NULL,
  CONSTRAINT fk_chauffage FOREIGN KEY(id_chauffage) REFERENCES Travaux(id_travaux) ON DELETE CASCADE,
  CONSTRAINT c1_chauffage CHECK(energie_avant_travaux = 'AUTRES' OR energie_avant_travaux = 'BOIS' OR energie_avant_travaux = 'ELECTRICITE' OR energie_avant_travaux = 'FIOUL' OR energie_avant_travaux = 'GAZ'),
  CONSTRAINT c2_chauffage CHECK(energie_installee = 'AUTRES' OR energie_installee = 'BOIS' OR energie_installee = 'ELECTRICITE' OR energie_installee = 'FIOUL' OR energie_installee = 'GAZ')
  CONSTRAINT c3_chauffage CHECK(type_generateur = 'AUTRES' OR type_generateur = 'CHAUDIERE' OR type_generateur = 'INSERT' OR type_generateur = 'PAC' OR type_generateur = 'POELE' OR type_generateur = 'RADIATEUR'),
  CONSTRAINT c4_chauffage CHECK(type_chaudiere = 'STANDARD' OR type_chaudiere = 'AIR-EAU' OR type_chaudiere = 'A CONDENSATION' OR type_chaudiere = 'AUTRES' OR type_chaudiere = 'AIR-AIR' OR type_chaudiere = 'GEOTHERMIE' OR type_chaudiere = 'HPE')
);

CREATE TRIGGER UpChauff
  AFTER UPDATE ON Travaux
  BEGIN
    UPDATE Chauffage SET id_chauffage = NEW.id_travaux WHERE id_chauffage = OLD.id_travaux;
  END;

CREATE TRIGGER DelTravChauff
  AFTER DELETE ON Travaux
  BEGIN
    DELETE FROM Chauffage WHERE id_chauffage = OLD.id_travaux;
  END;

CREATE TRIGGER DelChauffTrav
  AFTER DELETE ON Chauffage
  BEGIN
    DELETE FROM Travaux WHERE id_travaux = OLD.id_chauffage;
  END;

--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

CREATE TABLE Photovoltaique (
  id_photovoltaique INTEGER PRIMARY KEY,
  puissance_installee INTEGER,
  type_panneaux TEXT NOT NULL,
  CONSTRAINT fk_photovoltaique FOREIGN KEY(id_photovoltaique) REFERENCES Travaux(id_travaux) ON DELETE CASCADE,
  CONSTRAINT c_photovoltaique CHECK(type_panneaux = 'MONOCRISTALLIN' OR type_panneaux = 'POLYCRISTALLIN')
);

CREATE TRIGGER UpPhoto
  AFTER UPDATE ON Travaux
  BEGIN
    UPDATE Photovoltaique SET id_photovoltaique = NEW.id_travaux WHERE id_photovoltaique = OLD.id_travaux;
  END;

CREATE TRIGGER DelTravPhoto
  AFTER DELETE ON Travaux
  BEGIN
    DELETE FROM Photovoltaique WHERE id_photovoltaique = OLD.id_travaux;
  END;

CREATE TRIGGER DelPhotoTrav
  AFTER DELETE ON Photovoltaique
  BEGIN
    DELETE FROM Travaux WHERE id_travaux = OLD.id_photovoltaique;
  END;

