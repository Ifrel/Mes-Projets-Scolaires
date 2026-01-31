JAVAC = javac
JAVA = java
PYTHON = python3
SRC = $(wildcard *.java)
CLASSES = $(SRC:.java=.class)
TEST_DIR = tests

# Fichiers GUI
GUI_CLASSES = GrapheApp.class GraphePanel.class GrapheObserver.class

all: compile

compile: $(CLASSES)

%.class: %.java
	$(JAVAC) $<

# Cible pour générer des graphes de test
gen-tests:
	mkdir -p $(TEST_DIR)
	# Petit graphe pair (Solution forcée)
	$(PYTHON) gen_graphe.py 20 2 $(TEST_DIR)/petit_20.txt 1
	# Graphe moyen (Solution forcée)
	$(PYTHON) gen_graphe.py 500 3 $(TEST_DIR)/moyen_500.txt 1
	# Grand graphe (Solution forcée)
	$(PYTHON) gen_graphe.py 2000 4 $(TEST_DIR)/grand_2000.txt 1
	# Très grand graphe (Solution forcée)
	$(PYTHON) gen_graphe.py 5000 2 $(TEST_DIR)/tg_5000.txt 1
	# Graphe impair (Pas de solution possible)
	$(PYTHON) gen_graphe.py 1001 3 $(TEST_DIR)/impair_1001.txt 0
	# Graphe aléatoire dense (Solution probable mais pas garantie)
	$(PYTHON) gen_graphe.py 1000 10 $(TEST_DIR)/dense_1000.txt 0

test: compile
	chmod +x test.sh
	./test.sh

run-gui: compile
	$(JAVA) GrapheApp

clean:
	rm -f *.class
	rm -rf $(TEST_DIR)
	rm -f *.tmp # Nettoyage des fichiers temporaires
	rm -f *.log # Nettoyage des logs éventuels

.PHONY: all compile clean test gen-tests run-gui
