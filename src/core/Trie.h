#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <cctype>

class TrieNode {
public:
    std::unordered_map<char, TrieNode*> children;
    std::vector<int> placeIds; // IDs of places ending at or under this prefix
    bool isEndOfWord;
    
    TrieNode() : isEndOfWord(false) {}
    ~TrieNode() {
        for (auto& pair : children) {
            delete pair.second;
        }
    }
};

class Trie {
private:
    TrieNode* root;

    std::string toLowerAndClean(const std::string& s) const {
        std::string res;
        for (char c : s) {
            if (!std::isspace(c)) {
                res += std::tolower(c);
            }
        }
        return res;
    }

    void collectIds(TrieNode* node, std::vector<int>& results) const {
        if (!node) return;
        for (int id : node->placeIds) {
            results.push_back(id);
        }
    }

public:
    Trie() {
        root = new TrieNode();
    }
    ~Trie() {
        delete root;
    }

    void insert(const std::string& name, int placeId) {
        std::string word = toLowerAndClean(name);
        TrieNode* curr = root;
        for (char c : word) {
            if (curr->children.find(c) == curr->children.end()) {
                curr->children[c] = new TrieNode();
            }
            curr = curr->children[c];
            // Store place ID at each node for fast prefix retrieval
            curr->placeIds.push_back(placeId);
        }
        curr->isEndOfWord = true;
    }

    std::vector<int> searchPrefix(const std::string& prefix) const {
        std::string word = toLowerAndClean(prefix);
        TrieNode* curr = root;
        for (char c : word) {
            if (curr->children.find(c) == curr->children.end()) {
                return {};
            }
            curr = curr->children[c];
        }
        return curr->placeIds;
    }
};
