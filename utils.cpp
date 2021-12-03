#include "utils.h"

#include <stdexcept>
#include <random>
#include <chrono>
#include <string>
#include <algorithm>

using namespace std;


std::vector<std::string> Split(std::string_view sv, const std::string& delimiter) {
    if (delimiter.empty()) {
        throw invalid_argument("Delimiter shouldn't be empty!");
    }
    vector<string> result;
    while (!sv.empty()) {
        size_t del_pos = sv.find(delimiter);
        result.emplace_back(sv.substr(0, del_pos));
        sv.remove_prefix(min(sv.size(), del_pos));
        while (sv.substr(0, delimiter.size()) == delimiter) {
            sv.remove_prefix(delimiter.size());
        }
    }
    return result;
}


std::string ReadEverythingFromFile(const std::string& filename) {
    std::ifstream f_input(filename);
    std::string result;
    for (std::string line; getline(f_input, line); ) {
        result.insert(result.end(), make_move_iterator(line.begin()),
                                     make_move_iterator(line.end()));
        result.push_back('\n');
    }
    return result;
}


std::string MakeLower(std::string str) {
    transform(begin(str), end(str), begin(str),
              [](char ch) { return tolower(ch); });
    return str;
}
