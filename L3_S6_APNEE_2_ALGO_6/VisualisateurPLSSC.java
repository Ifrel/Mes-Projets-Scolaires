import javax.swing.*;
import javax.swing.table.DefaultTableCellRenderer;
import javax.swing.table.DefaultTableModel;
import javax.swing.text.DefaultHighlighter;
import javax.swing.text.Highlighter;
import java.awt.*;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.util.List;
import java.util.Random;
import java.util.concurrent.ExecutionException;

public class VisualisateurPLSSC extends JFrame {

    // Composants UI
    private JTextField txtS1, txtS2;
    private JTextArea logArea;
    private JTable matrixTable;
    private DefaultTableModel tableModel;
    private JSlider speedSlider;
    private JLabel statusLabel;
    private JButton btnRunNaive, btnRunDP, btnGen, btnLoad;

    // Couleurs
    private static final Color COLOR_CURRENT = new Color(255, 255, 150); // Jaune
    private static final Color COLOR_MATCH = new Color(150, 255, 150);   // Vert clair
    private static final Color COLOR_PATH = new Color(100, 200, 100);    // Vert foncé (solution)
    private static final Color COLOR_MISMATCH = new Color(255, 200, 200); // Rouge clair

    // Worker en cours
    private SwingWorker<?, ?> currentWorker;

