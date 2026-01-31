import javax.swing.*;
import java.awt.*;
import java.io.*;
import java.util.List;

public class GrapheApp extends JFrame implements GrapheObserver {
    private GraphePanel panel;
    private Graphe graphe;
    private JSlider speedSlider;
    private JLabel statusLabel;
    private JButton btnSearch;
    private JToggleButton btnPhysics;
    
    // Pour l'exécution dans un thread séparé
    private SwingWorker<Void, Void> worker;

    public GrapheApp() {
        super("Visualisation Couplage Parfait - Ultimate Edition");
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setSize(1200, 900); // Plus grand
        setLocationRelativeTo(null);
        
        // Thème global sombre pour la fenêtre (Gris)
        try {
            UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
        } catch (Exception ignored) {}
        this.getContentPane().setBackground(new Color(60, 63, 65)); // Gris Darcula

        // --- Panel Principal (Graph) ---
        panel = new GraphePanel();
        add(panel, BorderLayout.CENTER);

        // --- Barre d'outils (Style Dark Gray) ---
        JPanel toolbar = new JPanel();
        toolbar.setBackground(new Color(45, 48, 50)); // Gris un peu plus foncé que le fond
        toolbar.setBorder(BorderFactory.createMatteBorder(0, 0, 1, 0, new Color(80, 80, 80)));
        
        JButton btnLoad = createDarkButton("Charger Fichier");
        btnLoad.addActionListener(e -> chargerFichier());
        
        JButton btnGen = createDarkButton("Générer Aléatoire");
        btnGen.addActionListener(e -> genererGraphe());

        btnSearch = createDarkButton("Lancer Recherche");
        btnSearch.setForeground(new Color(100, 255, 100)); // Vert néon
        btnSearch.setEnabled(false);
        btnSearch.addActionListener(e -> lancerRecherche());
        
        btnPhysics = new JToggleButton("Physique: OFF");
        styleToggleButton(btnPhysics);
        btnPhysics.addActionListener(e -> {
            panel.togglePhysics();
            btnPhysics.setText("Physique: " + (panel.isPhysicsEnabled() ? "ON" : "OFF"));
            if (panel.isPhysicsEnabled()) btnPhysics.setBackground(new Color(0, 100, 200));
            else btnPhysics.setBackground(new Color(70, 70, 70));
        });

        speedSlider = new JSlider(0, 500, 50); // 0 à 500ms
        speedSlider.setToolTipText("Délai animation (ms)");
        speedSlider.setBackground(new Color(45, 48, 50));
        speedSlider.setForeground(Color.WHITE);

        toolbar.add(btnLoad);
        toolbar.add(btnGen);
        toolbar.add(new JSeparator(JSeparator.VERTICAL));
        toolbar.add(btnPhysics);
        toolbar.add(new JSeparator(JSeparator.VERTICAL));
        toolbar.add(btnSearch);
        
        JLabel lblSpeed = new JLabel("Vitesse:");
        lblSpeed.setForeground(Color.WHITE);
        lblSpeed.setFont(new Font("SansSerif", Font.BOLD, 12));
        toolbar.add(lblSpeed);
        toolbar.add(speedSlider);

        add(toolbar, BorderLayout.NORTH);
        
        // --- Barre de statut ---
        statusLabel = new JLabel("Prêt");
        statusLabel.setForeground(Color.WHITE); // Blanc pur
        statusLabel.setFont(new Font("SansSerif", Font.PLAIN, 12));
        statusLabel.setBorder(BorderFactory.createEmptyBorder(5, 10, 5, 10));
        statusLabel.setOpaque(true);
        statusLabel.setBackground(new Color(45, 48, 50));
        add(statusLabel, BorderLayout.SOUTH);
    }
    
