#pragma once

#include "base_factory.h"

class GrayscaleFactory : public BaseFactory {
public:
    std::shared_ptr<BaseFilter> Create(const std::vector<std::string>& params) override;
};