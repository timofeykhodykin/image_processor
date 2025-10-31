#pragma once

#include "base_factory.h"

class NegativeFactory : public BaseFactory {
public:
    std::shared_ptr<BaseFilter> Create(const std::vector<std::string>& params) override;
};