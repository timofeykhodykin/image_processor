#include "parser.h"

#include "exceptions.h"

bool operator==(const FilterInput& a, const FilterInput& b) {
    return a.name == b.name && a.params == b.params;
}

bool operator==(const ParserResult& a, const ParserResult& b) {
    return a.input_path == b.input_path && a.output_path == b.output_path && a.filters == b.filters;
}

ParserResult Parse(int argc, char** argv) {
    if (argc < 3) {
        throw UsageException("you should specify input path and output path");
    }
    ParserResult result;
    result.input_path = argv[1];
    result.output_path = argv[2];

    for (size_t i = 3; i < argc; i++) {
        std::string arg = argv[i];
        if (arg[0] == '-') {
            result.filters.emplace_back();
            result.filters.back().name = arg.substr(1, arg.size() - 1);
        } else {
            if (result.filters.empty()) {
                throw UsageException("excess of unnamed arguments");
            }
            result.filters.back().params.push_back(arg);
        }
    }
    return result;
}