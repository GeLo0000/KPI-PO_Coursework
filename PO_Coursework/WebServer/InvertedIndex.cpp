// InvertedIndex.cpp
#include "InvertedIndex.h"

void InvertedIndex::add(const std::string& word, const std::string& filename) {
    std::unique_lock<std::shared_mutex> lock(mtx);
    index[word].insert(filename);
}

std::vector<std::string> InvertedIndex::search(const std::string& word) const {
    std::shared_lock<std::shared_mutex> lock(mtx);
    std::vector<std::string> result;
    auto it = index.find(word);
    if (it != index.end()) {
        for (const auto& file : it->second) {
            result.push_back(file);
        }
    }
    return result;
}

size_t InvertedIndex::getSize() const {
    std::shared_lock<std::shared_mutex> lock(mtx);
    return index.size();
}

std::string InvertedIndex::cleanWord(const std::string& word) {
    std::string clean;
    for (char c : word) {
        if (isalnum((unsigned char)c)) {
            clean += tolower((unsigned char)c);
        }
    }
    return clean;
}