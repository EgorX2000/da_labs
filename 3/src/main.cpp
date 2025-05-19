#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <map>
#include <random>
#include <sstream>
#include <string>
#include <vector>

const int T = 2;
const int MIN_ELEMS = T - 1;
const int MAX_ELEMS = 2 * T - 1;
const int MAX_CHILDREN = 2 * T;

struct NodeElem {
    std::string key;
    unsigned long long value;
};

struct Node {
    bool isLeaf;
    int elemNumber;
    NodeElem elems[MAX_ELEMS];
    Node* children[MAX_CHILDREN];
};

class BTree {
   private:
    Node* root;
    int t;

    bool strLess(const std::string& a, const std::string& b) {
        size_t len = std::min(a.size(), b.size());
        for (size_t i = 0; i < len; ++i) {
            char ca = std::tolower(a[i]);
            char cb = std::tolower(b[i]);
            if (ca < cb) return true;
            if (ca > cb) return false;
        }
        return a.size() < b.size();
    }

    bool strEqual(const std::string& a, const std::string& b) {
        if (a.size() != b.size()) return false;
        for (size_t i = 0; i < a.size(); ++i) {
            if (std::tolower(a[i]) != std::tolower(b[i])) return false;
        }
        return true;
    }

    Node* createNode() {
        Node* newNode = new Node();
        newNode->isLeaf = true;
        newNode->elemNumber = 0;
        for (int i = 0; i < MAX_CHILDREN; ++i) {
            newNode->children[i] = nullptr;
        }
        return newNode;
    }

    void splitNode(Node* parent, int index) {
        Node* child = parent->children[index];
        Node* newChild = createNode();
        newChild->isLeaf = child->isLeaf;
        newChild->elemNumber = t - 1;
        for (int i = 0; i < t - 1; ++i) {
            newChild->elems[i] = child->elems[t + i];
        }
        if (!child->isLeaf) {
            for (int i = 0; i < t; ++i) {
                newChild->children[i] = child->children[t + i];
            }
        }
        child->elemNumber = t - 1;
        NodeElem middle = child->elems[t - 1];

        for (int i = parent->elemNumber; i > index; --i) {
            parent->elems[i] = parent->elems[i - 1];
            parent->children[i + 1] = parent->children[i];
        }
        parent->elems[index] = middle;
        parent->children[index + 1] = newChild;
        parent->elemNumber++;
    }

    void insertElem(Node* node, const NodeElem& elem) {
        if (node->isLeaf) {
            int i = node->elemNumber - 1;
            while (i >= 0 && strLess(elem.key, node->elems[i].key)) {
                node->elems[i + 1] = node->elems[i];
                i--;
            }
            node->elems[i + 1] = elem;
            node->elemNumber++;
        } else {
            int i = 0;
            while (i < node->elemNumber &&
                   strLess(node->elems[i].key, elem.key)) {
                i++;
            }
            Node* child = node->children[i];
            if (child->elemNumber == 2 * t - 1) {
                splitNode(node, i);
                if (strLess(elem.key, node->elems[i].key)) {
                    child = node->children[i];
                } else {
                    child = node->children[i + 1];
                }
            }
            insertElem(child, elem);
        }
    }

    void deleteNode(Node* node) {
        if (node == nullptr) return;
        if (!node->isLeaf) {
            for (int i = 0; i <= node->elemNumber; ++i) {
                deleteNode(node->children[i]);
            }
        }
        delete node;
    }

    void saveNode(std::ostream& os, Node* node) {
        if (node == nullptr) return;
        for (int i = 0; i < node->elemNumber; ++i) {
            os << node->elems[i].key << " " << node->elems[i].value << "\n";
        }
        if (!node->isLeaf) {
            for (int i = 0; i <= node->elemNumber; ++i) {
                saveNode(os, node->children[i]);
            }
        }
    }

    NodeElem getMaxLeft(Node* node, int index) {
        Node* current = node->children[index];
        while (!current->isLeaf) {
            current = current->children[current->elemNumber];
        }
        return current->elems[current->elemNumber - 1];
    }

