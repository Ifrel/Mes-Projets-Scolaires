#!/usr/bin/env bash

# Script de test coloré et verbeux : compile puis compare assign1/assign2
# à un interpréteur Python de référence (priorités d'assign1).

set -u

# Palette ANSI (désactivée si sortie non interactive).
if command -v tput >/dev/null 2>&1 && [[ -t 1 ]]; then
  RED=$(tput setaf 1); GREEN=$(tput setaf 2); YELLOW=$(tput setaf 3); BLUE=$(tput setaf 4)
  BOLD=$(tput bold); RESET=$(tput sgr0)
else
  RED=""; GREEN=""; YELLOW=""; BLUE=""; BOLD=""; RESET=""
fi

VERBOSE=${VERBOSE:-1}
PYTHON=${PYTHON:-python3}
CLEAN=${CLEAN:-0}

info()   { echo "${BLUE}${BOLD}[INFO]${RESET} $*"; }
ok()     { echo "${GREEN}${BOLD}[OK]${RESET}   $*"; }
fail()   { echo "${RED}${BOLD}[FAIL]${RESET} $*"; }
warn()   { echo "${YELLOW}${BOLD}[WARN]${RESET} $*"; }
detail() { [[ ${VERBOSE} -gt 0 ]] && echo "   ${BLUE}- ${RESET}$*"; }

if ! command -v "${PYTHON}" >/dev/null 2>&1; then
  fail "Interpréteur ${PYTHON} introuvable (ajustez PYTHON=...)."
  exit 1
fi

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT"

info "Compilation en cours..."
if ! make; then
  fail "Compilation échouée. Installez flex/bison puis réessayez."
  exit 1
fi
ok "Compilation terminée."

programs=(assign1 assign2)
inputs=(tests/*.txt)
passed=0
failed=0

# Interpréteur Python pour générer les attentes dynamiquement.
run_py_expected() {
  local file="$1"
  ${PYTHON} - "$file" <<'PYCODE'
import math, sys

fname = sys.argv[1]
with open(fname, "r", encoding="utf-8") as fh:
    text = fh.read()

class Token:
    __slots__ = ("typ", "val")
    def __init__(self, typ, val=None):
        self.typ = typ
        self.val = val

def tokenize(s):
    tokens = []
    i, n = 0, len(s)
    while i < n:
        c = s[i]
        if c.isspace():
            i += 1
            continue
        if c.isdigit():
            j = i
            while j < n and (s[j].isdigit() or s[j] == '.'):
                j += 1
            tokens.append(Token("NUM", float(s[i:j])))
            i = j
            continue
        if c.isalpha():
            j = i
            while j < n and (s[j].isalnum() or s[j] == "_"):
                j += 1
            word = s[i:j]
            lower = word.lower()
            if lower == "cos":
                tokens.append(Token("COS"))
            elif lower == "sin":
                tokens.append(Token("SIN"))
            elif lower == "pi":
                tokens.append(Token("NUM", math.pi))
            else:
                tokens.append(Token("VAR", word))
            i = j
            continue
        if c in "+-*/%^=();":
            mapping = {
                "+": "PLUS", "-": "MINUS", "*": "MULT", "/": "DIV",
                "%": "MOD", "^": "EXP", "=": "ASSIGN", "(": "LP",
                ")": "RP", ";": "SEMI"
            }
            tokens.append(Token(mapping[c]))
            i += 1
            continue
        tokens.append(Token("SEMI"))  # caractère inconnu -> ';'
        i += 1
    tokens.append(Token("EOF"))
    return tokens

tokens = tokenize(text)
pos = 0
sym = {}

def current():
    return tokens[pos]

def consume(expected=None):
    global pos
    tok = tokens[pos]
    if expected and tok.typ != expected:
        raise ValueError(f"Attendu {expected}, obtenu {tok.typ}")
    pos += 1
    return tok

def parse_primary():
    tok = current()
    if tok.typ == "NUM":
        consume()
        return tok.val
    if tok.typ == "VAR":
        consume()
        return sym.get(tok.val, 0.0)
    if tok.typ == "LP":
        consume("LP")
        val = parse_assign()
        consume("RP")
        return val
    raise ValueError("Primaire attendu")

def parse_func():
    tok = current()
    if tok.typ == "COS":
        consume("COS")
        return math.cos(parse_expr())
    if tok.typ == "SIN":
        consume("SIN")
        return math.sin(parse_expr())
    return parse_primary()

def parse_unary():
    tok = current()
    if tok.typ == "PLUS":
        consume("PLUS")
        return parse_unary()
    if tok.typ == "MINUS":
        consume("MINUS")
        return -parse_unary()
    return parse_func()

def parse_power():
    left = parse_unary()
    if current().typ == "EXP":
        consume("EXP")
        right = parse_power()  # droite-associatif
        return math.pow(left, right)
    return left

def parse_term():
    val = parse_power()
    while current().typ in ("MULT", "DIV", "MOD"):
        op = consume().typ
        rhs = parse_power()
        if op == "MULT":
            val *= rhs
        elif op == "DIV":
            val /= rhs
        else:
            val = math.fmod(val, rhs)
    return val

def parse_add():
    val = parse_term()
    while current().typ in ("PLUS", "MINUS"):
        op = consume().typ
        rhs = parse_term()
        val = val + rhs if op == "PLUS" else val - rhs
    return val

def parse_expr():
    return parse_add()

def parse_assign():
    tok = current()
    if tok.typ == "VAR" and tokens[pos + 1].typ == "ASSIGN":
        varname = tok.val
        consume("VAR")
        consume("ASSIGN")
        val = parse_assign()
        sym[varname] = val
        return val
    return parse_expr()

def parse_stmt():
    val = parse_assign()
    consume("SEMI")
    return val

def parse_source():
    while current().typ != "EOF":
        if current().typ == "SEMI":
            consume("SEMI")
            continue
        parse_stmt()

try:
    parse_source()
    for k in sorted(sym.keys()):
        print(f"{k} = {sym[k]:.6f}")
except Exception:
    print("!!! ERREUR !!!")
PYCODE
}

