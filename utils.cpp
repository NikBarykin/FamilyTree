#include "utils.h"

#include <stdexcept>
#include <random>
#include <chrono>
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
