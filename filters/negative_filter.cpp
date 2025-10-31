#include "negative_filter.h"

NegativeFilter::NegativeFilter() {
}

void NegativeFilter::Apply(Image& image) const {
    std::vector pixels(image.GetHeight(), std::vector<Color>(image.GetWidth()));
    for (size_t i = 0; i < image.GetHeight(); ++i) {
        for (size_t j = 0; j < image.GetWidth(); ++j) {
            const Color old_color = image.GetPixel(i, j);
            pixels[i][j] = Color(1.0 - old_color.r, 1.0 - old_color.g, 1.0 - old_color.b);
        }
    }
    image.SetPixels(std::move(pixels));
}
