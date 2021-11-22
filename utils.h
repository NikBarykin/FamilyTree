#pragma once

#include "C:/Dev/Cpp/Libs/svg/svg.h"

#include <string>
#include <string_view>
#include <vector>
#include <chrono>

std::vector<std::string> Split(std::string_view sv, const std::string& delimiter=" ");

double Rand01(unsigned seed = time(nullptr) + 239);

Svg::Color RandColor(unsigned seed = time(nullptr) + 239);


Svg::Rgb MergeRgb(Svg::Rgb lhs, Svg::Rgb rhs);

template<typename SvgRgbIt>
Svg::Rgb MergeRgb(SvgRgbIt begin, SvgRgbIt end) {
    int red_sum = 0, green_sum = 0, blue_sum = 0, n_colors = 0;
    for (SvgRgbIt it = begin; it != end; ++it) {
        red_sum += it->red;
        green_sum += it->green;
        blue_sum += it->blue;
        ++n_colors;
    }
    return Svg::Rgb{red_sum / n_colors,
                    green_sum / n_colors,
                    blue_sum / n_colors};
}
