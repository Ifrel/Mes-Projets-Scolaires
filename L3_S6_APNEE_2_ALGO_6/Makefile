JAVAC = javac
JAVA = java
SRC_DIR = .
BUILD_DIR = build

SOURCES = $(wildcard $(SRC_DIR)/*.java)
CLASSES = $(SOURCES:$(SRC_DIR)/%.java=$(BUILD_DIR)/%.class)

# Options de compilation
JFLAGS = -d $(BUILD_DIR) -sourcepath $(SRC_DIR)

all: $(CLASSES)

$(BUILD_DIR)/%.class: $(SRC_DIR)/%.java
	@mkdir -p $(BUILD_DIR)
	$(JAVAC) $(JFLAGS) $<

# Lancer le CLI (ex: make run-cli ARGS="-p exemple.test")
run-cli: all
	$(JAVA) -cp $(BUILD_DIR) RecherchePLSSC $(ARGS)

# Lancer l'interface graphique
run-gui: all
	$(JAVA) -cp $(BUILD_DIR) VisualisateurPLSSC

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean run-cli run-gui