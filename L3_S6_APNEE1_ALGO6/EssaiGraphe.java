import java.io.*;
import java.util.*;

class EssaiGraphe {
    public static void main(String [] args) {
        FileInputStream f;
        Graphe g;

        if (args.length != 1) {
            System.out.println("Usage: java EssaiGraphe <fichier_graphe>");
            return;
        }

        try {
            f = new FileInputStream(args[0]);
            g = new Graphe(f);

            // Vérification des propriétés simples (nombre de sommets pair)
            if (g.nombreSommets() % 2 != 0) {
                System.out.println("Couplage impossible");
            } else {
                List<Arc> resultat = g.rechercherCouplage();
                
                if (resultat == null) {
                    System.out.println("Couplage non trouve");
                } else {
                    System.out.print("Couplage trouve : ");
                    
                    // Tri des arcs par leur numéro pour l'affichage 
                    // Le sujet demande "la liste des numéros des arêtes", l'ordre n'est pas explicitement strict 
                    // mais un tri est plus propre.
                    Collections.sort(resultat, new Comparator<Arc>() {
                        public int compare(Arc a1, Arc a2) {
                            return Integer.compare(a1.numero, a2.numero);
                        }
                    });

                    for (int i = 0; i < resultat.size(); i++) {
                        System.out.print(resultat.get(i).numero);
                        if (i < resultat.size() - 1) {
                            System.out.print(",");
                        }
                    }
                    System.out.println();
                }
            }

        } catch (Exception e) {
            System.out.println(e);
            e.printStackTrace();
        }
    }
}
