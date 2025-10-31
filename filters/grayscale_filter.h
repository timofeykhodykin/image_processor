#pragma once

#include "base_filter.h"

class GrayscaleFilter : public BaseFilter {
public:
    GrayscaleFilter();

    void Apply(Image& image) const override;
};