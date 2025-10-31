#pragma once

#include "base_filter.h"

class CropFilter : public BaseFilter {
public:
    CropFilter(size_t height, size_t width);

    void Apply(Image& image) const override;

private:
    size_t height_;
    size_t width_;
};