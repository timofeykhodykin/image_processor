#pragma once

#include "base_filter.h"
#include "grayscale_filter.h"
#include "matrix_filter.h"

class EdgeFilter : public BaseFilter {
public:
    explicit EdgeFilter(double threshold);

    void Apply(Image& image) const override;

private:
    double threshold_;
    GrayscaleFilter grayscale_filter_;
    MatrixFilter matrix_filter_;
};