    bool removeElem(Node* node, const std::string& key, Node* parent,
                    int indexInParent) {
        int index = 0;
        while (index < node->elemNumber &&
               strLess(node->elems[index].key, key)) {
            index++;
        }
        bool found =
            (index < node->elemNumber && strEqual(node->elems[index].key, key));
        if (node->isLeaf) {
            if (found) {
                for (int i = index; i < node->elemNumber - 1; ++i) {
                    node->elems[i] = node->elems[i + 1];
                }
                node->elemNumber--;
                if (parent != nullptr && node->elemNumber < MIN_ELEMS) {
                    balanceNodes(node, parent, indexInParent);
                }
                return true;
            }
            return false;
        } else {
            if (found) {
                NodeElem maxLeft = getMaxLeft(node, index);
                node->elems[index] = maxLeft;
                bool result =
                    removeElem(node->children[index], maxLeft.key, node, index);
                if (node->elemNumber < MIN_ELEMS && parent != nullptr) {
                    balanceNodes(node, parent, indexInParent);
                }
                return result;
            } else {
                int childIndex = index;
                if (index < node->elemNumber &&
                    !strLess(key, node->elems[index].key)) {
                    childIndex = index + 1;
                }
                bool result = removeElem(node->children[childIndex], key, node,
                                         childIndex);
                if (node->elemNumber < MIN_ELEMS && parent != nullptr) {
                    balanceNodes(node, parent, indexInParent);
                }
                return result;
            }
        }
    }

    void balanceNodes(Node* node, Node* parent, int index) {
        if (node->elemNumber >= MIN_ELEMS) return;
        if (index > 0) {
            Node* leftSibling = parent->children[index - 1];
            if (leftSibling->elemNumber > MIN_ELEMS) {
                for (int i = node->elemNumber; i > 0; i--) {
                    node->elems[i] = node->elems[i - 1];
                }
                node->elems[0] = parent->elems[index - 1];
                parent->elems[index - 1] =
                    leftSibling->elems[leftSibling->elemNumber - 1];
                leftSibling->elemNumber--;
                node->elemNumber++;
                if (!node->isLeaf) {
                    node->children[node->elemNumber] =
                        node->children[node->elemNumber - 1];
                    for (int i = node->elemNumber - 1; i > 0; i--) {
                        node->children[i] = node->children[i - 1];
                    }
                    node->children[0] =
                        leftSibling->children[leftSibling->elemNumber + 1];
                }
                return;
            }
        }
        if (index < parent->elemNumber) {
            Node* rightSibling = parent->children[index + 1];
            if (rightSibling->elemNumber > MIN_ELEMS) {
                node->elems[node->elemNumber] = parent->elems[index];
                parent->elems[index] = rightSibling->elems[0];
                for (int i = 0; i < rightSibling->elemNumber - 1; ++i) {
                    rightSibling->elems[i] = rightSibling->elems[i + 1];
                }
                rightSibling->elemNumber--;
                node->elemNumber++;
                if (!node->isLeaf) {
                    node->children[node->elemNumber] =
                        rightSibling->children[0];
                    for (int i = 0; i <= rightSibling->elemNumber; i++) {
                        rightSibling->children[i] =
                            rightSibling->children[i + 1];
                    }
                }
                return;
            }
        }

        if (index > 0) {
            Node* leftSibling = parent->children[index - 1];
            leftSibling->elems[leftSibling->elemNumber] =
                parent->elems[index - 1];
            leftSibling->elemNumber++;
            for (int i = 0; i < node->elemNumber; ++i) {
                leftSibling->elems[leftSibling->elemNumber + i] =
                    node->elems[i];
            }
            if (!node->isLeaf) {
                for (int i = 0; i <= node->elemNumber; ++i) {
                    leftSibling->children[leftSibling->elemNumber + i] =
                        node->children[i];
                }
            }
            leftSibling->elemNumber += node->elemNumber;
            for (int i = index - 1; i < parent->elemNumber - 1; ++i) {
                parent->elems[i] = parent->elems[i + 1];
                parent->children[i + 1] = parent->children[i + 2];
            }
            parent->elemNumber--;
            delete node;
        } else {
            Node* rightSibling = parent->children[index + 1];
            node->elems[node->elemNumber] = parent->elems[index];
            node->elemNumber++;
            for (int i = 0; i < rightSibling->elemNumber; ++i) {
                node->elems[node->elemNumber + i] = rightSibling->elems[i];
            }
            if (!node->isLeaf) {
                for (int i = 0; i <= rightSibling->elemNumber; ++i) {
                    node->children[node->elemNumber + i] =
                        rightSibling->children[i];
                }
            }
            node->elemNumber += rightSibling->elemNumber;
            for (int i = index; i < parent->elemNumber - 1; ++i) {
                parent->elems[i] = parent->elems[i + 1];
                parent->children[i + 1] = parent->children[i + 2];
            }
            parent->elemNumber--;
            delete rightSibling;
        }
    }

