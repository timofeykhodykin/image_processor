#include "sharpening_filter.h"

SharpeningFilter::SharpeningFilter() : matrix_filter_({{-1.0}, {-1.0, 5.0, -1.0}, {-1.0}}) {
}

void SharpeningFilter::Apply(Image& image) const {
    matrix_filter_.Apply(image);
}