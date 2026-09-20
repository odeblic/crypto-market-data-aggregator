#pragma once

#include <string>
#include <vector>

struct Captions
{
    std::string title;
    std::string symbol;
    std::vector<std::string> askLabels;
    std::vector<std::string> bidLabels;
    size_t maxLabelSize{0};
};
