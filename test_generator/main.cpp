#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <fstream>
#include <map>
#include <optional>
#include <ostream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

using namespace std;

struct CandyInfo{
    int count;
    int satisfaction;
};

struct Listing {
    string type;
    int count;
};

// S [1, 10000]
// Starburst    o [1, 100] a [1, 10]
// Skittles     o [1, 100] a [1, 10]
// Twizzlers    o [1, 100] a [1, 10]
// ReesesPieces o [1, 100] a [1, 10]
// Nerds        o [1, 100] a [1, 10]
// N [1, 100]
// [Candy Type]_i c_i [1, 10]  (i in [1, N])
// Starburst    b [1, 10] (Hidden)
// Skittles     b [1, 10] (Hidden)
// Twizzlers    b [1, 10] (Hidden)
// ReesesPieces b [1, 10] (Hidden)
// Nerds        b [1, 10] (Hidden)

// Initialize with max values
struct input {
    int S = 10000;
    unordered_map<string, CandyInfo> aliceCandy = {
        {"Starburst",    CandyInfo{100 ,10}},
        {"Skittles",     CandyInfo{100, 10}},
        {"Twizzlers",    CandyInfo{100, 10}},
        {"ReesesPieces", CandyInfo{100, 10}},
        {"Nerds",        CandyInfo{100, 10}}
    };
    int N = 100;
    Listing listings[300];

    unordered_map<string, int> bobCandy = { // Hidden
        {"Starburst",    10},
        {"Skittles",     10},
        {"Twizzlers",    10},
        {"ReesesPieces", 10},
        {"Nerds",        10}
    };
};

void write_input(input in, const string& fileName) {
    // S [1, 10000]
    // Starburst    o [1, 100] a [1, 10]
    // Skittles     o [1, 100] a [1, 10]
    // Twizzlers    o [1, 100] a [1, 10]
    // ReesesPieces o [1, 100] a [1, 10]
    // Nerds        o [1, 100] a [1, 10]
    // N [1, 100]
    // [Candy Type]_i c_i [1, 50]  (i in [1, N])
    // Starburst    b [1, 10] (Hidden)
    // Skittles     b [1, 10] (Hidden)
    // Twizzlers    b [1, 10] (Hidden)
    // ReesesPieces b [1, 10] (Hidden)
    // Nerds        b [1, 10] (Hidden)
    ofstream(fileName.substr(0, fileName.size() - 3) + ".ans").close();  // empty .ans (Deliverable 2.6)
    ofstream inputFile(fileName);


    inputFile << in.S << "\n";
    inputFile << "Starburst " << in.aliceCandy["Starburst"].count << " " << in.aliceCandy["Starburst"].satisfaction << "\n";
    inputFile << "Skittles " << in.aliceCandy["Skittles"].count << " " << in.aliceCandy["Skittles"].satisfaction << "\n";
    inputFile << "Twizzlers " << in.aliceCandy["Twizzlers"].count << " " << in.aliceCandy["Twizzlers"].satisfaction << "\n";
    inputFile << "ReesesPieces " << in.aliceCandy["ReesesPieces"].count << " " << in.aliceCandy["ReesesPieces"].satisfaction << "\n";
    inputFile << "Nerds " << in.aliceCandy["Nerds"].count << " " << in.aliceCandy["Nerds"].satisfaction << "\n";
    inputFile << in.N << "\n";
    for (int i = 0; i < in.N; i++) inputFile << in.listings[i].type << " " << in.listings[i].count << "\n";
    inputFile << "Starburst " << in.bobCandy["Starburst"] << "\n";
    inputFile << "Skittles " << in.bobCandy["Skittles"] << "\n";
    inputFile << "Twizzlers " << in.bobCandy["Twizzlers"] << "\n";
    inputFile << "ReesesPieces " << in.bobCandy["ReesesPieces"] << "\n";
    inputFile << "Nerds " << in.bobCandy["Nerds"] << "\n";
    inputFile << flush;
    inputFile.close();
}

