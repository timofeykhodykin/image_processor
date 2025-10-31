#include "crop_filter.h"

#include <vector>

CropFilter::CropFilter(size_t height, size_t width) : height_(height), width_(width) {
}

void CropFilter::Apply(Image& image) const {
    const size_t new_height = std::min(height_, image.GetHeight());
    const size_t new_width = std::min(width_, image.GetWidth());
    std::vector pixels(new_height, std::vector<Color>(new_width));
    for (size_t i = 0; i < new_height; ++i) {
        for (size_t j = 0; j < new_width; ++j) {
            pixels[i][j] = image.GetPixel(i, j);
        }
    }
    image.SetPixels(std::move(pixels));
}
