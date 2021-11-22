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

double Rand01(unsigned seed) {
    mt19937 rnd(seed);
    return static_cast<double>(rnd()) / mt19937::max();
}

Svg::Color RandColor(unsigned seed) {
    mt19937 rnd(seed);
    Svg::Rgb result;
    result.red = rnd() % 256;
    result.green = rnd() % 256;
    result.blue = rnd() % 256;
    return result;
}

Svg::Rgb MergeRgb(Svg::Rgb lhs, Svg::Rgb rhs) {
    return Svg::Rgb{
            (lhs.red + rhs.red) / 2,
            (lhs.green + rhs.green) / 2,
            (lhs.blue + rhs.blue) / 2,
    };
}
