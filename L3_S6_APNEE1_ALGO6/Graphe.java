import java.util.*;
import java.io.*;

class Graphe {

    // Tableau de taille n, avec n le nombre de sommets du graphe.
    // Pour i allant de 0 à n-1, sommets[i] contient l'ensemble des arcs ayant pour source le noeud (i+1), dans une liste chaînée. 
    Maillon [] sommets;

    Graphe(InputStream in) throws Exception {
        lire(in);
    }

    void lire(InputStream in) throws Exception {
        Scanner s;
        int nombre_sommets;
        String specification_arc;
        String [] parties;
        int numero, source, destination, etiquette;

        s = new Scanner(in);
        nombre_sommets = s.nextInt();
        sommets = new Maillon[nombre_sommets];

        while (s.hasNext()) {
            specification_arc = s.next();
            if (!specification_arc.matches(
                        "[0-9]+/[0-9]+\\+[0-9]+/->-?[0-9]+"))
                throw new Exception("Arc mal formé : " + specification_arc);

            parties = specification_arc.split("/", 2);
            numero = Integer.valueOf(parties[0]);
            parties = parties[1].split("\\+", 2);
            source = Integer.valueOf(parties[0]) - 1;
            parties = parties[1].split("/->", 2);
            destination = Integer.valueOf(parties[0]) - 1;
            etiquette = Integer.valueOf(parties[1]);

            Maillon nouveau, courant;
            nouveau = new Maillon();
            nouveau.arc = new Arc(numero, source, destination,
                    new Etiquette(etiquette));
            nouveau.suivant = null;
            if (sommets[source] == null) {
                sommets[source] = nouveau;
            } else {
                courant = sommets[source];
                while (courant.suivant != null)
                    courant = courant.suivant;
                courant.suivant = nouveau;
            }
        }
    }

    public String toString() {
        String resultat;

        resultat = sommets.length + "\n";
        for (int i=0; i<sommets.length; i++) {
            Maillon courant;

            courant = sommets[i]; 
            while (courant != null) {
                resultat += courant.arc + "\n";
                courant = courant.suivant;
            }
        }

        return resultat;
    }



    // Retourne le nombre de sommets dans le graphe
    public int nombreSommets(){
        return sommets.length;
    }

    // Retourne le nombre d'arcs dans le graphe
    public int nombreArcs() {
        int nbArcs = 0;
        for (Maillon m: sommets) {
            while(m != null) {
                m = m.suivant;
                nbArcs++;
            }
        }
        return nbArcs;
    }


    // Cherche si un arc ayant pour source sommetSource a pour destination sommetDest
    // S'il est trouvé, le renvoie. Sinon, renvoie null.
    // Utilisé par d'autres méthodes.
    private Arc chercheArcVers(int sommetSource, int sommetDest) {
        Maillon actuel = sommets[sommetSource];
        while (actuel != null) {
            Arc candidat = actuel.arc;
            if (candidat.destination == sommetDest) return candidat;
            actuel = actuel.suivant;
        }
        return null;
    }

    // Prend en entrée sommet1 et sommet2, deux entiers correspondant à deux sommets.
    // Renvoie vrai s'il existe un arc reliant sommet1 et sommet2
    // Renvoie faux sinon
    // Renvoie également faux si un de entiers est invalide (<0 ou >= nbrSommets)
    public boolean adjacents(int sommet1, int sommet2) {
        if (sommet1 <0 || sommet2 < 0 || sommet1 >= nombreSommets() || sommet2 >=nombreSommets()) {
            return false;
        }

        // Cherche, pour les deux sommets, s'ils possèdent un arc les reliant.
        if (chercheArcVers(sommet1, sommet2) != null || chercheArcVers(sommet2, sommet1) != null) {
            return true;
        }

        // Si aucun arc ne correspond, renvoie faux
        return false;
    }

    // Prend en entrée sommet1 et sommet2, deux entiers correspondant à deux sommets.
    // Si il existe, renvoie l'arc les reliant.
    // Sinon, renvoie null.
    public Arc arcEntre(int sommet1, int sommet2) {
        if (adjacents(sommet1, sommet2)) {
            Arc candidat = chercheArcVers(sommet1, sommet2);
            if (candidat != null) return candidat;
            else return chercheArcVers(sommet2, sommet1); 
        }
        return null;
    }


    // Renvoie un tableau contenant les successeurs du sommet en entrée.
    public int[] successeurs(int sommet) {
        int successeurs[] = new int[0];
        // Si le sommet en entrée n'est pas valide, retourne un tableau vide.
        if (sommet < 0 || sommet >= nombreSommets()) return successeurs;

        // Sinon, vérifie pour tous les sommets s'ils sont adjacents, et compte le nombre trouvés.
        int nbAdjacents = 0;
        boolean adjacents[] = new boolean[nombreSommets()];
        for (int sommet2 = 0; sommet2 < nombreSommets(); sommet2++) {
            if (adjacents(sommet, sommet2)) {
                adjacents[sommet2] = true;
                nbAdjacents++;
            }
            else adjacents[sommet2] = false;
        }
        successeurs = new int[nbAdjacents];
        int indexSuccesseurs = 0;
        for (int sommet2 = 0; sommet2 < nombreSommets(); sommet2++) {
            if(adjacents[sommet2]) {
                successeurs[indexSuccesseurs] = sommet2;
                indexSuccesseurs++;
            }
        }
        return successeurs;
    }