    private JButton createDarkButton(String text) {
        JButton btn = new JButton(text);
        //btn.setBackground(new Color(60, 60, 65));
        btn.setForeground(Color.WHITE);
        btn.setFont(new Font("SansSerif", Font.BOLD, 13));
        btn.setFocusPainted(false);
        btn.setOpaque(true);
        btn.setContentAreaFilled(true);
        btn.setBorder(BorderFactory.createCompoundBorder(
            BorderFactory.createLineBorder(new Color(100, 100, 100), 1),
            BorderFactory.createEmptyBorder(8, 15, 8, 15)
        ));
        
        btn.addMouseListener(new java.awt.event.MouseAdapter() {
            public void mouseEntered(java.awt.event.MouseEvent evt) {
                if(btn.isEnabled()) btn.setBackground(new Color(90, 90, 100));
            }
            public void mouseExited(java.awt.event.MouseEvent evt) {
                if(btn.isEnabled()) btn.setBackground(new Color(60, 60, 65));
            }
        });
        return btn;
    }
    
    private void styleToggleButton(JToggleButton btn) {
        btn.setBackground(new Color(60, 60, 65));
        btn.setForeground(Color.WHITE);
        btn.setFont(new Font("SansSerif", Font.BOLD, 13));
        btn.setFocusPainted(false);
        btn.setOpaque(true);
        btn.setContentAreaFilled(true);
        btn.setBorder(BorderFactory.createCompoundBorder(
            BorderFactory.createLineBorder(new Color(100, 100, 100), 1),
            BorderFactory.createEmptyBorder(8, 15, 8, 15)
        ));
    }

    private void chargerFichier() {
        JFileChooser fc = new JFileChooser(new File(".")); 
        if (fc.showOpenDialog(this) == JFileChooser.APPROVE_OPTION) {
            chargerGraphe(fc.getSelectedFile());
        }
    }
    
    private void chargerGraphe(File f) {
        try {
            FileInputStream fis = new FileInputStream(f);
            graphe = new Graphe(fis);
            graphe.setObserver(this); 
            panel.setGraphe(graphe);
            statusLabel.setText("Chargé : " + f.getName() + " (" + graphe.nombreSommets() + " sommets)");
            btnSearch.setEnabled(true);
            fis.close();
        } catch (Exception e) {
            JOptionPane.showMessageDialog(this, "Erreur chargement: " + e.getMessage());
        }
    }

    private void genererGraphe() {
        String nbStr = JOptionPane.showInputDialog(this, "Nombre de sommets (pair recommandé) :", "20");
        if (nbStr == null) return;
        try {
            int n = Integer.parseInt(nbStr);
            File temp = File.createTempFile("graphe_gen", ".txt");
            ProcessBuilder pb = new ProcessBuilder("python3", "gen_graphe.py", String.valueOf(n), "3", temp.getAbsolutePath(), "1");
            Process p = pb.start();
            p.waitFor();
            chargerGraphe(temp);
            temp.deleteOnExit(); 
        } catch (Exception e) {
            JOptionPane.showMessageDialog(this, "Erreur génération: " + e.getMessage());
        }
    }

    private void lancerRecherche() {
        if (graphe == null) return;
        
        btnSearch.setEnabled(false);
        btnPhysics.setEnabled(false); // Eviter de bouger pendant le calcul intensif si physique ON
        statusLabel.setText("Recherche en cours...");
        panel.resetVisuals(); // On ne reset QUE le visuel (couleurs), PAS les positions
        
        worker = new SwingWorker<Void, Void>() {
            @Override
            protected Void doInBackground() throws Exception {
                List<Arc> res = graphe.rechercherCouplage();
                
                SwingUtilities.invokeLater(() -> {
                    if (res != null) statusLabel.setText("Couplage Trouvé ! (" + res.size() + " arcs)");
                    else statusLabel.setText("Couplage Impossible / Non trouvé");
                    btnSearch.setEnabled(true);
                    btnPhysics.setEnabled(true);
                });
                return null;
            }
        };
        worker.execute();
    }

    @Override
    public void onArcAjoute(int u, int v) {
        SwingUtilities.invokeLater(() -> {
            panel.ajouterArcCouplage(u, v);
            // Pas de repaint explicite ici car la loopTimer le fait si nécessaire
        });
    }

    @Override
    public void onArcRetire(int u, int v) {
        SwingUtilities.invokeLater(() -> {
            panel.retirerArcCouplage(u, v);
        });
    }

    @Override
    public void refresh() {
    }

    @Override
    public int getDelay() {
        return speedSlider.getValue();
    }

    public static void main(String[] args) {
        SwingUtilities.invokeLater(() -> {
            new GrapheApp().setVisible(true);
        });
    }
}
