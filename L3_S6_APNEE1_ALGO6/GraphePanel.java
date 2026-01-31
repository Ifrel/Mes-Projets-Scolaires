import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.awt.geom.*;
import java.util.*;
import javax.swing.Timer;

public class GraphePanel extends JPanel {
    private Graphe graphe;
    private Set<String> arcsCouplage = new HashSet<>(); 
    
    // Physique et Positionnement
    private Point2D.Double[] coords;
    private Point2D.Double[] velocities;
    private boolean physicsEnabled = false;
    private Timer loopTimer;
    
    // Constantes visuelles (Dark Theme - Gris)
    private static final Color BG_COLOR = new Color(60, 63, 65); // Gris "Darcula"
    private static final Color GRID_COLOR = new Color(255, 255, 255, 20);
    private static final Color NODE_COLOR = new Color(0, 180, 255); // Cyan
    private static final Color NODE_COVERED_COLOR = new Color(0, 255, 150); // Green Neon
    private static final Color NODE_BORDER = new Color(255, 255, 255, 200);
    private static final Color EDGE_COLOR = new Color(200, 200, 200, 40); 
    private static final Color MATCH_COLOR = new Color(255, 80, 120); 
    private static final int NODE_RADIUS = 12;

    // Interaction
    private int draggingNode = -1;
    private int hoveredNode = -1;
    
    // Animation
    private int lastU = -1, lastV = -1;
    
    public GraphePanel() {
        this.setBackground(BG_COLOR);
        this.setPreferredSize(new Dimension(1000, 700));
        
        // Boucle de rendu et physique (60 FPS)
        loopTimer = new Timer(16, e -> updateLoop());
        loopTimer.start();

        MouseAdapter ma = new MouseAdapter() {
            @Override
            public void mousePressed(MouseEvent e) {
                if (coords == null) return;
                for (int i = 0; i < coords.length; i++) {
                    if (coords[i].distance(e.getPoint()) <= NODE_RADIUS + 5) {
                        draggingNode = i;
                        if (physicsEnabled) velocities[i].x = velocities[i].y = 0;
                        break;
                    }
                }
            }

            @Override
            public void mouseReleased(MouseEvent e) {
                draggingNode = -1;
            }

            @Override
            public void mouseDragged(MouseEvent e) {
                if (draggingNode != -1) {
                    coords[draggingNode].x = e.getX();
                    coords[draggingNode].y = e.getY();
                }
            }

            @Override
            public void mouseMoved(MouseEvent e) {
                if (coords == null) return;
                hoveredNode = -1;
                for (int i = 0; i < coords.length; i++) {
                    if (coords[i].distance(e.getPoint()) <= NODE_RADIUS + 5) {
                        hoveredNode = i;
                        break;
                    }
                }
            }
        };
        addMouseListener(ma);
        addMouseMotionListener(ma);
    }

    public void setGraphe(Graphe g) {
        this.graphe = g;
        this.arcsCouplage.clear();
        this.lastU = -1; this.lastV = -1;
        initPositionsCercles();
        if (g != null) {
            velocities = new Point2D.Double[g.nombreSommets()];
            for(int i=0; i<velocities.length; i++) velocities[i] = new Point2D.Double(0,0);
        }
        repaint();
    }
    
    public void resetVisuals() {
        this.arcsCouplage.clear();
        this.lastU = -1;
        this.lastV = -1;
        repaint();
    }

    public void togglePhysics() {
        this.physicsEnabled = !this.physicsEnabled;
    }
    
    public boolean isPhysicsEnabled() {
        return physicsEnabled;
    }

    private void updateLoop() {
        if (physicsEnabled && graphe != null && coords != null) {
            appliquerPhysique();
        }
        if (hoveredNode != -1 || physicsEnabled) {
            repaint();
        }
    }