for prog in "${programs[@]}"; do
  if [[ ! -x "$prog" ]]; then
    warn "$prog introuvable ou non exécutable."
    continue
  fi
  info "Tests avec ${prog}..."
  for input in "${inputs[@]}"; do
    detail "Exécution sur ${input}"
    tmp_out="$(mktemp)"
    tmp_err="${tmp_out}.err"
    tmp_expected="$(mktemp)"

    "./$prog" < "$input" > "$tmp_out" 2> "$tmp_err" || true
    run_py_expected "$input" > "$tmp_expected"

    if grep -q "^!!! ERREUR !!!$" "$tmp_expected"; then
      if grep -q "^!!! ERREUR !!!$" "$tmp_out"; then
        ok "$input"
        ((passed++))
      else
        fail "$input"
        echo "--- attendu ---"
        cat "$tmp_expected"
        echo "--- obtenu ---"
        cat "$tmp_out"
        [[ -s "$tmp_err" ]] && { echo "--- stderr ---"; cat "$tmp_err"; }
        ((failed++))
      fi
    else
      # Comparaison tolérante des floats via Python (isclose).
      if ${PYTHON} - "$tmp_expected" "$tmp_out" <<'PYCODE'
import sys, math
exp_path, act_path = sys.argv[1:3]

def parse(path):
    vals = {}
    with open(path, "r", encoding="utf-8") as fh:
        for line in fh:
            line = line.strip()
            if not line or line.startswith("!!! ERREUR !!!"):
                continue
            if "=" not in line:
                continue
            name, val = line.split("=", 1)
            vals[name.strip()] = float(val.strip())
    return vals

exp = parse(exp_path)
act = parse(act_path)

errors = []
all_keys = sorted(set(exp) | set(act))
for k in all_keys:
    if k not in exp:
        errors.append(f"Manque dans attendu: {k}")
        continue
    if k not in act:
        errors.append(f"Manque dans obtenu: {k}")
        continue
    if not math.isclose(exp[k], act[k], rel_tol=1e-5, abs_tol=1e-5):
        errors.append(f"{k}: attendu {exp[k]:.6f}, obtenu {act[k]:.6f}")

if errors:
    print("ECARTS:")
    for e in errors:
        print(" -", e)
    sys.exit(1)
PYCODE
      then
        ok "$input"
        ((passed++))
      else
        fail "$input"
        [[ -s "$tmp_err" ]] && { echo "--- stderr ---"; cat "$tmp_err"; }
        ((failed++))
      fi
    fi

    rm -f "$tmp_out" "$tmp_err" "$tmp_expected" "${tmp_out}.sorted" "${tmp_expected}.sorted"
  done
done

echo
if [[ $failed -eq 0 ]]; then
  ok "Résultat final: $passed réussis, $failed échoués."
else
  fail "Résultat final: $passed réussis, $failed échoués."
fi

if [[ "$CLEAN" -eq 1 ]]; then
  info "Nettoyage (make distclean)..."
  if ! make distclean >/dev/null 2>&1; then
    warn "Nettoyage incomplet (make distclean a échoué)."
  fi
else
  detail "Nettoyage distclean désactivé (exporter CLEAN=1 pour nettoyer)."
fi

exit $failed
