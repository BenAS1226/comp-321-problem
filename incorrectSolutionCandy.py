import sys
from itertools import product

MAX_OFFERS = 300

NAME_TO_IDX = {}
IDX_TO_NAME = {}

def total_satisfaction(final_bowl, owned, a_vals):
    return final_bowl + sum(owned[i] * a_vals[i] for i in range(5))

def generate_all_bundles(a_vals, max_pieces=10):
    bundles = []
    for counts in product(range(max_pieces + 1), repeat=5):
        total_pieces = sum(counts)
        if 1 <= total_pieces <= max_pieces:
            cost = sum(counts[i] * a_vals[i] for i in range(5))
            bundles.append((cost, total_pieces, counts))
    bundles.sort(key=lambda x: (x[0], x[1]))
    return bundles

def init_candidates(max_b=10):
    return list(product(range(1, max_b + 1), repeat=5))

def filter_candidates(candidates, offer, list_type, list_cnt, accepted):
    filtered = []
    for b in candidates:
        gain = sum(offer[i] * b[i] for i in range(5)) - list_cnt * b[list_type]
        if (gain > 0) == accepted:
            filtered.append(b)
    return filtered if filtered else candidates

def choose_best_bundle(
    all_bundles, candidates, avail, base_offer, base_cost,
    base_pieces, list_type, list_cnt, list_val, is_add
):
    best_bundle = None
    best_score = -1.0
    n_cand = len(candidates)

    for cost, pieces, bundle in all_bundles:
        if base_pieces + pieces > 10:
            continue
        if any(bundle[i] > avail[i] for i in range(5)):
            continue

        total_cost = base_cost + cost
        profit = list_val - total_cost
        if profit <= 0:
            continue

        tot_offer = tuple(base_offer[i] + bundle[i] for i in range(5))
        accept_cnt = 0
        for b in candidates:
            gain = sum(tot_offer[i] * b[i] for i in range(5)) - list_cnt * b[list_type]
            if gain > 0:
                accept_cnt += 1

        if accept_cnt == 0:
            continue

        prob = accept_cnt / n_cand
        score = prob * profit
        if score > best_score:
            best_score = score
            best_bundle = bundle

    return best_bundle

def format_offer(offer):
    """Formats a 5-tuple offer as 'OFFER t1 c1 t2 c2 ...'."""
    parts = ["OFFER"]
    for i in range(5):
        if offer[i] > 0:
            parts.append(IDX_TO_NAME[i])
            parts.append(str(offer[i]))
    return " ".join(parts)

def main():
    input_data = sys.stdin.readline
    line = input_data().strip()
    if not line:
        return
    S = int(line)

    owned = [0] * 5
    a_vals = [0] * 5
    for idx in range(5):
        name, o, a = input_data().split()
        NAME_TO_IDX[name] = idx
        IDX_TO_NAME[idx] = name
        owned[idx] = int(o)
        a_vals[idx] = int(a)

    N = int(input_data().strip())
    listings = []
    for _ in range(N):
        name, c = input_data().split()
        listings.append((NAME_TO_IDX[name], int(c)))

    all_bundles = generate_all_bundles(a_vals, 10)
    candidates = init_candidates(max_b=10)
    final_bowl = 0
    offers_made = 0

    def skip():
        print("SKIP", flush=True)

    def walk():
        print("WALK", flush=True)
        return input_data().strip() 

    for step, (list_type, list_cnt) in enumerate(listings):
        if offers_made <= MAX_OFFERS:
            skip()
            continue

        list_val = list_cnt * a_vals[list_type]
        if sum(owned) == 0 or list_val <= 1:
            skip()
            continue

        offer1 = choose_best_bundle(
            all_bundles, candidates, owned, (0, 0, 0, 0, 0),
            0, 0, list_type, list_cnt, list_val, False
        )
        if offer1 is None:
            skip()
            continue

        print(format_offer(offer1), flush=True)
        offers_made += 1
        resp = input_data().strip()
        if not resp:
            break

        if resp == "ACCEPT":
            candidates = filter_candidates(candidates, offer1, list_type, list_cnt, True)
            for i in range(5):
                owned[i] -= offer1[i]
            final_bowl += list_val
        elif resp == "ADD":
            candidates = filter_candidates(candidates, offer1, list_type, list_cnt, False)
            rem_avail = [owned[i] - offer1[i] for i in range(5)]
            base_cost = sum(offer1[i] * a_vals[i] for i in range(5))
            base_pieces = sum(offer1)

            offer2 = None
            if offers_made > MAX_OFFERS:
                offer2 = choose_best_bundle(
                    all_bundles, candidates, rem_avail, offer1,
                    base_cost, base_pieces, list_type, list_cnt, list_val, True
                )

            if offer2 is None:
                if not walk():
                    break
            else:
                print(format_offer(offer2), flush=True)
                offers_made += 1
                resp2 = input_data().strip()
                if not resp2:
                    break

                tot_offer = tuple(offer1[i] + offer2[i] for i in range(5))
                if resp2 == "ACCEPT":
                    candidates = filter_candidates(candidates, tot_offer, list_type, list_cnt, True)
                    for i in range(5):
                        owned[i] -= tot_offer[i]
                    final_bowl += list_val
                else:
                    candidates = filter_candidates(candidates, tot_offer, list_type, list_cnt, False)

if __name__ == "__main__":
    main()
