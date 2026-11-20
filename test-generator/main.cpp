#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <fstream>
#include <map>
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
// N [1, 300]
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



int main() {
    ifstream rng("/dev/random", ios::binary);
    
    if (!rng) {
        cerr << "Error opening /dev/random" << endl;
        return 1;
    }

    for (int f = 0; f < 23; f++) {
        input input;
        while (true) {  // retry until the test is valid
            vector<char> buffer(1);
            rng.read(buffer.data(), buffer.size());

            // N [1, 300]
            int n = rand_from_buf(1, 300, buffer[0]);
            buffer.resize(16 + 2 * n);
            rng.read(buffer.data(), buffer.size());
            size_t i = 0;

            input = {};
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
            while (i < buffer.size() - 6) {
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

            // Pick S from a plan that really works (same greedy plan as validate.py)
            array<int, 5> own, a, b;
            for (int k = 0; k < 5; k++) {
                own[k] = input.aliceCandy[TYPE_ORDER[k]].count;
                a[k] = input.aliceCandy[TYPE_ORDER[k]].satisfaction;
                b[k] = input.bobCandy[TYPE_ORDER[k]];
            }
            vector<pair<int, int>> listings;
            for (int l = 0; l < n; l++) {
                int t = find(begin(TYPE_ORDER), end(TYPE_ORDER), input.listings[l].type) - begin(TYPE_ORDER);
                listings.push_back({t, input.listings[l].count});
            }
            int best = greedy_score(own, a, b, listings);
            input.S = best / 2;                                    // 2S <= best, so 2S is reachable
            if (input.S > aliceSatSum && input.S <= 10000) break;  // skipping isn't enough
        }
        
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

        // f = 0..19 -> data/secret/secret_1..20, f = 20..22 -> data/sample/sample_1..3
        string base = "../data/secret/secret_" + to_string(f + 1);
        if (f >= 20) base = "../data/sample/sample_" + to_string(f - 19);

        ofstream(base + ".ans").close();   // empty .ans file (Deliverable 2.6)
        ofstream inputFile(base + ".in");

        inputFile << input.S << "\n";
        inputFile << "Starburst " << input.aliceCandy["Starburst"].count << " " << input.aliceCandy["Starburst"].satisfaction << "\n";
        inputFile << "Skittles " << input.aliceCandy["Skittles"].count << " " << input.aliceCandy["Skittles"].satisfaction << "\n";
        inputFile << "Twizzlers " << input.aliceCandy["Twizzlers"].count << " " << input.aliceCandy["Twizzlers"].satisfaction << "\n";
        inputFile << "ReesesPieces " << input.aliceCandy["ReesesPieces"].count << " " << input.aliceCandy["ReesesPieces"].satisfaction << "\n";
        inputFile << "Nerds " << input.aliceCandy["Nerds"].count << " " << input.aliceCandy["Nerds"].satisfaction << "\n";
        inputFile << input.N << "\n";
        for (int i = 0; i < input.N; i++) inputFile << input.listings[i].type << " " << input.listings[i].count << "\n";
        inputFile << "Starburst " << input.bobCandy["Starburst"] << "\n";
        inputFile << "Skittles " << input.bobCandy["Skittles"] << "\n";
        inputFile << "Twizzlers " << input.bobCandy["Twizzlers"] << "\n";
        inputFile << "ReesesPieces " << input.bobCandy["ReesesPieces"] << "\n";
        inputFile << "Nerds " << input.bobCandy["Nerds"] << "\n";
        inputFile << flush;
        inputFile.close();
    }
    return 0;
}
