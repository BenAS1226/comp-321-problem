import sys
from itertools import product

#program ends after 300 offers
MAX_OFFERS = 300

NAME_TO_IDX = {}
IDX_TO_NAME = {}

#all possible combos of candy under 10 pieces
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
    # all possible b-value vectors(Bob's satisfaction) in [1..max_b]^5
    return list(product(range(1, max_b + 1), repeat=5))


def filter_candidates(candidates, offer, list_type, list_cnt, accepted):
    # bob accepts if gain more than 0  
    filtered = []
    for b in candidates:
        gain = sum(offer[i] * b[i] for i in range(5)) - list_cnt * b[list_type]
        if (gain > 0) == accepted:
            filtered.append(b)
    return filtered if filtered else candidates


def choose_best_bundle(
    all_bundles, candidates, avail, base_offer, base_cost, base_pieces, list_type, list_cnt, list_val
):
    # Selects profitable bundle with the highest expected value given remaining 
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
        # weeds out bad bundles
        if profit <= 0:
            continue

        tot_offer = tuple(base_offer[i] + bundle[i] for i in range(5))
        accept_cnt = sum(
            1 for b in candidates
            if sum(tot_offer[i] * b[i] for i in range(5)) - list_cnt * b[list_type] > 0
        )

        if accept_cnt == 0:
            continue

        prob = accept_cnt / n_cand
        score = prob * profit

        if score > best_score:
            # updates best bundle 
            best_score = score
            best_bundle = bundle

    return best_bundle


def get_fallback_bundle(avail, a_vals):
    # if no profitable offer exists, picks 1 piece of cheapest candy instead 
    # aka worst case scenario
    best_idx = -1
    lowest_val = float('inf')
    for i in range(5):
        if avail[i] > 0 and a_vals[i] < lowest_val:
            # cheapest candy
            lowest_val = a_vals[i]
            best_idx = i
    if best_idx == -1:
        return None
    bundle = [0] * 5
    bundle[best_idx] = 1
    return tuple(bundle)

# function to format offer aka OFFER candy amount
def format_offer(offer):
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
    # satisfaction
    S = int(line)

    owned = [0] * 5
    a_vals = [0] * 5

    # to keep track of owned and available candy 
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

# using helper functions 
    all_bundles = generate_all_bundles(a_vals, 10)
    candidates = init_candidates(max_b=10)
    # setting up bowl and offers 
    final_bowl = 0
    offers_made = 0

    for step, (list_type, list_cnt) in enumerate(listings):
        if sum(owned) == 0:
            break  # Cannot make any valid non-empty offer if inventory is completely empty

        list_val = list_cnt * a_vals[list_type]

        #Bob's first offer yipee
        offer1 = choose_best_bundle(
            all_bundles, candidates, owned, (0, 0, 0, 0, 0), 0, 0, list_type, list_cnt, list_val
        )

        if offer1 is None:
            # use worst-case scenario function
            offer1 = get_fallback_bundle(owned, a_vals)

        # print Bob's first offer (bob is usually a penny pincher >:/)
        print(format_offer(offer1), flush=True)
        offers_made += 1


        # documenting user response 
        resp = input_data().strip()
        if not resp:
            break
        # if user accepts offer then 
        if resp == "ACCEPT":
            candidates = filter_candidates(candidates, offer1, list_type, list_cnt, True)
            for i in range(5):
                # Bob no longer owns this 
                owned[i] -= offer1[i]
            # goes into our final bowl 
            final_bowl += list_val

        # if user wants more candy from the offer
        elif resp == "ADD":
            candidates = filter_candidates(candidates, offer1, list_type, list_cnt, False)
            rem_avail = [owned[i] - offer1[i] for i in range(5)]
            base_cost = sum(offer1[i] * a_vals[i] for i in range(5))
            base_pieces = sum(offer1)

            offer2 = choose_best_bundle(
                all_bundles, candidates, rem_avail, offer1, base_cost, base_pieces, list_type, list_cnt, list_val
            )

            # minimal 1-candy fallback offer 
            if offer2 is None and base_pieces < 10:
                offer2 = get_fallback_bundle(rem_avail, a_vals)

            if offer2 is not None:
                print(format_offer(offer2), flush=True)
                offers_made += 1

                resp2 = input_data().strip()
                if not resp2:
                    break
                # if we respond Accept again
                tot_offer = tuple(offer1[i] + offer2[i] for i in range(5))
                if resp2 == "ACCEPT":
                    candidates = filter_candidates(candidates, tot_offer, list_type, list_cnt, True)
                    for i in range(5):
                        # updating Bob's owned and our final bowl 
                        owned[i] -= tot_offer[i]
                    final_bowl += list_val
                else:
                    # if response is decline then loop runs again
                    candidates = filter_candidates(candidates, tot_offer, list_type, list_cnt, False)


if __name__ == "__main__":
    main()
