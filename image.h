#pragma once

#include <vector>

struct Color {
    Color() = default;
    Color(double r, double g, double b);

    double r = 0;
    double g = 0;
    double b = 0;
};

bool operator==(const Color& a, const Color& b);

Color operator+(const Color& a, const Color& b);

Color operator*(const Color& a, double x);

double NormalizeColorValue(double x);

class Image {
public:
    Image();
    explicit Image(const std::vector<std::vector<Color>>& pixels);
    explicit Image(std::vector<std::vector<Color>>&& pixels);

    size_t GetHeight() const;
    size_t GetWidth() const;

    Color GetPixel(size_t i, size_t j) const;
    std::vector<std::vector<Color>> GetPixels() const;

    void SetPixels(const std::vector<std::vector<Color>>& pixels);
    void SetPixels(std::vector<std::vector<Color>>&& pixels);

private:
    std::vector<std::vector<Color>> pixels_;
    size_t height_ = 0, width_ = 0;
};