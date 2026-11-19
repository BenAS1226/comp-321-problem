#include <cmath>
#include <iostream>
#include <fstream>
#include <ostream>
#include <string>
#include <unordered_map>
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
    // string responses[600]; // 2 inputs per listing
};

struct output {
    string responses[600]; // 2 outputs per listing
};

int rand_from_buf(int min, int max, char byte) {
    return min + (max - min) * (static_cast<float>(byte) + 128) / 255;
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
int main() {
    ifstream rng("/dev/random", ios::binary);
    
    if (!rng) {
        cerr << "Error opening /dev/random" << endl;
        return 1;
    }

    vector<char> buffer(1);
    rng.read(buffer.data(), buffer.size());

    // N [1, 100]
    int n = rand_from_buf(1, 100, buffer[0]);
    buffer.resize(11 + 3 * n);
    rng.read(buffer.data(), buffer.size());
    int i = 0;

    input example;
    example.N = n;

    // Starburst    o [1, 100] a [1, 10]
    // Skittles     o [1, 100] a [1, 10]
    // Twizzlers    o [1, 100] a [1, 10]
    // ReesesPieces o [1, 100] a [1, 10]
    // Nerds        o [1, 100] a [1, 10]
    int satSum = 0;
    for (auto& [type, info] : example.aliceCandy) {
        int count = info.count = rand_from_buf(1, 100, buffer[i++]);
        int sat = info.satisfaction = rand_from_buf(1, 10, buffer[i++]);
        satSum += sat * count;
    }
    example.S = satSum + rand_from_buf(1, 10000 - satSum, buffer[i++]);

    string types[] = {
        "Nerds",
        "ReesesPieces",
        "Skittles",
        "Starburst",
        "Twizzlers"
    };

    // [Candy Type]_i c_i [1, 50]  (i in [1, N])
    satSum = 0;
    while (i < buffer.size()) {
        int l = floor((i - 11) / 3);
        string type = types[rand_from_buf(0, 4, buffer[i++])];
        example.listings[l].type = type;
        int count = example.listings[l].count = rand_from_buf(1, 50, buffer[i++]);
        satSum += count * example.aliceCandy[type].satisfaction;
    }
    
    // S [1, 10000]
    // Starburst    o [1, 100] a [1, 10]
    // Skittles     o [1, 100] a [1, 10]
    // Twizzlers    o [1, 100] a [1, 10]
    // ReesesPieces o [1, 100] a [1, 10]
    // Nerds        o [1, 100] a [1, 10]
    // N [1, 100]
    // [Candy Type]_i c_i [1, 50]  (i in [1, N])
    cout << example.S << "\n";
    cout << "Starburst " << example.aliceCandy["Starburst"].count << " " << example.aliceCandy["Starburst"].satisfaction << "\n";
    cout << "Skittles " << example.aliceCandy["Skittles"].count << " " << example.aliceCandy["Skittles"].satisfaction << "\n";
    cout << "Twizzlers " << example.aliceCandy["Twizzlers"].count << " " << example.aliceCandy["Twizzlers"].satisfaction << "\n";
    cout << "ReesesPieces " << example.aliceCandy["ReesesPieces"].count << " " << example.aliceCandy["ReesesPieces"].satisfaction << "\n";
    cout << "Nerds " << example.aliceCandy["Nerds"].count << " " << example.aliceCandy["Nerds"].satisfaction << "\n";
    cout << example.N << "\n";
    for (int i = 0; i < example.N; i++) cout << example.listings[i].type << " " << example.listings[i].count << "\n";
    cout << flush;
  
    return 0;
}
