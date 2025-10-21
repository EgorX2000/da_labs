#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

enum patternSymbol { GREATER, LESS, PATTERN_FOUND };

int comparePattern(const std::string& text, int pos,
                   const std::string& pattern) {
    int n = text.size(), m = pattern.size();
    int len = std::min(n - pos, m);
    for (int i = 0; i < len; i++) {
        if (text[pos + i] < pattern[i]) return -1;
        if (text[pos + i] > pattern[i]) return 1;
    }
    if (n - pos < m) return -1;
    return 0;
}

std::pair<int, int> findRange(const std::vector<int>& sa,
                              const std::string& text,
                              const std::string& pattern) {
    int l = 0, r = sa.size();
    while (l < r) {
        int mid = (l + r) / 2;
        if (comparePattern(text, sa[mid], pattern) < 0)
            l = mid + 1;
        else
            r = mid;
    }
    int start = l;
    l = 0;
    r = sa.size();
    while (l < r) {
        int mid = (l + r) / 2;
        if (comparePattern(text, sa[mid], pattern) <= 0)
            l = mid + 1;
        else
            r = mid;
    }
    return {start, l};
}

std::vector<int> countingSort(const std::vector<int>& indices,
                              const std::vector<int>& ec) {
    int n = indices.size();
    int classes = *std::max_element(ec.begin(), ec.end()) + 1;
    std::vector<int> cnt(classes, 0);
    for (int i = 0; i < n; i++) cnt[ec[indices[i]]]++;
    std::vector<int> pos(classes, 0);
    for (int i = 1; i < classes; i++) pos[i] = pos[i - 1] + cnt[i - 1];
    std::vector<int> res(n);
    for (int i = 0; i < n; i++) {
        int cl = ec[indices[i]];
        res[pos[cl]] = indices[i];
        pos[cl]++;
    }
    return res;
}

int main() {
    std::string input;
    std::cin >> input;
    int arraySize = input.size();

    input.push_back('$');
    int n = input.size();

    std::vector<std::pair<std::string, int>> str;
    for (int i = 0; i < n; i++) str.push_back({std::string{input[i]}, i});
    std::sort(str.begin(), str.end());

    std::vector<int> ec(n, 0);
    ec[str[0].second] = 0;
    for (int i = 1; i < n; i++) {
        if (str[i].first != str[i - 1].first)
            ec[str[i].second] = ec[str[i - 1].second] + 1;
        else
            ec[str[i].second] = ec[str[i - 1].second];
    }

    std::vector<int> indices(n);
    for (int i = 0; i < n; i++) indices[i] = str[i].second;

    for (int k = 1; k < n; k *= 2) {
        for (int i = 0; i < n; i++) indices[i] = (indices[i] - k + n) % n;
        indices = countingSort(indices, ec);

        std::vector<int> ec_new(n);
        ec_new[indices[0]] = 0;
        for (int i = 1; i < n; i++) {
            int prev1 = ec[indices[i - 1]];
            int prev2 = ec[(indices[i - 1] + k) % n];
            int cur1 = ec[indices[i]];
            int cur2 = ec[(indices[i] + k) % n];
            ec_new[indices[i]] =
                ec_new[indices[i - 1]] + (prev1 != cur1 || prev2 != cur2);
        }
        ec = ec_new;
        if (*std::max_element(ec.begin(), ec.end()) == n - 1) break;
    }

    std::vector<int> suffixArray;
    for (int i : indices)
        if (i < arraySize) suffixArray.push_back(i);

    std::string pattern;
    int patternNumber = 1;
    std::string originalText = input.substr(0, arraySize);
    while (std::cin >> pattern) {
        auto range = findRange(suffixArray, originalText, pattern);
        int start = range.first, end = range.second;
        if (start != end) {
            std::vector<int> pos;
            for (int i = start; i < end; i++) pos.push_back(suffixArray[i] + 1);
            std::sort(pos.begin(), pos.end());
            std::cout << patternNumber << ": ";
            for (size_t i = 0; i < pos.size(); i++) {
                if (i) std::cout << ", ";
                std::cout << pos[i];
            }
            std::cout << "\n";
        }
        patternNumber++;
    }
    return 0;
}
