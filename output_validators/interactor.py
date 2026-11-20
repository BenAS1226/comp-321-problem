#!/usr/bin/env python3
"""
Interactor (output validator) for Trick-or-Trade.

Kattis runs it as:
    interactor.py <input_file> <answer_file> <feedback_dir>
with its stdout connected to the contestant's stdin, and its stdin
connected to the contestant's stdout.

The .in file has the format checked by the input validator; it also
holds Bob's hidden values: the last five lines, "Starburst b" ... "Nerds b",
in the same order as the candy lines. They are NEVER sent to the contestant.

Exit 42 = Accepted, exit 43 = Wrong Answer. The reason is written to
<feedback_dir>/judgemessage.txt.
"""

import sys

TYPES = ["Starburst", "Skittles", "Twizzlers", "ReesesPieces", "Nerds"]
MAX_PIECES = 10

in_path, ans_path, feedback_dir = sys.argv[1], sys.argv[2], sys.argv[3]


def judge_message(msg):
    with open(feedback_dir + "/judgemessage.txt", "a") as f:
        f.write(msg + "\n")


def accept(msg):
    judge_message(msg)
    sys.exit(42)


def wrong(msg):
    judge_message("Wrong Answer: " + msg)
    sys.exit(43)


def send(line):
    """Send one line to the contestant."""
    try:
        print(line, flush=True)
    except BrokenPipeError:
        wrong("contestant exited early")


def receive(what):
    """Read one line from the contestant, or fail if they stopped talking."""
    line = sys.stdin.readline()
    if line == "":
        wrong(f"unexpected end of output while waiting for {what}")
    return line.split()


# ---------- read the test file ----------
with open(in_path) as f:
    lines = f.read().splitlines()

visible = lines[:-5]                       # everything except Bob's 5 lines
b = [int(line.split()[1]) for line in lines[-5:]]   # Bob's hidden values

S = int(lines[0])
own, mine = [], []
for i in range(5):
    _, o, a = lines[1 + i].split()
    own.append(int(o))
    mine.append(int(a))
N = int(lines[6])
listings = []
for i in range(N):
    name, c = lines[7 + i].split()
    listings.append((TYPES.index(name), int(c)))

stock = own[:]          # candy you can still offer
bowl = [0] * 5          # Final Bowl: received candy, never offerable

# ---------- send the visible input (NOT Bob's line) ----------
for line in visible:
    send(line)


def parse_offer(tokens, already, listing_no):
    """Parse 'OFFER t1 c1 t2 c2 ...' given pieces already on the table.
    Returns the new pieces as a list of 5 counts."""
    pairs = tokens[1:]
    if len(pairs) == 0 or len(pairs) % 2 != 0:
        wrong(f"listing {listing_no}: OFFER needs type/count pairs")
    add = [0] * 5
    seen = set()
    for j in range(0, len(pairs), 2):
        name, cnt = pairs[j], pairs[j + 1]
        if name not in TYPES:
            wrong(f"listing {listing_no}: unknown type {name!r}")
        if name in seen:
            wrong(f"listing {listing_no}: type {name} appears twice in one OFFER")
        seen.add(name)
        if not cnt.isdigit() or (len(cnt) > 1 and cnt[0] == "0"):
            wrong(f"listing {listing_no}: bad count {cnt!r}")
        cnt = int(cnt)
        if cnt < 1:
            wrong(f"listing {listing_no}: count must be at least 1")
        add[TYPES.index(name)] = cnt
    total = [already[k] + add[k] for k in range(5)]
    if sum(total) > MAX_PIECES:
        wrong(f"listing {listing_no}: more than {MAX_PIECES} pieces offered")
    for k in range(5):
        if total[k] > stock[k]:
            wrong(f"listing {listing_no}: offered {total[k]} {TYPES[k]} "
                  f"but only {stock[k]} available")
    return add


def bob_gain(table, t, c):
    return sum(b[k] * table[k] for k in range(5)) - b[t] * c


trades = 0
for i, (t, c) in enumerate(listings, start=1):
    tokens = receive(f"a command for listing {i}")
    if tokens == ["SKIP"]:
        continue
    if not tokens or tokens[0] != "OFFER":
        wrong(f"listing {i}: expected SKIP or OFFER, got {' '.join(tokens)!r}")

    table = parse_offer(tokens, [0] * 5, i)
    if bob_gain(table, t, c) > 0:
        traded = True
        send("ACCEPT")
    else:
        send("ADD")
        tokens = receive(f"OFFER or WALK after ADD on listing {i}")
        if tokens == ["WALK"]:
            traded = False
        elif tokens and tokens[0] == "OFFER":
            extra = parse_offer(tokens, table, i)
            table = [table[k] + extra[k] for k in range(5)]
            traded = bob_gain(table, t, c) > 0
        else:
            wrong(f"listing {i}: expected OFFER or WALK, got {' '.join(tokens)!r}")
        send("ACCEPT" if traded else "DECLINE")

    if traded:
        for k in range(5):
            stock[k] -= table[k]
        bowl[t] += c
        trades += 1

# ---------- after the last listing ----------
if sys.stdin.read().strip() != "":
    wrong("extra output after the last listing")

score = sum(mine[k] * (stock[k] + bowl[k]) for k in range(5))
if score < S:
    wrong(f"score {score} is below the goal S = {S} ({trades} trades made)")
accept(f"score {score} >= S = {S} ({trades} trades made)")