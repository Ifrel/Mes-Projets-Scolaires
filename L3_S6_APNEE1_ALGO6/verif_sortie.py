#!/usr/bin/env python3

import sys

count=0
line=None
for line in sys.stdin:
    count += 1
    if count > 1000:
        print("Erreur: trop de lignes!")
        exit(1)
    print(line, end='')

def succes():
    print("OK")
    exit(0)

def echec(msg):
    print("Erreur:", msg)
    exit(1)

if line is None:
    echec("aucune ligne produite en sortie")

if "Couplage impossible" in line:
    succes()

if "Couplage non trouve" in line:
    succes()

if "Couplage trouve" not in line:
    echec("mauvais format pour la derniere ligne")

if ":" not in line:
    echec("`:' manquant sur la derniere ligne")

_,c = line.split(':')
c = c.strip()

nums = c.split(',')

try:
    for n in nums:
        i = int(n)
except ValueError:
    echec("les aretes du couplage doivent etre des entiers")

succes()

