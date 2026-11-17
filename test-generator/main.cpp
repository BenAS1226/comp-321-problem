#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>

enum class CandyType {
    Starburst,
    Skittles,
    Twizzlers,
    ReesesPieces,
    Nerds
};

struct CandyInfo{
    int count;
    int satisfaction;
};

struct Listing {
    CandyType type;
    int count;
};

enum class AliceResponses {
    Skip,
    Offer,
    Walk
};

enum class BobResponses {
    Accept,
    Add,
    Decline
};

// Initialize with max values
struct input {
    int S = 10000;
    std::pmr::unordered_map<CandyType, CandyInfo> aliceCandy = {
        {CandyType::Starburst,    CandyInfo{100 ,10}},
        {CandyType::Skittles,     CandyInfo{100, 10}},
        {CandyType::Twizzlers,    CandyInfo{100, 10}},
        {CandyType::ReesesPieces, CandyInfo{100, 10}},
        {CandyType::Nerds,        CandyInfo{100, 10}}
    };
    int N = 100;
    Listing listings[300];
    AliceResponses responses[600]; // 2 inputs per listing
};

struct output {
    BobResponses responses[600]; // 2 outputs per listing
};

// S [1, 10000]
// Starburst    o [1, 100] a [1, 10]
// Skittles     o [1, 100] a [1, 10]
// Twizzlers    o [1, 100] a [1, 10]
// ReesesPieces o [1, 100] a [1, 10]
// Nerds        o [1, 100] a [1, 10]
// N [1, 100]
// [Candy Type]_i c_i [1, 50]  (i in [1, N])

int clamp(int min, int max, int val) {
    return std::max({std::min({val, max}), min});
}

int lerp(int min, int max, int val) {
    return (val - min) / (max - min);
}

int main() {
    // Open /dev/random
    std::ifstream randomFile("/dev/random", std::ios::binary);
    
    if (!randomFile) {
        std::cerr << "Error opening /dev/random" << std::endl;
        return 1;
    }

    // Buffer to hold random data
    std::vector<char> buffer(1); // Read 1 bytes

    // Read random data
    randomFile.read(buffer.data(), buffer.size());

    // Check if read was successful
    if (static_cast<long unsigned int>(randomFile.gcount()) != buffer.size()) {
        std::cerr << "Error reading from /dev/random" << std::endl;
        return 1;
    }

    // Output the random bytes
    std::cout << "Random bytes: ";
    for (char byte : buffer) {
        std::cout << clamp(1, 100, static_cast<unsigned int>(byte)) << " ";
    }
    std::cout << std::dec << std::endl; // Reset to decimal output

    return 0;
}
