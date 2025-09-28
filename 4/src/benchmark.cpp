#include <chrono>
#include <fstream>
#include <iostream>
#include <queue>
#include <random>
#include <sstream>
#include <string>
#include <vector>

// Структуры из вашего исходного кода
struct Seg {
    std::vector<unsigned int> word;
    size_t offset;
};

struct Node {
    unsigned int value;
    std::vector<Node*> children;
    Node* failureLink = nullptr;
    std::vector<size_t> spOffsets;
    size_t depth = 0;
};

struct TextPosition {
    size_t lineIndex;
    size_t wordIndex;
};

struct AKItem {
    size_t index;
    TextPosition pos;
};

struct TextItem {
    unsigned int value;
    TextPosition pos;
};

// Класс Trie для алгоритма Ахо-Корасика
class Trie {
   public:
    Trie() {
        root = new Node();
        root->depth = 0;
    }
    ~Trie() { deleteSubtree(root); }

    void insert(const std::vector<unsigned int>& word, size_t spOffset) {
        Node* cur = root;
        for (unsigned int num : word) {
            Node* nxt = findChild(cur, num);
            if (!nxt) {
                nxt = new Node();
                nxt->value = num;
                nxt->depth = cur->depth + 1;
                cur->children.push_back(nxt);
            }
            cur = nxt;
        }
        cur->spOffsets.push_back(spOffset);
    }

    std::vector<AKItem> searchAhoCorasick(const std::vector<TextItem>& text) {
        buildFailureLinks();
        std::vector<AKItem> matches;
        Node* cur = root;
        size_t n = text.size();

        for (size_t i = 0; i < n; ++i) {
            unsigned int c = text[i].value;
            while (cur != root && !findChild(cur, c)) cur = cur->failureLink;
            if (auto nxt = findChild(cur, c)) cur = nxt;

            for (Node* node = cur; node != root; node = node->failureLink) {
                if (node->spOffsets.empty()) continue;

                for (size_t off : node->spOffsets) {
                    size_t segLen = node->depth;
                    size_t segStart = i + 1 - segLen;
                    if (segStart < off) continue;
                    size_t fullStart = segStart - off;
                    if (fullStart + patLen > n) continue;
                    matches.push_back({fullStart, text[fullStart].pos});
                }
            }
        }
        return matches;
    }

    void setPatternLength(size_t L) { patLen = L; }

   private:
    Node* root;
    size_t patLen = 0;

    Node* findChild(Node* node, unsigned int val) const {
        for (Node* ch : node->children)
            if (ch->value == val) return ch;
        return nullptr;
    }

    void buildFailureLinks() {
        std::queue<Node*> q;
        root->failureLink = root;
        for (Node* ch : root->children) {
            ch->failureLink = root;
            q.push(ch);
        }
        while (!q.empty()) {
            Node* v = q.front();
            q.pop();
            for (Node* ch : v->children) {
                Node* f = v->failureLink;
                while (f != root && !findChild(f, ch->value))
                    f = f->failureLink;
                if (auto ptr = findChild(f, ch->value))
                    ch->failureLink = ptr;
                else
                    ch->failureLink = root;
                q.push(ch);
            }
        }
    }

    void deleteSubtree(Node* node) {
        if (!node) return;
        for (Node* ch : node->children) deleteSubtree(ch);
        delete node;
    }
};

// Алгоритм полного перебора
std::vector<AKItem> searchWithBruteForce(const std::vector<TextItem>& text,
                                         const std::string& pattern) {
    std::vector<AKItem> matches;
    std::istringstream iss(pattern);
    std::vector<std::string> tokens;
    std::string token;
    while (iss >> token) tokens.push_back(token);
    size_t patLen = tokens.size();
    size_t textLen = text.size();

    // Проверяем каждую возможную позицию в тексте
    for (size_t i = 0; i <= textLen - patLen; ++i) {
        bool match = true;
        for (size_t j = 0; j < patLen; ++j) {
            if (tokens[j] == "?") continue;  // Джокер совпадает с любым числом
            if (std::stoul(tokens[j]) != text[i + j].value) {
                match = false;
                break;
            }
        }
        if (match) {
            matches.push_back({i, text[i].pos});
        }
    }
    return matches;
}