    private void appliquerPhysique() {
        int n = graphe.nombreSommets();
        if (n > 600) return; 

        double repulsion = 1000.0;
        double ressort = 0.05;
        double friction = 0.85;
        
        for (int i = 0; i < n; i++) {
            if (i == draggingNode) continue;
            for (int j = 0; j < n; j++) {
                if (i == j) continue;
                double dx = coords[i].x - coords[j].x;
                double dy = coords[i].y - coords[j].y;
                double distSq = dx*dx + dy*dy;
                if (distSq < 1) distSq = 1;
                if (distSq > 40000) continue;
                
                double force = repulsion / distSq;
                double dist = Math.sqrt(distSq);
                velocities[i].x += (dx / dist) * force;
                velocities[i].y += (dy / dist) * force;
            }
        }

        Arc[] arcs = graphe.arcs();
        for (Arc a : arcs) {
            int u = a.source;
            int v = a.destination;
            if (u >= coords.length || v >= coords.length) continue;
            
            double dx = coords[u].x - coords[v].x;
            double dy = coords[u].y - coords[v].y;
            
            if (u != draggingNode) {
                velocities[u].x -= dx * ressort;
                velocities[u].y -= dy * ressort;
            }
            if (v != draggingNode) {
                velocities[v].x += dx * ressort;
                velocities[v].y += dy * ressort;
            }
        }
        
        double centerX = getWidth() / 2.0;
        double centerY = getHeight() / 2.0;
        for (int i = 0; i < n; i++) {
            if (i == draggingNode) continue;
            velocities[i].x += (centerX - coords[i].x) * 0.005;
            velocities[i].y += (centerY - coords[i].y) * 0.005;
            
            velocities[i].x *= friction;
            velocities[i].y *= friction;
            
            coords[i].x += velocities[i].x;
            coords[i].y += velocities[i].y;
            
            if (coords[i].x < NODE_RADIUS) coords[i].x = NODE_RADIUS;
            if (coords[i].x > getWidth() - NODE_RADIUS) coords[i].x = getWidth() - NODE_RADIUS;
            if (coords[i].y < NODE_RADIUS) coords[i].y = NODE_RADIUS;
            if (coords[i].y > getHeight() - NODE_RADIUS) coords[i].y = getHeight() - NODE_RADIUS;
        }
    }

    public void ajouterArcCouplage(int u, int v) {
        arcsCouplage.add(cl(u, v));
        lastU = u; lastV = v;
    }

    public void retirerArcCouplage(int u, int v) {
        arcsCouplage.remove(cl(u, v));
        lastU = u; lastV = v;
    }

    private String cl(int u, int v) {
        return Math.min(u, v) + "-" + Math.max(u, v);
    }

    private void initPositionsCercles() {
        if (graphe == null) return;
        int n = graphe.nombreSommets();
        coords = new Point2D.Double[n];
        int w = getWidth();
        int h = getHeight();
        int cx = w / 2;
        int cy = h / 2;
        int r = Math.min(w, h) / 2 - 80;

        for (int i = 0; i < n; i++) {
            double angle = 2 * Math.PI * i / n;
            coords[i] = new Point2D.Double(cx + r * Math.cos(angle), cy + r * Math.sin(angle));
        }
    }

