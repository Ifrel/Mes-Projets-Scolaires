import java.util.List;

public interface GrapheObserver {
    void onArcAjoute(int u, int v);
    void onArcRetire(int u, int v);
    void refresh();
    int getDelay();
}
