#include <fstream>
#include <random>

const int BLOCK_SIZE = 16 * 1024; // 16KB
const int NUM_BLOCKS = 500000;

int main() {
    std::ofstream output("input.data", std::ios::binary);

    // Create a random number generator
    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> dist(32, 126);

    // Generate random data and write it to the output file
    for (int i = 0; i < NUM_BLOCKS; i++) {
        for (int j = 0; j < BLOCK_SIZE; j++) {
            char random_byte = dist(rng);
            output.write(&random_byte, 1);
        }
    }

    output.close();

    return 0;
}
