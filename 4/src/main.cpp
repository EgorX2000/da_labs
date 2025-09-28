#include <iostream>
#include <queue>
#include <sstream>
#include <string>
#include <vector>

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

int main() {
    std::string line;
    if (!std::getline(std::cin, line)) return 0;
    std::istringstream iss(line);
    std::vector<std::string> tokens;
    std::string tok;
    while (iss >> tok) tokens.push_back(tok);

    size_t patLen = tokens.size();

    std::vector<Seg> segs;
    size_t offset = 0;
    for (size_t i = 0; i < patLen; ++i) {
        if (tokens[i] == "?") {
            ++offset;
        } else {
            std::vector<unsigned int> w;
            size_t startOff = offset;
            while (i < patLen && tokens[i] != "?") {
                w.push_back(std::stoul(tokens[i]));
                ++i;
                ++offset;
            }
            segs.push_back({w, startOff});
            --i;
        }
    }

    std::vector<TextItem> text;
    size_t lineIdx = 0;
    while (std::getline(std::cin, line)) {
        std::istringstream ts(line);
        unsigned int v;
        size_t col = 0;
        while (ts >> v) {
            text.push_back({v, TextPosition{lineIdx, col}});
            ++col;
        }
        ++lineIdx;
    }

    if (segs.empty()) {
        for (size_t i = 0, N = text.size(); i + patLen <= N; ++i) {
            auto p = text[i].pos;
            std::cout << p.lineIndex + 1 << ", " << p.wordIndex + 1 << "\n";
        }
        return 0;
    }

    Trie trie;
    trie.setPatternLength(patLen);
    for (auto& s : segs) trie.insert(s.word, s.offset);

    auto matches = trie.searchAhoCorasick(text);

    size_t N = text.size(), numSegs = segs.size();
    std::vector<size_t> cnt(N, 0);
    std::vector<TextPosition> startPos(N);
    std::vector<bool> seen(N, false);

    for (auto& m : matches) {
        ++cnt[m.index];
        if (!seen[m.index]) {
            startPos[m.index] = m.pos;
            seen[m.index] = true;
        }
    }

    for (size_t i = 0; i < N; ++i) {
        if (cnt[i] == numSegs) {
            auto p = startPos[i];
            std::cout << p.lineIndex + 1 << ", " << p.wordIndex + 1 << "\n";
        }
    }

    return 0;
}