int rand_from_buf(int min, int max, char byte) {
    return min + (max - min) * (static_cast<float>(byte) + 128) / 255;
}

const string TYPE_ORDER[5] = {"Starburst", "Skittles", "Twizzlers", "ReesesPieces", "Nerds"};
const int MAX_PIECES = 10;

struct Bundle {
    bool found = false;
    int cost = 0;             // a-values
    array<int, 5> pieces{};   // how many of each type we pay
};

// Mirrors best_bundle() in validate.py
Bundle best_bundle(const array<int, 5>& stock, int need, const array<int, 5>& a, const array<int, 5>& b) {
    const int cap = 10 * MAX_PIECES;     // max b-value of 10 pieces
    Bundle none;
    if (need > cap) return none;

    // key = (pieces used, b-value reached); value = cheapest (cost, pieces)
    using Key = pair<int, int>;
    vector<Key> order = {{0, 0}};        // insertion order
    map<Key, pair<int, array<int, 5>>> dp = {{{0, 0}, {0, {0, 0, 0, 0, 0}}}};

    for (int step = 0; step < MAX_PIECES; step++) {
        vector<Key> newOrder = order;
        auto newDp = dp;
        for (const Key& key : order) {
            int n = key.first, v = key.second;
            if (n == MAX_PIECES) continue;
            int cost = dp[key].first;
            const array<int, 5>& bun = dp[key].second;
            for (int k = 0; k < 5; k++) {
                if (bun[k] < stock[k]) {
                    Key nk = {n + 1, min(cap, v + b[k])};
                    array<int, 5> nb = bun;
                    nb[k]++;
                    int nc = cost + a[k];
                    auto it = newDp.find(nk);
                    if (it == newDp.end()) {
                        newDp[nk] = {nc, nb};
                        newOrder.push_back(nk);
                    } else if (it->second.first > nc) {
                        it->second = {nc, nb};
                    }
                }
            }
        }
        order = newOrder;
        dp = newDp;
    }

    Bundle best;
    for (const Key& key : order) {
        int n = key.first, v = key.second;
        int cost = dp[key].first;
        if (n >= 1 && v >= need && (!best.found || cost < best.cost)) {
            best.found = true;
            best.cost = cost;
            best.pieces = dp[key].second;
        }
    }
    return best;
}

// Score of the greedy plan. Mirrors greedy_lower_bound() in validate.py.
int greedy_score(const array<int, 5>& own, const array<int, 5>& a, const array<int, 5>& b, const vector<pair<int, int>>& listings) {
    array<int, 5> stock = own;
    int total = 0;
    for (int k = 0; k < 5; k++) total += own[k] * a[k];

    // most valuable listings first (by a * c)
    vector<int> idx(listings.size());
    for (size_t i = 0; i < idx.size(); i++) idx[i] = i;
    stable_sort(idx.begin(), idx.end(), [&](int x, int y) {
        return a[listings[x].first] * listings[x].second >
               a[listings[y].first] * listings[y].second;
    });

    for (int i : idx) {
        int t = listings[i].first, c = listings[i].second;
        int need = b[t] * c + 1;            // bob's gain must be strictly positive
        Bundle bb = best_bundle(stock, need, a, b);
        if (!bb.found) continue;
        int gain = a[t] * c - bb.cost;
        if (gain > 0) {
            for (int k = 0; k < 5; k++) stock[k] -= bb.pieces[k];
            total += gain;
        }
    }
    return total;
}

// Greedy score for a test (same plan as validate.py). 2S must be <= this.
int best_score(input& in) {
    array<int, 5> own, a, b;
    for (int k = 0; k < 5; k++) {
        own[k] = in.aliceCandy[TYPE_ORDER[k]].count;
        a[k] = in.aliceCandy[TYPE_ORDER[k]].satisfaction;
        b[k] = in.bobCandy[TYPE_ORDER[k]];
    }
    vector<pair<int, int>> listings;
    for (int l = 0; l < in.N; l++) {
        int t = find(begin(TYPE_ORDER), end(TYPE_ORDER), in.listings[l].type) - begin(TYPE_ORDER);
        listings.push_back({t, in.listings[l].count});
    }
    return greedy_score(own, a, b, listings);
}

