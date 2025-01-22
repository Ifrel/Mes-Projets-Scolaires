#!/usr/bin/env python

import glob

from utils import *


for fichier in glob.glob("*.dimacs"):
    os.remove(fichier)

for fichier in glob.glob("**/*.dimacs", recursive=True):
    os.remove(fichier)

if os.path.exists("__pycache__"):
    shutil.rmtree("__pycache__")
if os.path.exists(".mypy_cache"):
    shutil.rmtree(".mypy_cache")