   public:
    BTree(int tVal) : t(tVal), root(nullptr) {}

    ~BTree() { deleteNode(root); }

    unsigned long long* find(const std::string& key) {
        Node* current = root;
        while (current != nullptr) {
            for (int i = 0; i < current->elemNumber; ++i) {
                if (strEqual(current->elems[i].key, key)) {
                    return &current->elems[i].value;
                }
            }
            if (current->isLeaf) {
                return nullptr;
            }
            int childIndex = 0;
            for (int i = 0; i < current->elemNumber; ++i) {
                if (strLess(key, current->elems[i].key)) {
                    break;
                }
                childIndex = i + 1;
            }
            current = current->children[childIndex];
        }
        return nullptr;
    }

    void insert(const std::string& key, unsigned long long value) {
        if (root == nullptr) {
            root = createNode();
        }
        if (root->elemNumber == 2 * t - 1) {
            Node* newRoot = createNode();
            newRoot->isLeaf = false;
            newRoot->children[0] = root;
            splitNode(newRoot, 0);
            insertElem(newRoot, {key, value});
            root = newRoot;
        } else {
            insertElem(root, {key, value});
        }
    }

    bool remove(const std::string& key) {
        if (root == nullptr) return false;
        bool removed = removeElem(root, key, nullptr, 0);
        if (removed && root->elemNumber == 0 && !root->isLeaf) {
            Node* oldRoot = root;
            root = root->children[0];
            delete oldRoot;
        }
        return removed;
    }

    bool save(const std::string& path) {
        std::ofstream os(path);
        saveNode(os, root);
        return true;
    }

    static BTree* load(const std::string& path) {
        std::ifstream is(path);
        BTree* tree = new BTree(T);
        std::string line;
        while (std::getline(is, line)) {
            std::istringstream iss(line);
            std::string key;
            unsigned long long value;
            if (iss >> key >> value) {
                tree->insert(key, value);
            }
        }
        return tree;
    }
};

int main() {
    BTree* tree = new BTree(T);
    std::string line;

    while (std::getline(std::cin, line)) {
        std::istringstream iss(line);
        std::string command;
        iss >> command;

        if (command == "+") {
            std::string word;
            unsigned long long number;
            if (iss >> word >> number) {
                if (tree->find(word) != nullptr) {
                    // std::cout << "Exist" << std::endl;
                } else {
                    tree->insert(word, number);
                    // std::cout << "OK" << std::endl;
                }
            }
        } else if (command == "-") {
            std::string word;
            if (iss >> word) {
                if (tree->remove(word)) {
                    // std::cout << "OK" << std::endl;
                } else {
                    // std::cout << "NoSuchWord" << std::endl;
                }
            }
        } else if (command == "!") {
            std::string subcommand;
            iss >> subcommand;
            std::string path;
            iss >> path;
            if (subcommand == "Save") {
                tree->save(path);
                // std::cout << "OK" << std::endl;
            } else if (subcommand == "Load") {
                BTree* newTree = BTree::load(path);
                newTree;
                delete tree;
                tree = newTree;
                // std::cout << "OK" << std::endl;
            }
        } else {
            std::string word = command;
            unsigned long long* value = tree->find(word);
            if (value) {
                // std::cout << "OK: " << *value << std::endl;
            } else {
                // std::cout << "NoSuchWord" << std::endl;
            }
        }
    }

    delete tree;

    return 0;
}