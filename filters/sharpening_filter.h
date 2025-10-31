#pragma once

#include "base_filter.h"
#include "matrix_filter.h"

class SharpeningFilter : public BaseFilter {
public:
    SharpeningFilter();

    void Apply(Image& image) const override;

private:
    MatrixFilter matrix_filter_;
};