    // Renvoie un tableau contenant les arcs du graphe
    public Arc[] arcs(){
        Arc arcs[] = new Arc[nombreArcs()];
        int index = 0;
        for (Maillon m : sommets) {
            while (m != null) {
                arcs[index] = m.arc;
                index++;
                m = m.suivant;
            }
        }
        return arcs;
    }

   
   
    /**
     * Retourne le degré d'un sommet.
     * Le degré correspond au nombre d'arcs incidents à ce sommet.
     *
     * @param sommet Le numéro du sommet (0 à n-1)
     * @return Le degré du sommet
     */
    public int degre(int sommet) {        
        // On utilise `successeurs(sommet).length` qui fait déjà ce travail
        // en vérifiant l'adjacence dans les deux sens.
        return successeurs(sommet).length;
    }

    /**
     * Vérifie si deux arcs sont indépendants.
     * Deux arcs sont indépendants s'ils n'ont aucun sommet en commun.
     *
     * @param a1 Le premier arc
     * @param a2 Le deuxième arc
     * @return true si les arcs sont indépendants, false sinon
     */
    public boolean arcsIndependants(Arc a1, Arc a2) {
        if (a1 == null || a2 == null) return false;
        // On vérifie si une extrémité de a1 touche une extrémité de a2
        return (a1.source != a2.source && a1.source != a2.destination &&
                a1.destination != a2.source && a1.destination != a2.destination);
    }

    /**
     * Vérifie si une liste d'arcs constitue un couplage valide.
     * Un couplage est un ensemble d'arcs deux à deux indépendants.
     * (Chaque sommet apparaît au plus une fois).
     *
     * @param candidat La liste d'arcs à vérifier
     * @return true si c'est un couplage, false sinon
     */
    public boolean estCouplage(List<Arc> candidat) {
        if (candidat == null) return false;
        
        // Tableau pour marquer les sommets visités
        boolean[] vus = new boolean[nombreSommets()];
        
        for (Arc a : candidat) {
            if (a.source < 0 || a.source >= nombreSommets() ||
                a.destination < 0 || a.destination >= nombreSommets()) {
                return false; // Sommets invalides
            }
            
            // Si l'un des sommets est déjà couvert, ce n'est pas un couplage
            if (vus[a.source] || vus[a.destination]) {
                return false;
            }
            
            vus[a.source] = true;
            vus[a.destination] = true;
        }
        return true;
    }

    /**
     * Recherche un couplage parfait dans le graphe.
     * Utilise une heuristique gloutonne avec backtracking :
     * 1. Traite en priorité les sommets de degré 1 (choix forcé).
     * 2. Sinon, choisit un sommet de faible degré et teste ses voisins (triés par degré).
     *
     * @return La liste des arcs du couplage parfait, ou null si aucun n'existe.
     */
    public List<Arc> rechercherCouplage() {
        // Si le nombre de sommets est impair, impossible d'avoir un couplage parfait
        // if (nombreSommets() % 2 != 0) return null;

        // Pré-calcul des degrés pour optimiser
        int[] degresDynamiques = new int[nombreSommets()];
        for (int i = 0; i < nombreSommets(); i++) {
            degresDynamiques[i] = degre(i);
        }

        // Tableau pour savoir si un sommet est couvert par le couplage courant
        boolean[] couverts = new boolean[nombreSommets()];
        
        // Liste résultat
        List<Arc> couplage = new ArrayList<>();
        
        if (backtrackCouplage(couplage, couverts, degresDynamiques, 0)) {
            return couplage;
        } else {
            return null;
        }
    }

