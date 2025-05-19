#include <iostream>
#include <queue>
#include <sstream>
#include <string>
#include <vector>

struct SubPattern {
    std::vector<unsigned int> word;
    size_t jokerNumber;
    size_t index;
    size_t offset;
};

struct Node {
    unsigned int value;
    std::vector<Node*> children;
    Node* failureLink;
    bool isLeaf;
    size_t spIndex;
    size_t spOffset;
    size_t depth = 0;
};

struct AKResult {
    size_t index;
    size_t offset;
};

class Trie {
   public:
    Trie() {
        root = new Node();
        root->value = 0;
        root->isLeaf = false;
    }

    ~Trie() { deleteSubtree(root); }

    void insert(const std::vector<unsigned int>& word, size_t spIndex,
                size_t spOffset) {
        Node* current = root;
        for (unsigned int num : word) {
            Node* child = findChild(current, num);
            if (!child) {
                child = new Node();
                child->value = num;
                child->isLeaf = false;
                current->children.push_back(child);
            }
            current = child;
        }
        current->isLeaf = true;
        current->spIndex = spIndex;
        current->spOffset = spOffset;
    }

    std::vector<AKResult> searchAhoCorasick(
        const std::vector<unsigned int>& text) {
        buildFailureLinks();

        AKResult res;
        std::vector<AKResult> matches;
        Node* current = root;

        for (size_t i = 0; i < text.size(); ++i) {
            unsigned int c = text[i];
            while (current && !findChild(current, c)) {
                current = current->failureLink;
            }
            current = current ? findChild(current, c) : root;

            for (Node* node = current; node != root; node = node->failureLink) {
                if (node->isLeaf) {
                    res.index = i - node->depth - 1;
                    res.offset = node->spOffset;
                    matches.push_back(res);
                }
            }
        }

        return matches;
    }

   private:
    Node* root;

    void deleteSubtree(Node* node) {
        if (!node) return;
        for (Node* child : node->children) {
            deleteSubtree(child);
        }
        delete node;
    }

    Node* findChild(Node* node, unsigned int val) const {
        if (!node) return nullptr;
        for (Node* child : node->children) {
            if (child->value == val) return child;
        }
        return nullptr;
    }

    void buildFailureLinks() {
        std::queue<Node*> q;
        root->failureLink = nullptr;
        q.push(root);

        while (!q.empty()) {
            Node* current = q.front();
            q.pop();

            for (Node* child : current->children) {
                Node* failureLink = current->failureLink;
                while (failureLink && !findChild(failureLink, child->value)) {
                    failureLink = failureLink->failureLink;
                }
                child->failureLink =
                    failureLink ? findChild(failureLink, child->value) : root;
                q.push(child);
            }
        }
    }
};

int main() {
    Trie trie;

    std::vector<SubPattern> pattern;
    SubPattern sp;
    size_t spIndex = 0;

    std::string line;
    std::getline(std::cin, line);
    std::istringstream iss(line);

    bool wasJoker = false;
    size_t count = 0;

    std::string letter;
    while (iss >> letter) {
        if (letter == "?") {
            wasJoker = true;
            count++;
        } else {
            if (wasJoker) {
                sp.jokerNumber = count;
                wasJoker = false;
                count = 0;

                sp.index = spIndex;
                pattern.push_back(sp);
                spIndex++;
                sp.word.clear();
                sp.jokerNumber = 0;
            }
            std::istringstream iss(letter);
            unsigned int letterInt;
            iss >> letterInt;
            sp.word.push_back(letterInt);
        }
    }
    sp.jokerNumber = count;
    sp.index = spIndex;
    pattern.push_back(sp);

    size_t patternLength = 0;
    for (SubPattern sp : pattern) {
        sp.offset = patternLength;
        patternLength += sp.word.size() + sp.jokerNumber;

        trie.insert(sp.word, sp.index, sp.offset);
    }

    std::vector<unsigned int> text;
    unsigned int num;
    while (std::cin >> num) {
        text.push_back(num);
    }

    std::vector<AKResult> matches = trie.searchAhoCorasick(text);

    std::vector<size_t> votes(matches.size(), 0);
    for (AKResult match : matches) {
        votes[match.index - match.offset + 1]++;
    }

    for (size_t i = 0; i < votes.size(); i++) {
        if (votes[i] == pattern.size()) {
            std::cout << i + 1 << std::endl;
        }
    }

    return 0;
}