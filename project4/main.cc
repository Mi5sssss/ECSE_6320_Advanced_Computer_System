#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <algorithm>
#include <chrono>
#include <cstdlib>
#ifdef __AVX2__
#include <immintrin.h>
#endif



int get_num_threads_from_env() {
    const char* num_threads_str = std::getenv("NUM_THREADS");
    return num_threads_str ? std::stoi(num_threads_str) : 4;
}

const int NUM_THREADS = get_num_threads_from_env();

// Global variables
std::mutex mtx;

// Dictionary encoder
void encode_thread(const std::vector<std::string> &data, int start, int end, std::unordered_map<std::string, int> &dict) {
    for (int i = start; i < end; i++) {
        mtx.lock();
        if (dict.find(data[i]) == dict.end()) {
            dict[data[i]] = dict.size();
        }
        mtx.unlock();
    }
}

std::vector<int> encode(const std::vector<std::string> &data) {
    std::vector<int> encoded_data(data.size());
    std::unordered_map<std::string, int> dict;

    int data_size = data.size();
    int chunk_size = data_size / NUM_THREADS;
    std::vector<std::thread> threads;
    for (int i = 0; i < NUM_THREADS; i++) {
        int start = i * chunk_size;
        int end = (i + 1) * chunk_size;
        if (i == NUM_THREADS - 1) {
            end = data_size;
        }
        threads.push_back(std::thread(encode_thread, std::ref(data), start, end, std::ref(dict)));
    }

    for (auto &t : threads) {
        t.join();
    }

    for (size_t i = 0; i < data.size(); i++) {
        encoded_data[i] = dict[data[i]];
    }

    return encoded_data;
}

// Query functions
std::vector<int> search(const std::vector<std::string> &data, const std::string &value) {
    std::vector<int> indices;
    for (size_t i = 0; i < data.size(); i++) {
        if (data[i] == value) {
            indices.push_back(i);
        }
    }
    return indices;
}

#ifdef __AVX2__
std::vector<int> search_simd(const std::vector<std::string> &data, const std::string &value) {
    std::vector<int> indices;

    __m256i value_data = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(value.data()));
    for (size_t i = 0; i < data.size(); i++) {
        __m256i data_i = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(data[i].data()));
        __m256i cmp = _mm256_cmpeq_epi8(value_data, data_i);

        if (_mm256_movemask_epi8(cmp) == 0xFFFFFFFF) {
            indices.push_back(i);
        }
    }
    return indices;
}
#endif

std::vector<std::pair<std::string, int>> prefix_search(const std::vector<std::string> &data, const std::string &prefix) {
    std::vector<std::pair<std::string, int>> result;

    for (size_t i = 0; i < data.size(); i++) {
        if (data[i].substr(0, prefix.size()) == prefix) {
            result.emplace_back(data[i], i);
        }
    }

    return result;
}


// Main function
int main(int argc, char *argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <search_value> <prefix> <data_path>" << std::endl;
        return 1;
    }
    // Load data from file
    std::ifstream infile(argv[3]);
    std::vector<std::string> data;
    std::string line;
    while (std::getline(infile, line)) {
        data.push_back(line);
    }
    infile.close();

    // Encode the data
    auto start = std::chrono::high_resolution_clock::now();
    std::vector<int> encoded_data = encode(data);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> encoding_time = end - start;

    // Query operations
    std::string search_value = argv[1];
    std::string prefix = argv[2];

    // Single data item search speed performance
    start = std::chrono::high_resolution_clock::now();
    std::vector<int> search_result = search(data, search_value);
    end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> search_time = end - start;

    #ifdef __AVX2__
    start = std::chrono::high_resolution_clock::now();
    std::vector<int> search_simd_result = search_simd(data, search_value);
    end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> search_simd_time = end - start;
    #endif

    // Prefix scan speed performance
    start = std::chrono::high_resolution_clock::now();
    std::vector<std::pair<std::string, int>> prefix_result = prefix_search(data, prefix);
    end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> prefix_time = end - start;

    // Print results
    std::cout << "Encoding time: " << encoding_time.count() << " seconds" << std::endl;
    std::cout << "Search time: " << search_time.count() << " seconds" << std::endl;
    #ifdef __AVX2__
    std::cout << "Search SIMD time: " << search_simd_time.count() << " seconds" << std::endl;
    #endif
    std::cout << "Prefix search time: " << prefix_time.count() << " seconds" << std::endl;

    return 0;
}
