#include "matrix_filter.h"

#include "../exceptions.h"

#include <iostream>

#include <cstdint>

MatrixFilter::MatrixFilter(const std::vector<std::vector<double>>& matrix) {
    if (matrix.size() % 2 == 0) {
        throw InternalException("convolution matrix must have odd number of rows");
    }
    for (size_t i = 0; i < matrix.size(); ++i) {
        if (matrix[i].size() % 2 == 0) {
            throw InternalException("convolution matrix must have odd number of columns");
        }
    }
    matrix_ = matrix;
}

MatrixFilter::MatrixFilter(std::vector<std::vector<double>>&& matrix) {
    if (matrix.size() % 2 == 0) {
        throw InternalException("convolution matrix must have odd number of rows");
    }
    for (size_t i = 0; i < matrix.size(); ++i) {
        if (matrix[i].size() % 2 == 0) {
            throw InternalException("convolution matrix must have odd number of columns");
        }
    }
    matrix_ = std::move(matrix);
}

std::pair<int64_t, int64_t> GetNearestPixel(int64_t i, int64_t j, int64_t matrix_i, int64_t matrix_j, int64_t h,
                                            int64_t w) {
    return {std::max(0ll, std::min(h - 1, i + matrix_i)), std::max(0ll, std::min(w - 1, j + matrix_j))};
}

void MatrixFilter::Apply(Image& image) const {
    const int64_t h = static_cast<int64_t>(image.GetHeight());
    const int64_t w = static_cast<int64_t>(image.GetWidth());
    std::vector pixels(h, std::vector<Color>(w));
    for (int64_t i = 0; i < h; ++i) {
        for (int64_t j = 0; j < w; ++j) {
            for (int64_t mi = 0; mi < matrix_.size(); ++mi) {
                for (int64_t mj = 0; mj < matrix_[mi].size(); ++mj) {
                    auto [conv_i, conv_j] = GetNearestPixel(i, j, mi - static_cast<int64_t>(matrix_.size() / 2),
                                                            mj - static_cast<int64_t>(matrix_[mi].size() / 2), h, w);
                    pixels[i][j] = pixels[i][j] + image.GetPixel(conv_i, conv_j) * matrix_[mi][mj];
                }
            }
        }
    }

    image.SetPixels(std::move(pixels));
}