// Set S to the largest valid value: half the greedy score, at most 10000.
void set_S(input& in) {
    in.S = min(best_score(in) / 2, 10000);
}

input generate_rand_case() {
    ifstream rng("/dev/random", ios::binary);
    while (true) {  // retry until the test is valid
        vector<char> buffer(1);
        rng.read(buffer.data(), buffer.size());

        // N [1, 300]
        int n = rand_from_buf(1, 300, buffer[0]);
        buffer.resize(17 + 2 * n);
        rng.read(buffer.data(), buffer.size());
        size_t i = 0;

        input input;
        input.N = n;

        // Starburst    o [1, 100] a [1, 10]
        // Skittles     o [1, 100] a [1, 10]
        // Twizzlers    o [1, 100] a [1, 10]
        // ReesesPieces o [1, 100] a [1, 10]
        // Nerds        o [1, 100] a [1, 10]
        int aliceSatSum = 0;
        vector<CandyInfo> minCosts(5);
        for (auto& [type, info] : input.aliceCandy) {
            int count = info.count = rand_from_buf(1, 100, buffer[i++]);
            int sat = info.satisfaction = rand_from_buf(1, 10, buffer[i++]);
            aliceSatSum += sat * count;

            bool added = false;
            for (size_t j = 0; j < minCosts.size(); j++) {
                if (sat < minCosts[j].satisfaction) {
                    minCosts.insert(minCosts.begin() + j, info);
                    added = true;
                    break;
                }
            }
            if (!added) minCosts.push_back(info);
        }

        int minCost = 0, left = n;
        for (auto c : minCosts) {
            if (c.count >= left) {
                minCost += c.satisfaction * left;
                break;
            } else {
                minCost += c.satisfaction * c.count;
                left -= c.count;
            }
        }

        string types[] = {
            "Nerds",
            "ReesesPieces",
            "Skittles",
            "Starburst",
            "Twizzlers"
        };

        // [Candy Type]_i c_i [1, 50]  (i in [1, N])
        int bobSatSum = 0;
        unordered_map<string, int> listingCounts = {
            {"Starburst",    0},
            {"Skittles",     0},
            {"Twizzlers",    0},
            {"ReesesPieces", 0},
            {"Nerds",        0}
        };
        while (i < buffer.size() - 7) {
            int l = floor((i - 10) / 2.0);
            string type = types[rand_from_buf(0, 4, buffer[i++])];
            input.listings[l].type = type;
            int count = input.listings[l].count = rand_from_buf(1, 10, buffer[i++]);
            listingCounts[type] += count;
            bobSatSum += count * input.aliceCandy[type].satisfaction;
        }

        int maxSat = aliceSatSum + bobSatSum - minCost;
        i++;

        left = maxSat;
        for (string type : types) {
            int sat = rand_from_buf(1, 10, buffer[i++]);
            int satOffered = sat * listingCounts[type];
            if (satOffered >= left) sat = left = 1;
            else left -= satOffered;
            input.bobCandy[type] = sat;
        }

        // Pick S at random from the valid range
        int lo = aliceSatSum + 1;
        int hi = min(best_score(input) / 2, 10000);
        if (lo <= hi) {
            input.S = rand_from_buf(lo, hi, buffer[i++]);
            return input;
        }
    }
}

