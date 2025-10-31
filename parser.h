#pragma once

#include <string>
#include <vector>

struct FilterInput {
    std::string name;
    std::vector<std::string> params;
};

bool operator==(const FilterInput& a, const FilterInput& b);

struct ParserResult {
    std::string input_path;
    std::string output_path;
    std::vector<FilterInput> filters;
};

bool operator==(const ParserResult& a, const ParserResult& b);

ParserResult Parse(int argc, char** argv);