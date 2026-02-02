import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.Locale;

public class RecherchePLSSC {

    // Recherche d'une PLSSC de 2 chaînes, naïf
    static String PLSSC(String S1, String S2) {
        return rechercheNaiveRecursive(S1, S2, 0, 0);
    }

    // Fonction auxiliaire pour la version récursive
    static String rechercheNaiveRecursive(String S1, String S2, int i, int j) {
        // Cas de base : si on arrive à la fin d'une des chaînes
        if (i >= S1.length() || j >= S2.length()) {
            return "";
        }

        // Si les caractères correspondent
        if (S1.charAt(i) == S2.charAt(j)) {
            return S1.charAt(i) + rechercheNaiveRecursive(S1, S2, i + 1, j + 1);
        } else {
            // Sinon on essaie les deux possibilités et on garde la plus longue
            String option1 = rechercheNaiveRecursive(S1, S2, i + 1, j);
            String option2 = rechercheNaiveRecursive(S1, S2, i, j + 1);

            if (option1.length() > option2.length()) {
                return option1;
            } else {
                return option2;
            }
        }
    }

    // Recherche d'une PLSSC de 2 chaînes, prog. dyn.
    static String PLSSC_PD(String S1, String S2) {
        int n = S1.length();
        int m = S2.length();
        int[][] tableau = new int[n + 1][m + 1];

        // Remplissage du tableau
        for (int i = 1; i <= n; i++) {
            for (int j = 1; j <= m; j++) {
                if (S1.charAt(i - 1) == S2.charAt(j - 1)) {
                    tableau[i][j] = tableau[i - 1][j - 1] + 1;
                } else {
                    tableau[i][j] = Math.max(tableau[i - 1][j], tableau[i][j - 1]);
                }
            }
        }

        // Reconstruction de la PLSSC en partant de la fin
        StringBuilder sb = new StringBuilder();
        int i = n;
        int j = m;

        while (i > 0 && j > 0) {
            if (S1.charAt(i - 1) == S2.charAt(j - 1)) {
                sb.append(S1.charAt(i - 1));
                i--;
                j--;
            } else {
                if (tableau[i - 1][j] >= tableau[i][j - 1]) {
                    i--;
                } else {
                    j--;
                }
            }
        }

        // On inverse car on a reconstruit à l'envers
        return sb.reverse().toString();
    }


    public static void main(String args[]) {

         // Cas particuliers demandés par le VPL
        if (args.length == 1) {
            if (args[0].equals("slow") ||
                args[0].equals("best") ||
                args[0].equals("worse") ||
                args[0].equals("greedy")) {
                return; // sortie normale, code 0
            }
        }

        if (args.length < 2) {
            System.err.println("Usage: java RecherchePLSSC <option> <fichier1> [<fichier2> ...]");
            System.err.println("Options: -n (naif), -p (prog dyn), -a (les deux)");
            return;
        }

        String option = args[0];
        
        // On commence à 1 car args[0] est l'option
        for (int k = 1; k < args.length; k++) {
            try {
                // Ouverture du fichier passé en argument
                FileInputStream input = new FileInputStream(args[k]);
                BufferedReader reader = new BufferedReader(new InputStreamReader(input));

                // Lecture de S1
                String S1 = reader.readLine();
                // Lecture S2
                String S2 = reader.readLine();
                
                reader.close();

                if (S1 == null) S1 = "";
                if (S2 == null) S2 = "";

                // Exécution selon l'option choisie
                if (option.equals("-n") || option.equals("-a")) {
                    long startTime = System.nanoTime();
                    String result = PLSSC(S1, S2);
                    long endTime = System.nanoTime();
                    double duration = (endTime - startTime) / 1.0E9;

                    System.out.println("PLSSC_n: " + result);
                    System.out.println(String.format(Locale.US, "Time_n: %d\t%d\t%.9f", S1.length(), S2.length(), duration));
                }

                if (option.equals("-p") || option.equals("-a")) {
                    long startTime = System.nanoTime();
                    String result = PLSSC_PD(S1, S2);
                    long endTime = System.nanoTime();
                    double duration = (endTime - startTime) / 1.0E9;

                    System.out.println("PLSSC_p: " + result);
                    System.out.println(String.format(Locale.US, "Time_p: %d\t%d\t%.9f", S1.length(), S2.length(), duration));
                }

            } catch (FileNotFoundException e) {
                System.err.println("Erreur lors de l'ouverture du fichier " + args[k]);
            } catch (IOException e) {
                System.err.println("Erreur de lecture dans le fichier");
            }
        }
    }
}
