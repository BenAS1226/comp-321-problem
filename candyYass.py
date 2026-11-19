
MAX_OFFERS = 300


def total_satisfaction(final_bowl, owned, a_vals):
    return final_bowl + sum(owned[i] * a_vals[i] for i in range(5))


def main():
    input_data = sys.stdin.readline

    line = input_data().strip()
    if not line:
        return
    S = int(line)

    owned = [0] * 5
    a_vals = [0] * 5
    for _ in range(5):
        name, o, a = input_data().split()
        idx = NAME_TO_IDX[name]
        owned[idx] = int(o)
        a_vals[idx] = int(a)

    N = int(input_data().strip())
    listings = []
    for _ in range(N):
        name, c = input_data().split()
        listings.append((NAME_TO_IDX[name], int(c)))

    all_bundles = generate_all_bundles(a_vals, 10)
    candidates = init_candidates()
    final_bowl = 0
    offers_made = 0

    def walk():
        # We walk away; Bob can only answer DECLINE (he cannot skip)
        print("WALK", flush=True)
        input_data()

    for step, (list_type, list_cnt) in enumerate(listings):
        if offers_made >= MAX_OFFERS:
            break

        list_val = list_cnt * a_vals[list_type]

        if sum(owned) == 0 or list_val <= 1:
            walk()
            log_status(step, N, final_bowl, owned, a_vals, S, candidates)
            continue

        offer1 = choose_best_bundle(
            all_bundles, candidates, owned,
            (0, 0, 0, 0, 0), 0, 0,
            list_type, list_cnt, list_val, False
        )

        if offer1 is None:
            walk()
            log_status(step, N, final_bowl, owned, a_vals, S, candidates)
            continue

        print(format_offer(offer1), flush=True)
        offers_made += 1
        resp = input_data().strip()  # Bob: ACCEPT / ADD / DECLINE

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
            if offers_made < MAX_OFFERS:
                offer2 = choose_best_bundle(
                    all_bundles, candidates, rem_avail,
                    offer1, base_cost, base_pieces,
                    list_type, list_cnt, list_val, True
                )

            if offer2 is None:
                walk()
            else:
                print(format_offer(offer2), flush=True)
                offers_made += 1
                resp2 = input_data().strip()  # ACCEPT / DECLINE
                tot_offer = tuple(offer1[i] + offer2[i] for i in range(5))
                if resp2 == "ACCEPT":
                    candidates = filter_candidates(candidates, tot_offer, list_type, list_cnt, True)
                    for i in range(5):
                        owned[i] -= tot_offer[i]
                    final_bowl += list_val
                else:
                    candidates = filter_candidates(candidates, tot_offer, list_type, list_cnt, False)

        # resp == "DECLINE": nothing changes

        log_status(step, N, final_bowl, owned, a_vals, S, candidates)

    # Final verdict: need at least 2 * S
    if total_satisfaction(final_bowl, owned, a_vals) >= 2 * S:
        print("great job! Satisfaction Level reached!", flush=True)
    else:
        print("wrong answer", flush=True)