    @Override
    protected void paintComponent(Graphics g) {
        super.paintComponent(g);
        Graphics2D g2 = (Graphics2D) g;
        g2.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
        g2.setRenderingHint(RenderingHints.KEY_TEXT_ANTIALIASING, RenderingHints.VALUE_TEXT_ANTIALIAS_ON);

        drawGrid(g2);

        if (graphe == null || coords == null) {
            drawCenteredString(g2, "Aucun graphe chargé", getWidth()/2, getHeight()/2, 24, Color.GRAY);
            return;
        }

        g2.setColor(EDGE_COLOR);
        g2.setStroke(new BasicStroke(1f));
        Arc[] arcs = graphe.arcs();
        for (Arc a : arcs) {
            if (!arcsCouplage.contains(cl(a.source, a.destination))) {
                Point2D.Double p1 = coords[a.source];
                Point2D.Double p2 = coords[a.destination];
                g2.draw(new Line2D.Double(p1.x, p1.y, p2.x, p2.y));
            }
        }

        g2.setStroke(new BasicStroke(2.5f));
        for (String key : arcsCouplage) {
            String[] parts = key.split("-");
            int u = Integer.parseInt(parts[0]);
            int v = Integer.parseInt(parts[1]);
            Point2D.Double p1 = coords[u];
            Point2D.Double p2 = coords[v];
            g2.setColor(new Color(MATCH_COLOR.getRed(), MATCH_COLOR.getGreen(), MATCH_COLOR.getBlue(), 100));
            g2.setStroke(new BasicStroke(5f));
            g2.draw(new Line2D.Double(p1.x, p1.y, p2.x, p2.y));
            g2.setColor(MATCH_COLOR);
            g2.setStroke(new BasicStroke(2f));
            g2.draw(new Line2D.Double(p1.x, p1.y, p2.x, p2.y));
        }

        for (int i = 0; i < coords.length; i++) {
            Point2D.Double p = coords[i];
            boolean isCovered = estCouvert(i);
            boolean isHover = (i == hoveredNode || i == draggingNode);
            Color c = isCovered ? NODE_COVERED_COLOR : NODE_COLOR;
            if (isHover) c = Color.WHITE;

            if (isCovered || isHover) {
                float[] dist = {0.0f, 1.0f};
                Color[] colors = {new Color(c.getRed(), c.getGreen(), c.getBlue(), 150), new Color(0,0,0,0)};
                RadialGradientPaint glow = new RadialGradientPaint(p, NODE_RADIUS * 2.5f, dist, colors);
                g2.setPaint(glow);
                g2.fill(new Ellipse2D.Double(p.x - NODE_RADIUS*2.5, p.y - NODE_RADIUS*2.5, NODE_RADIUS*5, NODE_RADIUS*5));
            }
            
            g2.setColor(c);
            g2.fill(new Ellipse2D.Double(p.x - NODE_RADIUS, p.y - NODE_RADIUS, NODE_RADIUS*2, NODE_RADIUS*2));
            g2.setColor(NODE_BORDER);
            g2.setStroke(new BasicStroke(1.5f));
            g2.draw(new Ellipse2D.Double(p.x - NODE_RADIUS, p.y - NODE_RADIUS, NODE_RADIUS*2, NODE_RADIUS*2));
            
            if (isHover || graphe.nombreSommets() < 50) {
                g2.setColor(Color.BLACK);
                g2.setFont(new Font("Arial", Font.BOLD, 10));
                FontMetrics fm = g2.getFontMetrics();
                String s = String.valueOf(i+1);
                g2.drawString(s, (float)(p.x - fm.stringWidth(s)/2), (float)(p.y + fm.getAscent()/2 - 2));
            }
        }
        drawHUD(g2);
    }

    private void drawGrid(Graphics2D g2) {
        g2.setColor(GRID_COLOR);
        int gridSize = 40;
        for (int x = 0; x < getWidth(); x+=gridSize) g2.drawLine(x, 0, x, getHeight());
        for (int y = 0; y < getHeight(); y+=gridSize) g2.drawLine(0, y, getWidth(), y);
    }
    
    private void drawHUD(Graphics2D g2) {
        g2.setColor(new Color(0, 0, 0, 150));
        g2.fillRoundRect(10, 10, 220, 80, 10, 10);
        g2.setColor(Color.WHITE);
        g2.setFont(new Font("Monospaced", Font.BOLD, 12));
        int y = 30;
        g2.drawString("Sommets   : " + (graphe!=null ? graphe.nombreSommets() : 0), 20, y); y += 20;
        g2.drawString("Couverts  : " + (arcsCouplage.size() * 2), 20, y); y += 20;
        g2.drawString("Physique  : " + (physicsEnabled ? "ON" : "OFF"), 20, y);
    }

    private void drawCenteredString(Graphics2D g, String text, int x, int y, int size, Color c) {
        g.setColor(c);
        g.setFont(new Font("Arial", Font.BOLD, size));
        FontMetrics fm = g.getFontMetrics();
        g.drawString(text, x - fm.stringWidth(text)/2, y + fm.getAscent()/2);
    }

    private boolean estCouvert(int u) {
        for (String key : arcsCouplage) {
            String[] parts = key.split("-");
            int s1 = Integer.parseInt(parts[0]);
            int s2 = Integer.parseInt(parts[1]);
            if (s1 == u || s2 == u) return true;
        }
        return false;
    }
}