int main() {
    //Handcrafted Cases    
    vector<input> cases;

    // 1. Minimal S: cheapest possible fill still exceeds S.
    //    All candy costs 1, N=1, listing count 50 -> best cost 1 > S=...
    //    Actually set S=1 and make every listing cost 2 (sat=2), so S=1 < min cost.
    {
       input in;
       in.S = 1;
       in.N = 1;
       for (auto& [t, c] : in.aliceCandy) c = CandyInfo{100, 2};
       in.listings[0] = Listing{"Starburst", 1};
       cases.push_back(in);
    }

    // 2. Max S beyond max achievable satisfaction (5000): every candy full stock
    //    sat 10, total sat = 5000 < S -> infeasible by satisfaction ceiling.
    {
       input in; // defaults are all {100,10} and S=10000
       for (string t : {"Starburst", "Skittles", "Twizzlers", "ReesesPieces"}) {
           in.aliceCandy[t] = CandyInfo{100, 1};
           in.bobCandy[t] = 10;
       }
       in.aliceCandy["Nerds"] = CandyInfo{100, 10};
       in.bobCandy["Nerds"] = 1;
       in.N = 300;
       for (int i = 0; i < 300; i++) in.listings[i] = Listing{"Nerds", 10};
       cases.push_back(in);
    }

    // 3. S exactly equals the optimal cost: cheap candy (sat 1, count 100) covers
    //    N=100 listings of count 1; S == 100, boundary acceptance.
    {
       input in;
       in.S = 100;
       in.N = 100;
       for (auto& [t, c] : in.aliceCandy) c = CandyInfo{100, 1};
       for (int i = 0; i < 100; i++) in.listings[i] = Listing{"Nerds", 1};
       in.bobCandy["Nerds"] = 10;
       cases.push_back(in);
    }

    // 4. N=1 minimum, cheapest single listing, S=1 (tightest feasible case).
    {
       input in;
       in.S = 1;
       in.N = 1;
       for (auto& [t, c] : in.aliceCandy) c = CandyInfo{100, 1};
       in.listings[0] = Listing{"Skittles", 1};
       for (auto& [t, b] : in.bobCandy) b = 1;
       cases.push_back(in);
    }

    // 5. One type exhausts its stock: listing counts force buying all 100 of the
    //    sat-1 candy, remaining N listings must use sat-10 candy -> greedy cost check.
    {
       input in;
       for (auto& [t, c] : in.aliceCandy) c = CandyInfo{0, 1};
       for (auto& [t, b] : in.bobCandy) b = 1;
       in.aliceCandy["Starburst"] = CandyInfo{100, 1};  in.bobCandy["Starburst"] = 3;
       in.aliceCandy["Skittles"]  = CandyInfo{100, 4};  in.bobCandy["Skittles"]  = 3;
       in.aliceCandy["Nerds"]     = CandyInfo{0, 10};   in.bobCandy["Nerds"]     = 2;
       in.N = 100;
       for (int i = 0; i < 100; i++) in.listings[i] = Listing{"Nerds", 2};
       set_S(in);
       cases.push_back(in); // cheap candy runs out -> must spill to expensive candy
    }

    // 6. All N listings the same cheap type, count=50 each (max per listing),
    //    S covers half: forces partial purchase of max-count listings.
    {
       input in;
       in.N = 2;
       for (auto& [t, c] : in.aliceCandy) c = CandyInfo{100, 5};
       //    FIX: c is at most 10 (statement), so 10 instead of 50.
       in.listings[0] = Listing{"Twizzlers", 10};
       in.listings[1] = Listing{"Twizzlers", 10};
       in.S = 125; // buys exactly 25 of the 100
       cases.push_back(in);
    }

    // Handcrafted cases -> data/secret/secret_1..6
    for (size_t i = 0; i < cases.size(); i++)
        write_input(cases[i], "../data/secret/secret_" + to_string(i + 1) + ".in");

    // Random cases -> data/secret/secret_7..26 and data/sample/sample_1..3
    for (int i = 0; i < 20; i++)
        write_input(generate_rand_case(), "../data/secret/secret_" + to_string(cases.size() + i + 1) + ".in");
    for (int i = 0; i < 3; i++)
        write_input(generate_rand_case(), "../data/sample/sample_" + to_string(i + 1) + ".in");

    return 0;
}
