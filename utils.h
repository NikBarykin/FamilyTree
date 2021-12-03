#pragma once

#include "svg.h"

#include <iostream>
#include <fstream>
#include <string>
#include <string_view>
#include <vector>
#include <chrono>
#include <unordered_set>
#include <sstream>


std::vector<std::string> Split(std::string_view sv, const std::string& delimiter=" ");


template<typename T>
std::unordered_set<T> UnorderedSetIntersection(const std::unordered_set<T>& lhs,
                                               const std::unordered_set<T>& rhs) {
    if (rhs.size() < lhs.size()) {
        return UnorderedSetIntersection(rhs, lhs);
    }
    std::unordered_set<T> result;
    result.reserve(lhs.size());
    for (const T& el : lhs) {
        if (rhs.count(el)) {
            result.insert(el);
        }
    }
    return result;
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
