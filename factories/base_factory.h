#pragma once

#include "../filters/base_filter.h"

#include <future>
#include <vector>

class BaseFactory {
public:
    virtual std::shared_ptr<BaseFilter> Create(const std::vector<std::string>& params) = 0;

    virtual ~BaseFactory();
};

size_t ConvertToSizeT(const std::string& s);

double ConvertToDouble(const std::string& s);