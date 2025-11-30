// InvertedIndex.h
#pragma once
#include <string>
#include <vector>
#include <map>
#include <set>
#include <shared_mutex>
#include <algorithm>

class InvertedIndex {
private:
    std::map<std::string, std::set<std::string>> index;

    // Ќќ¬≈: «бер≥гаЇмо ун≥кальн≥ ≥мена файл≥в
    std::set<std::string> processedFiles;

    mutable std::shared_mutex mtx;

public:
    void add(const std::string& word, const std::string& filename);
    std::vector<std::string> search(const std::string& word) const;
    size_t getSize() const;

    // Ќќ¬≈: ћетод, щоб д≥знатис€ к≥льк≥сть про≥ндексованих файл≥в
    int getFilesCount() const;

    static std::string cleanWord(const std::string& word);
};