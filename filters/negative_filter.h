#pragma once

#include "base_filter.h"

class NegativeFilter : public BaseFilter {
public:
    NegativeFilter();

    void Apply(Image& image) const override;
};