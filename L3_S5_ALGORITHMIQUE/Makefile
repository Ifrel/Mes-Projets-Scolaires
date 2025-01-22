CFLAGS= -g -Wall -Werror -DDEBUG_ARBRE
CC=gcc

all: huff_encode huff_decode

huff_encode : huff_encode.o arbrebin.o fap.o bfile.o
	$(CC) $(CFLAGS) $^ -o $@

huff_decode : huff_decode.o arbrebin.o bfile.o
	$(CC) $(CFLAGS) $^ -o $@

# Dépendances obtenues avec "gcc -MM *.c"
arbrebin.o: arbrebin.c arbrebin.h
bfile.o: bfile.c bfile.h
fap.o: fap.c fap.h arbrebin.h
huff_decode.o: huff_decode.c arbrebin.h bfile.h
huff_encode.o: huff_encode.c fap.h arbrebin.h bfile.h huffman_code.h

.PHONY: clean netoyage
clean:
	rm -f huff_encode huff_decode $(patsubst %.c,%.o,$(wildcard *.c)) *.dot

netoyage:
	rm -f huff_encode huff_decode $(patsubst %.c,%.o,$(wildcard *.c)) *.dot *.txt *.png *.jpg

# FORMATAGE DES FICHIERS
# Réindentation de tout le code source avec un style courant
.PHONY: reindent
reindent:
	clang-format -style="{BasedOnStyle: llvm, IndentWidth: 4}" -i *.c *.h

CLANG_FORMAT = clang-format
FORMAT_FILES = $(shell find . -name "*.c" -o -name "*.h") # recursive
# FORMAT_FILES := $(wildcard *.c *.h) # Sur le repertoire courant

# Règles
.PHONY: format check-format

format:
	@echo "Formatting source files..."
	@$(CLANG_FORMAT) -i $(FORMAT_FILES)

check-format:
	@echo "Checking format..."
	@$(CLANG_FORMAT) --dry-run --Werror $(FORMAT_FILES)
