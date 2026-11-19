#include <cmath>
#include <iostream>
#include <fstream>
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

int rand_from_buf(int min, int max, char byte) {
    return min + (max - min) * (static_cast<float>(byte) + 128) / 255;
}

int main() {
    ifstream rng("/dev/random", ios::binary);
    
    if (!rng) {
        cerr << "Error opening /dev/random" << endl;
        return 1;
    }

    for (int f = 0; f < 21; f++) {
        vector<char> buffer(1);
        rng.read(buffer.data(), buffer.size());

        // N [1, 100]
        int n = rand_from_buf(1, 100, buffer[0]);
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
            int count = input.listings[l].count = rand_from_buf(1, 50, buffer[i++]);
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
        string fileName = "./inputs/secret_" + to_string(f + 1) + ".in";
        if (f == 20) fileName = "./inputs/sample.in";
        ofstream inputFile(fileName);

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