    /**
     * Fonction récursive pour construire le couplage. 
     * 
     * @param couplage Liste des arcs choisis jusqu'ici
     * @param couverts État des sommets (true si couvert)
     * @param degres Tableau des degrés résiduels (nombre de voisins non couverts)
     * @param nbCouverts Nombre total de sommets couverts
     * @return {@code true} si un couplage parfait est trouvé
     */
    private boolean backtrackCouplage(List<Arc> couplage, boolean[] couverts, int[] degres, int nbCouverts) {
        int n = nombreSommets();
        
        // Cas de base : tous les sommets sont couverts -> Succès
        if (nbCouverts == n) return true;

        // Étape 1 : Rechercher un "Forced Move" (Sommet de degré 1)
        // Si un sommet non couvert a un seul voisin non couvert, on DOIT prendre cette arête.
        int sommetForce = -1;
        for (int i = 0; i < n; i++) {
            if (!couverts[i]) {
                if (degres[i] == 0) return false; // Impasse : sommet isolé non couvert
                if (degres[i] == 1) {
                    sommetForce = i;
                    break; 
                }
            }
        }

        if (sommetForce != -1) {
            // Application du choix forcé
            int u = sommetForce;
            // Trouver l'unique voisin v disponible
            int v = trouverVoisinDisponible(u, couverts);
            
            if (v == -1) return false; // Ne devrait pas arriver si degres[u] == 1

            return tenterAjoutArc(u, v, couplage, couverts, degres, nbCouverts);
        }

        // Étape 2 : Heuristique (Pas de choix forcé)
        // On choisit le sommet non couvert avec le degré le plus faible
        int u = -1;
        int minDeg = Integer.MAX_VALUE;
        for (int i = 0; i < n; i++) {
            if (!couverts[i]) {
                if (degres[i] < minDeg) {
                    minDeg = degres[i];
                    u = i;
                }
            }
        }

        if (u == -1) return false; // Ne devrait pas arriver si nbCouverts < n

        // On récupère les voisins disponibles de u
        List<Integer> voisins = new ArrayList<>();
        for (int v : successeurs(u)) { // Note: successeurs() peut être lent, on pourrait optimiser
            if (!couverts[v]) {
                voisins.add(v);
            }
        }

        // On trie les voisins : on essaie d'abord ceux qui ont le plus petit degré résiduel
        // Cela maximise les chances de créer des contraintes (degré 1) pour la suite
        final int[] d = degres;
        Collections.sort(voisins, new Comparator<Integer>() {
            public int compare(Integer a, Integer b) {
                return Integer.compare(d[a], d[b]);
            }
        });

        // Branching : on essaie chaque voisin
        for (int v : voisins) {
            if (tenterAjoutArc(u, v, couplage, couverts, degres, nbCouverts)) {
                return true;
            }
        }

        return false;
    }

    /**
     * Tente d'ajouter l'arc (u, v) au couplage et continue la récursion.
     * Gère la mise à jour des états (couverts, degrés) et le backtracking.
     */
    private boolean tenterAjoutArc(int u, int v, List<Arc> couplage, boolean[] couverts, int[] degres, int nbCouverts) {
        // 1. Récupérer l'arc (ici on utilise arcEntre qui retourne l'objet réel)
        Arc arc = arcEntre(u, v);
        if (arc == null) return false; // Erreur 

        // 2. Mettre à jour l'état (Avancer)
        couplage.add(arc);
        couverts[u] = true;
        couverts[v] = true;
        majDegres(u, couverts, degres, -1); // Décrémente les voisins de u
        majDegres(v, couverts, degres, -1); // Décrémente les voisins de v

        // NOTIFICATION GUI
        notifierAjout(u, v);

        // 3. Récursion
        if (backtrackCouplage(couplage, couverts, degres, nbCouverts + 2)) {
            return true;
        }

        // 4. Backtracking (Reculer) -> Annuler les changements
        // NOTIFICATION GUI (Avant de retirer logiciellement pour visualiser l'annulation)
        notifierRetrait(u, v);

        majDegres(v, couverts, degres, +1);
        majDegres(u, couverts, degres, +1);
        couverts[v] = false;
        couverts[u] = false;
        couplage.remove(couplage.size() - 1);

        return false;
    }

    /**
     * Met à jour les degrés des voisins d'un sommet qui vient d'être couvert ou découvert.
     * @param sommet Le sommet dont on regarde les voisins
     * @param couverts État des sommets
     * @param degres Tableau des degrés à mettre à jour
     * @param delta -1 pour décrémenter (sommet couvert), +1 pour incrémenter (backtrack)
     */
    private void majDegres(int sommet, boolean[] couverts, int[] degres, int delta) {
        // On parcourt tous les voisins potentiels du graphe initial
        // Note: successeurs() recalcule tout, ce qui est lent. 
        // Pour une version très optimisée, on pré-calculerait la liste d'adjacence.
        // Ici on fait avec les méthodes existantes pour rester simple.
        int[] voisins = successeurs(sommet);
        for (int voisin : voisins) {
            if (!couverts[voisin]) {
                degres[voisin] += delta;
            }
        }
    }

    /**
     * Trouve le premier voisin non couvert d'un sommet.
     */
    private int trouverVoisinDisponible(int u, boolean[] couverts) {
        int[] voisins = successeurs(u);
        for (int v : voisins) {
            if (!couverts[v]) return v;
        }
        return -1;
    }

    
    
    // --- PARTIE OBSERVATEUR POUR GUI ---
    private GrapheObserver observer;

    public void setObserver(GrapheObserver obs) {
        this.observer = obs;
    }

    private void notifierAjout(int u, int v) {
        if (observer != null) {
            observer.onArcAjoute(u, v);
            observer.refresh();
            dormir();
        }
    }

    private void notifierRetrait(int u, int v) {
        if (observer != null) {
            observer.onArcRetire(u, v);
            observer.refresh();
            dormir();
        }
    }

    private void dormir() {
        if (observer != null) {
            try {
                Thread.sleep(observer.getDelay());
            } catch (InterruptedException e) {
                // Ignore
            }
        }
    }
    // -----------------------------------

    
}
