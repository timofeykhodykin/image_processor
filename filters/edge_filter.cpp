#include "edge_filter.h"

EdgeFilter::EdgeFilter(const double threshold)
    : threshold_(threshold), matrix_filter_({{-1.0}, {-1.0, 4.0, -1.0}, {-1.0}}) {
}

void EdgeFilter::Apply(Image& image) const {
    grayscale_filter_.Apply(image);
    matrix_filter_.Apply(image);
    std::vector pixels(image.GetHeight(), std::vector<Color>(image.GetWidth()));
    for (size_t i = 0; i < image.GetHeight(); ++i) {
        for (size_t j = 0; j < image.GetWidth(); ++j) {
            if (image.GetPixel(i, j).r >= threshold_) {
                pixels[i][j] = Color(1.0, 1.0, 1.0);
            }
        }
    }
    image.SetPixels(std::move(pixels));
}
