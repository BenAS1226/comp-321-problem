#!/usr/bin/env python3
"""
Input validator for Trick-or-Trade.

Test file (.in) format:

    S                              1 <= S <= 10000
    Starburst    o a               0 <= o <= 100, 1 <= a <= 10
    Skittles     o a
    Twizzlers    o a
    ReesesPieces o a
    Nerds        o a
    N                               1 <= N <= 1000  (300 <= N with --secret)
    <type> c     (N lines)          1 <= c <= 10
    
    Starburst    b                      hidden values, 1 <= b <= 10
    Skittles     b
    Twizzlers    b
    ReesesPieces b
    Nerds        b

Bob's hidden values are on the last five lines of the .in file, one line
per type, in the same order as the candy lines. The interactor reads those
lines and does not forward them to the contestant.

Exit code 42 = valid.
"""

import sys
import re

TYPES = ["Starburst", "Skittles", "Twizzlers", "ReesesPieces", "Nerds"]
MAX_PIECES_PER_LISTING = 10

INT = r"(0|[1-9][0-9]*)"  #non-negative and no leading zeros


def read_line(pattern):
    line = sys.stdin.readline()
    assert line.endswith("\n"), f"missing newline or unexpected EOF: {line!r}"
    m = re.fullmatch(pattern, line[:-1])
    assert m, f"bad line: {line!r}"
    return m.groups()


# S
(s_tok,) = read_line(INT)
S = int(s_tok)
assert 1 <= S <= 10000, f"S out of range: {S}"

# five candy lines, fixed order
own, mine = [], []
for name in TYPES:
    o_tok, a_tok = read_line(re.escape(name) + " " + INT + " " + INT)
    o, a = int(o_tok), int(a_tok)
    assert 0 <= o <= 100, f"{name}: o out of range: {o}"
    assert 1 <= a <= 10, f"{name}: a out of range: {a}"
    own.append(o)
    mine.append(a)

# N
(n_tok,) = read_line(INT)
N = int(n_tok)
assert 1 <= N <= 1000, f"N out of range: {N}"
if secret:
    assert N >= 300, f"secret test needs N >= 300, got {N}"

# listings
type_re = "(" + "|".join(TYPES) + ")"
listings = []
for i in range(N):
    t_name, c_tok = read_line(type_re + " " + INT)
    c = int(c_tok)
    assert 1 <= c <= 10, f"listing {i + 1}: c out of range: {c}"
    listings.append((TYPES.index(t_name), c))

# hidden values
b = []
for name in TYPES:
    (b_tok,) = read_line(re.escape(name) + " " + INT)
    v = int(b_tok)
    assert 1 <= v <= 10, f"{name}: b out of range: {v}"
    b.append(v)

# no extra input
assert sys.stdin.read() == "", "extra input after last line"

# guarantee: with b known, a score of at least 2S is reachable
def base_score():
    return sum(o * a for o, a in zip(own, mine))

# any feasible plan is a valid lower bound. take listings in order of best profit. pay each with the cheapest bundle that beats Bob.
def greedy_lower_bound():
    stock = own[:]
    total = base_score()
    options = []
    for t, c in listings:
        need = b[t] * c + 1     # Bob's gain must be strictly positive
        options.append((t, c, need))
    # try profitable listings first (by value received)
    order = sorted(range(N), key=lambda i: -mine[options[i][0]] * options[i][1])
    for i in order:
        t, c, need = options[i]
        best = best_bundle(stock, need)
        if best is None:
            continue
        cost, bundle = best
        gain = mine[t] * c - cost
        if gain > 0:
            for k in range(5):
                stock[k] -= bundle[k]
            total += gain
    return total

#Cheapest (by our a-value) bundle of 1-10 pieces from stock whose b-value is >= need. Small DP over piece count and b-value.
def best_bundle(stock, need):
    cap = 10 * MAX_PIECES_PER_LISTING  # max b-value of 10 pieces
    if need > cap:
        return None
    # dp[n][v] = (cost, bundle) using n pieces reaching b-value v (capped)
    dp = {(0, 0): (0, (0, 0, 0, 0, 0))}
    for _ in range(MAX_PIECES_PER_LISTING):
        new = dict(dp)
        for (n, v), (cost, bun) in dp.items():
            if n == MAX_PIECES_PER_LISTING:
                continue
            for k in range(5):
                if bun[k] < stock[k]:
                    key = (n + 1, min(cap, v + b[k]))
                    nb = list(bun); nb[k] += 1; nb = tuple(nb)
                    nc = cost + mine[k]
                    if key not in new or new[key][0] > nc:
                        new[key] = (nc, nb)
        dp = new
    best = None
    for (n, v), (cost, bun) in dp.items():
        if n >= 1 and v >= need and (best is None or cost < best[0]):
            best = (cost, bun)
    return best

target = 2 * S
lb = greedy_lower_bound()
assert lb >= target, f"greedy plan reaches {lb} < 2S = {target}"

sys.exit(42)