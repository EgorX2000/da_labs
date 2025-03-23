#include <array>
#include <iostream>
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

int getDigit(unsigned long long num, int p) {
    for (int i = 0; i < p; i++) {
        num /= 10;
    }

    return num % 10;
}

void radixLSD(std::vector<Pair>& pairs, const int& maxLength) {
    std::array<std::vector<Pair>, 11> temp;

    int length;
    for (int p = 0; p < maxLength; p++) {
        for (int i = 0; i < pairs.size(); i++) {
            length = getLength(pairs[i].key);
            if (length > p) {
                temp[getDigit(pairs[i].key, p)].push_back(pairs[i]);
            } else {
                temp[10].push_back(pairs[i]);
            }
        }
        for (int i = 0; i < temp[10].size(); i++) {
            pairs[i] = temp[10][i];
        }
        int index = 0;
        for (int i = 0; i < 10; i++) {
            if (!(temp[i].empty())) {
                for (int j = 0; j < temp[i].size(); j++) {
                    pairs[temp[10].size() + index] = temp[i][j];
                    index++;
                }
            }
            temp[i].clear();
        }
        temp[10].clear();
    }
}

int main() {
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

    return 0;
}