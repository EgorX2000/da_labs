#include <algorithm>
#include <array>
#include <chrono>
#include <cstddef>
#include <fstream>
#include <functional>
#include <iostream>
#include <iterator>
#include <numeric>
#include <random>
#include <string>
#include <vector>

typedef struct Pair {
    unsigned long long key;
    unsigned long long value;
} Pair;

int getLength(unsigned long long num) {
    int length = 0;

    do {
        length++;
        num /= 10;
    } while (num > 0);

    return length;
}

int getDigit(unsigned long long num, const int& p) {
    for (int i = 0; i < p; i++) {
        num /= 10;
    }

    return num % 10;
}

void radixLSD(std::vector<Pair>& pairs, const int& maxLength) {
    std::array<std::vector<Pair>, 10> temp;

    for (int p = 0; p < maxLength; p++) {
        for (int i = 0; i < pairs.size(); i++) {
            temp[getDigit(pairs[i].key, p)].push_back(pairs[i]);
        }
        int index = 0;
        for (int i = 0; i < 10; i++) {
            if (!(temp[i].empty())) {
                for (int j = 0; j < temp[i].size(); j++) {
                    pairs[index] = temp[i][j];
                    index++;
                }
            }
            temp[i].clear();
        }
    }
}

std::vector<Pair> GenerateDataset(size_t N) {
    std::random_device rd;
    std::mt19937_64 rng(rd());

    std::uniform_int_distribution<unsigned long long> dist(
        0u, std::numeric_limits<unsigned long long>::max());

    std::vector<Pair> dataset(N);

    for (auto& pair : dataset) {
        pair.key = dist(rng);
        pair.value = dist(rng);
    }

    return dataset;
}

void Benchmark() {
    constexpr const size_t DatasetSize = 10'000'000;

    std::vector<Pair> dataset = GenerateDataset(DatasetSize);

    int maxLength = 0;
    for (int i = 0; i < dataset.size(); i++) {
        maxLength = std::max(maxLength, getLength(dataset[i].key));
    }

    // std::sort
    std::chrono::duration<float> standartSort;
    {
        std::vector<Pair> data = dataset;
        auto start = std::chrono::steady_clock::now();
        std::sort(data.begin(), data.end(),
                  [](const Pair& a, const Pair& b) { return a.key < b.key; });
        standartSort = std::chrono::steady_clock::now() - start;
    }

    // radix sort
    std::chrono::duration<float> radixSort;
    {
        std::vector<Pair> data = dataset;
        auto start = std::chrono::steady_clock::now();
        radixLSD(data, maxLength);
        radixSort = std::chrono::steady_clock::now() - start;
    }

    std::cout << "Standart sort took " << standartSort.count() << " seconds.\n";
    std::cout << "Radix sort took " << radixSort.count() << " seconds.\n";
}

void WriteCSV(std::string fileName) {
    std::ofstream file(fileName);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << fileName << std::endl;
        return;
    }

    file << "N,radix_time,std_time\n";
    size_t N = 10'000;

    for (int i = 1; i < 200; ++i) {
        N += 10'000;
        std::vector<Pair> data = GenerateDataset(N);

        int maxLength = 0;
        for (const auto& pair : data) {
            maxLength = std::max(maxLength, getLength(pair.key));
        }

        // std::sort
        std::chrono::duration<float> std_time;
        {
            auto dataset = data;
            auto start = std::chrono::steady_clock::now();
            std::sort(
                dataset.begin(), dataset.end(),
                [](const Pair& a, const Pair& b) { return a.key < b.key; });
            std_time = std::chrono::steady_clock::now() - start;
        }

        // radix sort
        std::chrono::duration<float> radix_time;
        {
            auto dataset = data;
            auto start = std::chrono::steady_clock::now();
            radixLSD(dataset, maxLength);
            radix_time = std::chrono::steady_clock::now() - start;
        }

        char buffer[100];
        snprintf(buffer, sizeof(buffer), "%zu,%f,%f\n", N, radix_time.count(),
                 std_time.count());
        file << buffer;
    }
}

int main() {
    Benchmark();
    // WriteCSV("sort.csv");

    /*
    unsigned long long inputKey, inputValue;
    Pair inputPair;
    std::vector<Pair> pairs;
    int maxLength = 0;
    while (std::cin >> inputKey >> inputValue) {
        inputPair.key = inputKey;
        inputPair.value = inputValue;
        pairs.push_back(inputPair);

        maxLength = std::max(maxLength, getLength(inputKey));
    }

    radixLSD(pairs, maxLength);

    for (int i = 0; i < pairs.size(); i++) {
        std::cout << pairs[i].key << "\t" << pairs[i].value << std::endl;
    }
    */

    return 0;
}