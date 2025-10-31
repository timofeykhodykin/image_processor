#include "image.h"

#include "exceptions.h"

Color::Color(double r, double g, double b) : r(r), g(g), b(b) {
}

bool operator==(const Color& a, const Color& b) {
    return a.r == b.r && a.g == b.g && a.b == b.b;
}

Color operator+(const Color& a, const Color& b) {
    return {a.r + b.r, a.g + b.g, a.b + b.b};
}

Color operator*(const Color& a, double x) {
    return {a.r * x, a.g * x, a.b * x};
}

Image::Image() {
}

Image::Image(const std::vector<std::vector<Color>>& pixels) {
    SetPixels(pixels);
}

Image::Image(std::vector<std::vector<Color>>&& pixels) {
    SetPixels(std::move(pixels));
}

size_t Image::GetHeight() const {
    return height_;
}

size_t Image::GetWidth() const {
    return width_;
}

Color Image::GetPixel(const size_t i, const size_t j) const {
    if (this->height_ <= i || this->width_ <= j) {
        throw InternalException("GetPixel coordinates are out of bounds");
    }
    return pixels_[i][j];
}

std::vector<std::vector<Color>> Image::GetPixels() const {
    return pixels_;
}

double NormalizeColorValue(const double x) {
    return std::max(0.0, std::min(1.0, x));
}

void Image::SetPixels(const std::vector<std::vector<Color>>& pixels) {
    if (!pixels.empty()) {
        const size_t width = pixels[0].size();
        for (size_t i = 0; i < pixels.size(); ++i) {
            if (pixels[i].size() != width) {
                throw InternalException("trying to set image with different lengths of rows");
            }
        }
    }

    pixels_ = pixels;
    for (size_t i = 0; i < pixels_.size(); ++i) {
        for (size_t j = 0; j < pixels_[i].size(); ++j) {
            pixels_[i][j].r = NormalizeColorValue(pixels_[i][j].r);
            pixels_[i][j].g = NormalizeColorValue(pixels_[i][j].g);
            pixels_[i][j].b = NormalizeColorValue(pixels_[i][j].b);
        }
    }

    height_ = pixels_.size();
    if (height_ == 0) {
        width_ = 0;
    } else {
        width_ = pixels[0].size();
    }
}

void Image::SetPixels(std::vector<std::vector<Color>>&& pixels) {
    if (!pixels.empty()) {
        const size_t width = pixels[0].size();
        for (size_t i = 0; i < pixels.size(); ++i) {
            if (pixels[i].size() != width) {
                throw InternalException("trying to set image with different lengths of rows");
            }
        }
    }

    pixels_ = std::move(pixels);
    for (size_t i = 0; i < pixels_.size(); ++i) {
        for (size_t j = 0; j < pixels_[i].size(); ++j) {
            pixels_[i][j].r = NormalizeColorValue(pixels_[i][j].r);
            pixels_[i][j].g = NormalizeColorValue(pixels_[i][j].g);
            pixels_[i][j].b = NormalizeColorValue(pixels_[i][j].b);
        }
    }

    height_ = pixels_.size();
    if (height_ == 0) {
        width_ = 0;
    } else {
        width_ = pixels_[0].size();
    }
}