    public VisualisateurPLSSC() {
        super("Visualisateur PLSSC - Algorithmique");
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setSize(1000, 700);
        setLocationRelativeTo(null);
        setLayout(new BorderLayout());

        // --- Panel Haut : Entrées et Contrôles ---
        JPanel topPanel = new JPanel(new GridLayout(3, 1, 5, 5));
        topPanel.setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));

        // Ligne 1 : S1
        JPanel p1 = new JPanel(new BorderLayout());
        p1.add(new JLabel("Chaîne S1 : "), BorderLayout.WEST);
        txtS1 = new JTextField("ALGORITHME");
        txtS1.setFont(new Font("Monospaced", Font.BOLD, 14));
        p1.add(txtS1, BorderLayout.CENTER);
        topPanel.add(p1);

        // Ligne 2 : S2
        JPanel p2 = new JPanel(new BorderLayout());
        p2.add(new JLabel("Chaîne S2 : "), BorderLayout.WEST);
        txtS2 = new JTextField("LOGARITHME");
        txtS2.setFont(new Font("Monospaced", Font.BOLD, 14));
        p2.add(txtS2, BorderLayout.CENTER);
        topPanel.add(p2);

        // Ligne 3 : Boutons
        JPanel p3 = new JPanel(new FlowLayout(FlowLayout.CENTER));
        btnGen = new JButton("Générer Aléatoire");
        btnLoad = new JButton("Importer Fichier");
        btnRunNaive = new JButton("Lancer Naïf (Récursif)");
        btnRunDP = new JButton("Lancer Prog. Dyn.");
        JButton btnStop = new JButton("Stop");
        
        speedSlider = new JSlider(0, 500, 50); // 0ms à 500ms
        speedSlider.setInverted(true); // Plus c'est haut, plus c'est lent -> on inverse pour l'UI "Vitesse"
        speedSlider.setToolTipText("Délai (ms)");
        
        p3.add(btnGen);
        p3.add(btnLoad);
        p3.add(new JSeparator(JSeparator.VERTICAL));
        p3.add(btnRunNaive);
        p3.add(btnRunDP);
        p3.add(btnStop);
        p3.add(new JLabel("  Vitesse:"));
        p3.add(speedSlider);

        topPanel.add(p3);
        add(topPanel, BorderLayout.NORTH);

        // --- Panel Centre : Visualisation ---
        JTabbedPane tabbedPane = new JTabbedPane();

        // Onglet 1 : Matrice (pour PD)
        tableModel = new DefaultTableModel();
        matrixTable = new JTable(tableModel);
        matrixTable.setRowHeight(30);
        matrixTable.setFont(new Font("SansSerif", Font.PLAIN, 12));
        matrixTable.setDefaultRenderer(Object.class, new MatrixCellRenderer());
        matrixTable.setAutoResizeMode(JTable.AUTO_RESIZE_OFF);
        tabbedPane.addTab("Matrice (Prog. Dyn.)", new JScrollPane(matrixTable));

        // Onglet 2 : Logs / Sortie
        logArea = new JTextArea();
        logArea.setEditable(false);
        logArea.setFont(new Font("Monospaced", Font.PLAIN, 12));
        tabbedPane.addTab("Logs / Résultats", new JScrollPane(logArea));

        add(tabbedPane, BorderLayout.CENTER);

        // --- Panel Bas : Status ---
        statusLabel = new JLabel("Prêt");
        statusLabel.setBorder(BorderFactory.createEmptyBorder(5, 5, 5, 5));
        add(statusLabel, BorderLayout.SOUTH);

        // --- Listeners ---
        btnGen.addActionListener(e -> generateRandomStrings());
        btnLoad.addActionListener(e -> loadFile());
        btnRunNaive.addActionListener(e -> startNaive());
        btnRunDP.addActionListener(e -> startDP());
        btnStop.addActionListener(e -> stopWorker());
    }

    private void stopWorker() {
        if (currentWorker != null && !currentWorker.isDone()) {
            currentWorker.cancel(true);
            statusLabel.setText("Annulé par l'utilisateur.");
        }
    }

    private void generateRandomStrings() {
        String chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
        Random r = new Random();
        int len1 = 10 + r.nextInt(10);
        int len2 = 10 + r.nextInt(10);
        StringBuilder s1 = new StringBuilder();
        StringBuilder s2 = new StringBuilder();
        for (int i = 0; i < len1; i++) s1.append(chars.charAt(r.nextInt(chars.length())));
        for (int i = 0; i < len2; i++) s2.append(chars.charAt(r.nextInt(chars.length())));
        txtS1.setText(s1.toString());
        txtS2.setText(s2.toString());
    }

    private void loadFile() {
        JFileChooser fileChooser = new JFileChooser(".");
        if (fileChooser.showOpenDialog(this) == JFileChooser.APPROVE_OPTION) {
            File file = fileChooser.getSelectedFile();
            try (BufferedReader br = new BufferedReader(new FileReader(file))) {
                txtS1.setText(br.readLine());
                txtS2.setText(br.readLine());
                statusLabel.setText("Fichier chargé : " + file.getName());
            } catch (Exception ex) {
                JOptionPane.showMessageDialog(this, "Erreur lecture fichier: " + ex.getMessage());
            }
        }
    }

    private int getDelay() {
        // Slider value 0 (fast) to 500 (slow). 
        // Inverted in UI logic: Right (max) = Fast, Left (min) = Slow
        // But JSlider value is raw. Let's say right is 0 delay, left is 500 delay.
        return 500 - speedSlider.getValue();
    }

    // ==========================================
    // WORKER : NAIF
    // ==========================================
    private void startNaive() {
        stopWorker();
        String s1 = txtS1.getText();
        String s2 = txtS2.getText();
        if (s1.length() > 14 || s2.length() > 14) {
            int rep = JOptionPane.showConfirmDialog(this, "Attention, l'algo naïf est très lent pour > 14 chars.\nContinuer ?", "Avertissement", JOptionPane.YES_NO_OPTION);
            if (rep != JOptionPane.YES_OPTION) return;
        }

        logArea.setText("Début recherche naïve...\n");
        statusLabel.setText("Exécution Naïve en cours...");
        
        currentWorker = new SwingWorker<String, Void>() {
            Highlighter h1 = txtS1.getHighlighter();
            Highlighter h2 = txtS2.getHighlighter();
            Highlighter.HighlightPainter painter = new DefaultHighlighter.DefaultHighlightPainter(COLOR_CURRENT);

            @Override
            protected String doInBackground() throws Exception {
                return naiveRec(s1, s2, 0, 0);
            }

            private String naiveRec(String S1, String S2, int i, int j) throws InterruptedException {
                if (isCancelled()) return "";
                
                // Visualization logic
                final int finalI = i;
                final int finalJ = j;
                SwingUtilities.invokeLater(() -> {
                    try {
                        h1.removeAllHighlights();
                        h2.removeAllHighlights();
                        if (finalI < S1.length()) h1.addHighlight(finalI, finalI + 1, painter);
                        if (finalJ < S2.length()) h2.addHighlight(finalJ, finalJ + 1, painter);
                    } catch (Exception e) {}
                });
                
                Thread.sleep(getDelay());

                if (i >= S1.length() || j >= S2.length()) return "";

                if (S1.charAt(i) == S2.charAt(j)) {
                    return S1.charAt(i) + naiveRec(S1, S2, i + 1, j + 1);
                } else {
                    String op1 = naiveRec(S1, S2, i + 1, j);
                    String op2 = naiveRec(S1, S2, i, j + 1);
                    return (op1.length() > op2.length()) ? op1 : op2;
                }
            }

            @Override
            protected void done() {
                try {
                    h1.removeAllHighlights();
                    h2.removeAllHighlights();
                    if (!isCancelled()) {
                        String res = get();
                        logArea.append("Résultat Naïf: " + res + "\nLongueur: " + res.length() + "\n");
                        statusLabel.setText("Terminé (Naïf).");
                    }
                } catch (Exception e) {
                    statusLabel.setText("Erreur ou Annulation.");
                }
            }
        };
        currentWorker.execute();
    }


    // ==========================================
    // WORKER : PROG DYNAMIQUE
    // ==========================================
    private void startDP() {
        stopWorker();
        String s1 = txtS1.getText();
        String s2 = txtS2.getText();

        // Setup Table
        int n = s1.length();
        int m = s2.length();
        
        // Headers
        String[] cols = new String[m + 2];
        cols[0] = " "; // Row headers
        cols[1] = "ε";
        for (int k = 0; k < m; k++) cols[k + 2] = String.valueOf(s2.charAt(k));
        
        Object[][] data = new Object[n + 2][m + 2];
        // Init grid empty
        for (int i=0; i<n+2; i++) {
            for (int j=0; j<m+2; j++) {
                data[i][j] = "";
            }
        }
        // Labels
        data[0][0] = " ";
        data[0][1] = "ε"; // Col header displayed in table content for simplicity of rendering index
        for(int k=0; k<m; k++) data[0][k+2] = String.valueOf(s2.charAt(k)); // Just visual
        
        data[1][0] = "ε";
        for(int k=0; k<n; k++) data[k+2][0] = String.valueOf(s1.charAt(k));

        tableModel.setDataVector(data, cols);
        
        // Custom renderer data
        final int[][] dpValues = new int[n+1][m+1];
        final boolean[][] isPath = new boolean[n+2][m+2];
        final Point currentCell = new Point(-1, -1);
        
        ((MatrixCellRenderer) matrixTable.getDefaultRenderer(Object.class)).setMetaData(isPath, currentCell);
        
        logArea.setText("Début Prog Dynamique...\n");
        statusLabel.setText("Remplissage du tableau...");

        currentWorker = new SwingWorker<String, Point>() {
            @Override
            protected String doInBackground() throws Exception {
                // 1. Remplissage
                for (int i = 0; i <= n; i++) {
                    for (int j = 0; j <= m; j++) {
                        if (isCancelled()) return "";
                        
                        publish(new Point(i, j)); // Update UI focus
                        Thread.sleep(getDelay() / 2); // Un peu plus rapide le remplissage
                        
                        if (i == 0 || j == 0) {
                            dpValues[i][j] = 0;
                        } else if (s1.charAt(i - 1) == s2.charAt(j - 1)) {
                            dpValues[i][j] = dpValues[i - 1][j - 1] + 1;
                        } else {
                            dpValues[i][j] = Math.max(dpValues[i - 1][j], dpValues[i][j - 1]);
                        }
                        
                        // Update Data model directly (safe if done via invokeLater, but here we update primitive array then refresh table)
                        final int val = dpValues[i][j];
                        final int r = i + 1; // +1 car row 0 is header chars
                        final int c = j + 1; 
                        SwingUtilities.invokeLater(() -> tableModel.setValueAt(val, r, c));
                    }
                }

                // 2. Remontée (Traceback)
                publish(new Point(-2, -2)); // Signal start traceback
                Thread.sleep(500);

                StringBuilder sb = new StringBuilder();
                int i = n;
                int j = m;
                
                while (i > 0 && j > 0) {
                    if (isCancelled()) return "";
                    
                    isPath[i+1][j+1] = true;
                    publish(new Point(i, j)); // Trigger repaint
                    Thread.sleep(getDelay());

                    if (s1.charAt(i - 1) == s2.charAt(j - 1)) {
                        sb.append(s1.charAt(i - 1));
                        i--;
                        j--;
                    } else {
                        if (dpValues[i - 1][j] >= dpValues[i][j - 1]) {
                            i--;
                        } else {
                            j--;
                        }
                    }
                }
                // Mark start
                isPath[i+1][j+1] = true;
                matrixTable.repaint();

                return sb.reverse().toString();
            }

            @Override
            protected void process(List<Point> chunks) {
                Point last = chunks.get(chunks.size() - 1);
                if (last.x == -2) {
                    statusLabel.setText("Reconstruction du chemin...");
                    currentCell.setLocation(-1, -1);
                } else {
                    currentCell.setLocation(last.x + 1, last.y + 1);
                }
                matrixTable.repaint();
            }

            @Override
            protected void done() {
                try {
                    currentCell.setLocation(-1, -1);
                    matrixTable.repaint();
                    if (!isCancelled()) {
                        String res = get();
                        logArea.append("Résultat ProgDyn: " + res + "\nLongueur: " + res.length() + "\n");
                        statusLabel.setText("Terminé (Prog Dyn).");
                    }
                } catch (Exception e) { e.printStackTrace(); }
            }
        };
        currentWorker.execute();
    }


    // Custom Renderer for colors
    class MatrixCellRenderer extends DefaultTableCellRenderer {
        private boolean[][] isPath;
        private Point currentCell;

        public void setMetaData(boolean[][] isPath, Point currentCell) {
            this.isPath = isPath;
            this.currentCell = currentCell;
        }

        @Override
        public Component getTableCellRendererComponent(JTable table, Object value, boolean isSelected, boolean hasFocus, int row, int column) {
            Component c = super.getTableCellRendererComponent(table, value, isSelected, hasFocus, row, column);
            
            // Headers background
            if (row == 0 || column == 0) {
                c.setBackground(Color.LIGHT_GRAY);
                c.setFont(c.getFont().deriveFont(Font.BOLD));
                setHorizontalAlignment(CENTER);
                return c;
            }

            // Normal Cells
            if (currentCell != null && row == currentCell.x && column == currentCell.y) {
                c.setBackground(COLOR_CURRENT);
            } else if (isPath != null && row < isPath.length && column < isPath[0].length && isPath[row][column]) {
                c.setBackground(COLOR_PATH);
            } else {
                c.setBackground(Color.WHITE);
            }
            
            setHorizontalAlignment(CENTER);
            return c;
        }
    }

    public static void main(String[] args) {
        SwingUtilities.invokeLater(() -> new VisualisateurPLSSC().setVisible(true));
    }
}