// Генерация случайного образца
std::string generatePattern(size_t patLen, size_t numJokers) {
    std::string pattern;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 100);  // Диапазон чисел
    size_t jokersPlaced = 0;
    for (size_t i = 0; i < patLen; ++i) {
        if (jokersPlaced < numJokers &&
            (patLen - i - 1) >= (numJokers - jokersPlaced)) {
            if (dis(gen) % 2 == 0) {
                pattern += "? ";
                ++jokersPlaced;
                continue;
            }
        }
        pattern += std::to_string(dis(gen)) + " ";
    }
    return pattern;
}

// Генерация случайного текста
std::vector<std::vector<unsigned int>> generateText(size_t numLines,
                                                    size_t wordsPerLine) {
    std::vector<std::vector<unsigned int>> text;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 100);  // Диапазон чисел
    for (size_t i = 0; i < numLines; ++i) {
        std::vector<unsigned int> line;
        for (size_t j = 0; j < wordsPerLine; ++j) {
            line.push_back(dis(gen));
        }
        text.push_back(line);
    }
    return text;
}

// Преобразование текста в формат TextItem
std::vector<TextItem> textToTextItems(
    const std::vector<std::vector<unsigned int>>& textLines) {
    std::vector<TextItem> text;
    for (size_t i = 0; i < textLines.size(); ++i) {
        for (size_t j = 0; j < textLines[i].size(); ++j) {
            text.push_back({textLines[i][j], {i, j}});
        }
    }
    return text;
}

// Генерация сегментов из образца
std::vector<Seg> generateSegments(const std::string& pattern) {
    std::vector<Seg> segs;
    std::istringstream iss(pattern);
    std::string token;
    size_t offset = 0;
    while (iss >> token) {
        if (token == "?") {
            ++offset;
        } else {
            std::vector<unsigned int> word;
            size_t startOff = offset;
            word.push_back(std::stoul(token));
            while (iss >> token && token != "?") {
                word.push_back(std::stoul(token));
                ++offset;
            }
            segs.push_back({word, startOff});
            if (token == "?") ++offset;
        }
    }
    return segs;
}

// Сравнение производительности алгоритмов с возрастающими размерами тестов
void compareAlgorithms(size_t numTests, const std::string& outputFile) {
    std::ofstream csvFile(outputFile);
    if (!csvFile.is_open()) {
        std::cerr << "Не удалось открыть файл для записи: " << outputFile
                  << std::endl;
        return;
    }
    csvFile << "AhoCorasickTime,BruteForceTime\n";

    for (size_t test = 0; test < numTests; ++test) {
        // Линейное увеличение размеров тестов
        size_t patLen = 10 + (90 * test) / (numTests - 1);   // От 10 до 100
        size_t numJokers = 1 + (9 * test) / (numTests - 1);  // От 1 до 10
        size_t numLines =
            100 + (900 * test) / (numTests - 1);  // От 100 до 1000
        size_t wordsPerLine =
            100 + (900 * test) / (numTests - 1);  // От 100 до 1000

        std::string pattern = generatePattern(patLen, numJokers);
        auto textLines = generateText(numLines, wordsPerLine);
        auto text = textToTextItems(textLines);
        auto segs = generateSegments(pattern);

        double acTime, bruteForceTime;

        // Измерение времени для Ахо-Корасика
        {
            auto start = std::chrono::high_resolution_clock::now();
            Trie trie;
            trie.setPatternLength(patLen);
            for (const auto& s : segs) {
                trie.insert(s.word, s.offset);
            }
            trie.searchAhoCorasick(text);
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> diff = end - start;
            acTime = diff.count();
        }

        // Измерение времени для полного перебора
        {
            auto start = std::chrono::high_resolution_clock::now();
            searchWithBruteForce(text, pattern);
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> diff = end - start;
            bruteForceTime = diff.count();
        }

        csvFile << acTime << "," << bruteForceTime << "\n";
    }
    csvFile.close();
    std::cout << "Результаты записаны в " << outputFile << std::endl;
}

int main() {
    size_t numTests = 10;  // Количество тестов
    compareAlgorithms(numTests, "benchmark_results.csv");
    return 0;
}