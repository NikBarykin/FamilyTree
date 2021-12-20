#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <string_view>
#include <vector>
#include <chrono>
#include <unordered_set>
#include <sstream>


std::vector<std::string> Split(std::string_view sv, const std::string& delimiter=" ");


template<class T, class THash, class TEqual>
std::unordered_set<T, THash, TEqual> UnorderedSetIntersection(const std::unordered_set<T, THash, TEqual>& lhs,
                                                              const std::unordered_set<T, THash, TEqual>& rhs) {
    if (rhs.size() < lhs.size()) {
        return UnorderedSetIntersection(rhs, lhs);
    }
    std::unordered_set<T, THash, TEqual> result;
    result.reserve(lhs.size());
    for (const T& el : lhs) {
        if (rhs.count(el)) {
            result.insert(el);
        }
    }
    return result;
}


template<class T, class THash, class TEqual>
bool IsSubset(const std::unordered_set<T, THash, TEqual>& potential_subset,
              const std::unordered_set<T, THash, TEqual>& target_set) {
    for (const T& el : potential_subset) {
        if (!target_set.count(el)) {
            return false;
        }
    }
    return true;
}


template<class T, class THash, class TEqual>
bool UnorderedSetEqual(const std::unordered_set<T, THash, TEqual>& lhs,
                             const std::unordered_set<T, THash, TEqual>& rhs) {
    return IsSubset(lhs, rhs) && IsSubset(rhs, lhs);
}


std::string ReadEverythingFromFile(const std::string& filename);


template<typename InputIterator>
void PrintSequenceWithDelimiter(std::ostream& output, InputIterator begin,
                                InputIterator end, const std::string& delimiter=" ") {
    bool first = true;
    for (InputIterator it = begin; it != end; ++it) {
        if (!first) {
            output << delimiter;
        } else {
            first = false;
        }
        output << *it;
    }
}


std::string MakeLower(std::string);
