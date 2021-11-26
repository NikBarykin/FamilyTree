#pragma once

#include "svg.h"

#include <string>
#include <string_view>
#include <vector>
#include <chrono>
#include <unordered_set>


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
//
//
//Svg::Rgb MergeRgb(Svg::Rgb lhs, Svg::Rgb rhs);
//
//
//template<typename SvgRgbIt>
//Svg::Rgb MergeRgb(SvgRgbIt begin, SvgRgbIt end) {
//    int red_sum = 0, green_sum = 0, blue_sum = 0, n_colors = 0;
//    for (SvgRgbIt it = begin; it != end; ++it) {
//        red_sum += it->red;
//        green_sum += it->green;
//        blue_sum += it->blue;
//        ++n_colors;
//    }
//    return Svg::Rgb{red_sum / n_colors,
//                    green_sum / n_colors,
//                    blue_sum / n_colors};
//}
