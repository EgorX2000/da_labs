#include <algorithm>
#include <iostream>
#include <set>
#include <string>
#include <vector>

enum patternSymbol { GREATER, LESS, PATTERN_FOUND };

void addSentinel(std::string& str) {
    while ((str.size() & (str.size() - 1)) != 0) {
        str.push_back('$');
    }
}

patternSymbol unmatchingSymbol(const std::string& pattern,
                               const std::string& text) {
    for (size_t i = 0; i < pattern.size(); i++) {
        if (std::tolower(pattern[i]) > std::tolower(text[i])) {
            return GREATER;
        } else if (std::tolower(pattern[i]) < std::tolower(text[i])) {
            return LESS;
        }
    }
    return PATTERN_FOUND;
}

std::vector<int> binarySearch(std::vector<int>& suffixArray,
                              const std::string& pattern,
                              const std::string& text) {
    int l = 0, r = suffixArray.size() - 1;
    std::vector<int> result;

    while (l <= r) {
        int m = l + (r - l) / 2;
        patternSymbol ps =
            unmatchingSymbol(pattern, text.substr(suffixArray[m]));
        if (ps == PATTERN_FOUND) {
            result.push_back(suffixArray[m]);

            int i = m - 1;
            while (i >= 0 &&
                   unmatchingSymbol(pattern, text.substr(suffixArray[i])) ==
                       PATTERN_FOUND) {
                result.push_back(suffixArray[i]);
                i--;
            }

            i = m + 1;
            while (i < suffixArray.size() &&
                   unmatchingSymbol(pattern, text.substr(suffixArray[i])) ==
                       PATTERN_FOUND) {
                result.push_back(suffixArray[i]);
                i++;
            }

            break;
        } else if (ps == GREATER) {
            l = m + 1;
        } else {
            r = m - 1;
        }
    }

    std::stable_sort(result.begin(), result.end());
    return result;
}

int main() {
    std::string input;
    std::cin >> input;
    int arraySize = input.size();

    addSentinel(input);
    int n = input.size();

    std::vector<std::pair<std::string, int>> str;
    for (int i = 0; i < n; i++) {
        str.push_back({std::string{input[i]}, i});
    }
    std::stable_sort(str.begin(), str.end());

    std::vector<int> ec(n, 0);
    ec[str[0].second] = 1;
    for (int i = 1; i < n; i++) {
        if (str[i].first != str[i - 1].first)
            ec[str[i].second] = ec[str[i - 1].second] + 1;
        else
            ec[str[i].second] = ec[str[i - 1].second];
    }

    std::vector<int> indices(n);
    for (int i = 0; i < n; i++) {
        indices[i] = str[i].second;
    }

    for (int k = 1; k < n; k *= 2) {
        for (int i = 0; i < n; i++) {
            indices[i] = (indices[i] - k + n) % n;
        }

        std::stable_sort(indices.begin(), indices.end(),
                         [&](int a, int b) { return ec[a] < ec[b]; });

        std::vector<int> ec_new(n, 0);
        ec_new[indices[0]] = 1;
        for (int i = 1; i < n; i++) {
            int prev1 = ec[indices[i - 1]];
            int prev2 = ec[(indices[i - 1] + k) % n];
            int cur1 = ec[indices[i]];
            int cur2 = ec[(indices[i] + k) % n];
            if (cur1 != prev1 || cur2 != prev2)
                ec_new[indices[i]] = ec_new[indices[i - 1]] + 1;
            else
                ec_new[indices[i]] = ec_new[indices[i - 1]];
        }
        ec = ec_new;

        if (*std::max_element(ec.begin(), ec.end()) == n) break;
    }

    std::vector<int> suffixArray;
    for (int i : indices) {
        if (i < arraySize) suffixArray.push_back(i);
    }

    std::string pattern;
    int patternNumber = 1;
    while (std::cin >> pattern) {
        std::vector<int> positions = binarySearch(suffixArray, pattern, input);
        std::cout << patternNumber << ": ";
        for (size_t i = 0; i < positions.size(); i++) {
            std::cout << positions[i] + 1;
            if (i + 1 < positions.size()) {
                std::cout << ", ";
            }
        }
        std::cout << std::endl;
        patternNumber++;
    }
    return 0;
}