#pragma once

#include "base_filter.h"

class GaussianBlurFilter : public BaseFilter {
public:
    explicit GaussianBlurFilter(double sigma);

    void Apply(Image& image) const override;

private:
    double sigma_;
    size_t max_distance_;
    std::vector<double> coefficients_;
};