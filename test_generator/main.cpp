#include <cmath>
#include <cstddef>
#include <iostream>
#include <fstream>
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
// [Candy Type]_i c_i [1, 50]  (i in [1, N])
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

void write_input(input& in, const string& fileName) {
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

input generate_rand_case() {
    ifstream rng("/dev/random", ios::binary);

    vector<char> buffer(1);
    rng.read(buffer.data(), buffer.size());

    // N [1, 300]
    int n = rand_from_buf(1, 300, buffer[0]);
    buffer.resize(16 + 2 * n);
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
    while (i < buffer.size() - 6) {
        int l = floor((i - 10) / 2.0);
        string type = types[rand_from_buf(0, 4, buffer[i++])];
        input.listings[l].type = type;
        int count = input.listings[l].count = rand_from_buf(1, 10, buffer[i++]);
        listingCounts[type] += count;
        bobSatSum += count * input.aliceCandy[type].satisfaction;
    }

    int maxSat = aliceSatSum + bobSatSum - minCost;
    input.S = rand_from_buf(aliceSatSum + 1, maxSat / 2, buffer[i++]);

    left = maxSat;
    for (string type : types) {
        int sat = rand_from_buf(1, 10, buffer[i++]);
        int satOffered = sat * listingCounts[type];
        if (satOffered >= left) sat = left = 1;
        else left -= satOffered;
        input.bobCandy[type] = sat;
    }

    return input;
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
       in.N = 1;
       in.listings[0] = Listing{"Starburst", 50};
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
       in.S = 10000;
       in.N = 100;
       for (auto& [t, c] : in.aliceCandy) c = CandyInfo{100, 10};
       in.aliceCandy["Starburst"] = CandyInfo{100, 1};
       for (int i = 0; i < 100; i++) in.listings[i] = Listing{"Starburst", 2};
       cases.push_back(in); // 100 listings x2 > 100 stock -> must spill to expensive candy
    }

    // 6. All N listings the same cheap type, count=50 each (max per listing),
    //    S covers half: forces partial purchase of max-count listings.
    {
       input in;
       in.N = 2;
       for (auto& [t, c] : in.aliceCandy) c = CandyInfo{100, 5};
       in.listings[0] = Listing{"Twizzlers", 50};
       in.listings[1] = Listing{"Twizzlers", 50};
       in.S = 125; // buys exactly 25 of the 100
       cases.push_back(in);
    }

    for (int i = 0; i < 20; i++) cases.push_back(generate_rand_case());
    for (size_t i = 0; i < cases.size() - 1; i++) write_input(cases[i], "./inputs/secret_" + to_string(i + 1) + ".in");
    write_input(cases[cases.size() - 1], "./inputs/sample.in");

    return 0;
}
