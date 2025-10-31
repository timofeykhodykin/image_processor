#include "grayscale_filter.h"

GrayscaleFilter::GrayscaleFilter() {
}

void GrayscaleFilter::Apply(Image& image) const {
    std::vector pixels(image.GetHeight(), std::vector<Color>(image.GetWidth()));
    for (size_t i = 0; i < image.GetHeight(); ++i) {
        for (size_t j = 0; j < image.GetWidth(); ++j) {
            const Color old_color = image.GetPixel(i, j);
            const double new_color_value = old_color.r * 0.299 + old_color.g * 0.587 + old_color.b * 0.114;
            pixels[i][j] = Color(new_color_value, new_color_value, new_color_value);
        }
    }
    image.SetPixels(std::move(pixels));
}
