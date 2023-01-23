// g++ -std=c++11 -pthread -o main main.cc -lzstd
#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <zstd.h>
#include <cstring>

const int BLOCK_SIZE = 16 * 1024; // 16KB block size
const int NUM_THREADS = 20; // number of worker threads

std::ifstream inputFile;
std::ofstream outputFile;
std::vector<std::thread> workerThreads;
std::mutex mtx;
std::condition_variable cv;
std::vector<std::pair<char*, int>> blocks; // vector to store blocks and their sizes
int currentBlock = 0; // index of the current block being processed
bool finished = false; // flag to indicate if all blocks have been processed

void compressBlock(int threadNum) {
    while (true) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, []{return currentBlock < blocks.size() || finished;});
        if (finished) {
            lock.unlock();
            break;
        }
        int blockNum = currentBlock;
        currentBlock++;
        lock.unlock();
        cv.notify_all();

        char* input = blocks[blockNum].first;
        int inputSize = blocks[blockNum].second;
        int compressedSize = ZSTD_compressBound(inputSize);
        char* compressed = new char[compressedSize];

        int success = ZSTD_compress(compressed, compressedSize, input, inputSize, 1);
        if (ZSTD_isError(success)) {
            std::cout << "Error: " << ZSTD_getErrorName(success) << std::endl;
            return;
        }

        lock.lock();
        outputFile.write(compressed, success);
        lock.unlock();
        cv.notify_all();

        delete[] compressed;
        delete[] input;
    }
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cout << "Usage: " << argv[0] << " input_file output_file" << std::endl;
        return 1;
    }

    inputFile.open(argv[1], std::ios::binary);
    outputFile.open(argv[2], std::ios::binary);

    if (!inputFile.is_open()) {
        std::cout << "Error: Could not open input file" << std::endl;
        return 1;
    }
    if (!outputFile.is_open()) {
        std::cout << "Error: Could not open output file" << std::endl;
        return 1;
    }

    char* buffer = new char[BLOCK_SIZE];
    while (inputFile.read(buffer, BLOCK_SIZE)) {
        int size = inputFile.gcount();
        char* block = new char[size];
        std::memcpy(block, buffer, size);
        blocks.push_back({block, size});
    }
    delete[] buffer;

    // start worker threads
    for (int i = 0; i < NUM_THREADS; i++) {
        workerThreads.emplace_back(compressBlock, i);
    }

    // measure time consumption
    auto start = std::chrono::high_resolution_clock::now();

    {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, []{return currentBlock == blocks.size();});
    }

    finished = true;
    cv.notify_all();

    for (auto& thread : workerThreads) {
        thread.join();
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Time consumption with " << NUM_THREADS << " threads: " << duration.count() << " milliseconds" << std::endl;

    inputFile.close();
    outputFile.close();

    return 0;
}
