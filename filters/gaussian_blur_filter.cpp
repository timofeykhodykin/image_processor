#include "gaussian_blur_filter.h"

#include <cmath>
#include <cstdint>

GaussianBlurFilter::GaussianBlurFilter(double sigma) {
    if (sigma < 0) {
        sigma *= -1;
    }
    sigma_ = sigma;

    max_distance_ = std::ceil(3 * sigma_);
    coefficients_.assign(max_distance_, 0);
    for (size_t x = 0; x < max_distance_; ++x) {
        coefficients_[x] = std::exp(-static_cast<double>(x) * static_cast<double>(x) / (2 * sigma * sigma));
    }
}

void GaussianBlurFilter::Apply(Image& image) const {
    if (sigma_ == 0) {
        return;
    }

    std::vector pixels1(image.GetHeight(), std::vector<Color>(image.GetWidth()));
    for (int32_t i = 0; i < image.GetHeight(); ++i) {
        for (int32_t j = 0; j < image.GetWidth(); ++j) {
            for (int32_t k = 0; k < max_distance_; ++k) {
                pixels1[i][j] = pixels1[i][j] + image.GetPixel(std::max(0, i - k), j) * coefficients_[k];
                if (k > 0) {
                    pixels1[i][j] =
                        pixels1[i][j] + image.GetPixel(std::min(static_cast<int32_t>(image.GetWidth()) - 1, i + k), j) *
                                            coefficients_[k];
                }
            }
        }
    }

    std::vector pixels2(image.GetHeight(), std::vector<Color>(image.GetWidth()));
    for (int32_t i = 0; i < image.GetHeight(); ++i) {
        for (int32_t j = 0; j < image.GetWidth(); ++j) {
            for (int32_t k = 0; k < max_distance_; ++k) {
                pixels2[i][j] = pixels2[i][j] + pixels1[i][std::max(0, j - k)] * coefficients_[k];
                if (k > 0) {
                    pixels2[i][j] =
                        pixels2[i][j] +
                        pixels1[i][std::min(static_cast<int32_t>(image.GetWidth()) - 1, j + k)] * coefficients_[k];
                }
            }
            pixels2[i][j] = pixels2[i][j] * (1.0 / (2 * M_PI * sigma_ * sigma_));
        }
    }

    image.SetPixels(std::move(pixels2));
}
