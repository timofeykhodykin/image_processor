#pragma once

#include "base_filter.h"

class MatrixFilter : public BaseFilter {
public:
    MatrixFilter() = default;
    explicit MatrixFilter(const std::vector<std::vector<double>>& matrix);
    explicit MatrixFilter(std::vector<std::vector<double>>&& matrix);

    void Apply(Image& image) const override;

private:
    std::vector<std::vector<double>> matrix_ = {{1}};
};