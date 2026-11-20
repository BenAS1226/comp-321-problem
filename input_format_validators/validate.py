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
    N                               1 <= N <= 300 
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
MAX_PIECES = 10
MAX_B_VALUE = 10 * MAX_PIECES
INT = r"(0|[1-9][0-9]*)"  #non-negative and no leading zeros

def read_line(pattern):
    line = sys.stdin.readline()
    assert line.endswith("\n"), f"missing newline or unexpected EOF: {line!r}"
    match = re.fullmatch(pattern, line[:-1])
    assert match, f"bad line: {line!r}"
    return match.groups()

def read_int(pattern, low, high, what):
    (token,) = read_line(pattern)
    value = int(token)
    assert low <= value <= high, f"{what} out of range: {value}"
    return value

# S
S = read_int(INT, 1, 10000, "S")

# five candy lines, fixed order
own, mine = [], []
for name in TYPES:
    o_tok, a_tok = read_line(f"{name} {INT} {INT}")
    o, a = int(o_tok), int(a_tok)
    assert 0 <= o <= 100, f"{name}: o out of range: {o}"
    assert 1 <= a <= 10, f"{name}: a out of range: {a}"
    own.append(o)
    mine.append(a)

# N
N = read_int(INT, 1, 300, "N")

# listings
type_re = "(" + "|".join(TYPES) + ")"
listings = []
for i in range(N):
    name, c_tok = read_line(f"{TYPE} {INT}") 
    c = int(c_tok)
    assert 1 <= c <= 10, f"listing {i + 1}: c out of range: {c}"
    listings.append((TYPES.index(name), c)) 

# hidden values
bob = [] 
for name in TYPES:
    bob.append(read_int(f"{name} {INT}", 1, 10, f"{name}: b"))

# no extra input
assert sys.stdin.read() == "", "extra input after last line"

def cheapest_payment(stock, need):
    if need > MAX_B_VALUE: # even 10 pieces can't be worth enough
        return None 

    table = {(0, 0): (0, (0, 0, 0, 0, 0))}
    for _ in range(MAX_PIECES):
        next_table = dict(table)
        
        for (used, value), (cost, pieces) in table.items(): 
            if used == MAX_PIECES:
                continue
            
            for k in range(5):
                if pieces[k] < stock[k]:
                    key = (used + 1, min(MAX_B_VALUE, value + bob[k]))
                    new_pieces = list(pieces)
                    new_pieces[k] += 1
                    new_cost = cost + mine[k]
                    if key not in next_table or next_table[key][0] > new_cost:
                        next_table[key] = (new_cost, tuple(new_pieces))
        table = next_table

    best = None
    for (used, value), (cost, pieces) in table.items():
        if used >= 1 and value >= need and (best is None or cost < best[0]):
            best = (cost, pieces)
    return best


def greedy_score():
    stock = own[:]
    score = sum(o * a for o, a in zip(own, mine))   # starting score

    by_value = sorted(range(N), key=lambda i: -mine[listings[i][0]] * listings[i][1])
    
    for i in by_value:
        t, c = listings[i]
        need = bob[t] * c + 1                    # Bob's gain must be > 0
        payment = cheapest_payment(stock, need)
        
        if payment is None:
            continue
        cost, pieces = payment
        gain = mine[t] * c - cost
        
        if gain > 0:
            for k in range(5):
                stock[k] -= pieces[k]
            score += gain
    return score


reached = greedy_score()
assert reached >= 2 * S, f"greedy plan reaches {reached} < 2S = {2 * S}"

sys.exit(42)