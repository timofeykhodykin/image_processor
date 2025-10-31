#include "fft.h"

#include <algorithm>
#include <iostream>

#include <cstdint>

ImageFrequencyDomainRepresentation::ImageFrequencyDomainRepresentation() {
}

ImageFrequencyDomainRepresentation::ImageFrequencyDomainRepresentation(
    const std::array<std::vector<std::vector<std::complex<double>>>, 3>& matrix) {
    SetElements(matrix);
}

ImageFrequencyDomainRepresentation::ImageFrequencyDomainRepresentation(
    std::array<std::vector<std::vector<std::complex<double>>>, 3>&& matrix) {
    SetElements(matrix);
}

size_t ImageFrequencyDomainRepresentation::GetHeight() const {
    return height_;
}

size_t ImageFrequencyDomainRepresentation::GetWidth() const {
    return width_;
}

std::array<std::complex<double>, 3> ImageFrequencyDomainRepresentation::GetElement(size_t i, size_t j) const {
    if (i > GetHeight() || j > GetWidth()) {
        throw InternalException("GetElement coordinates are out of bounds");
    }
    return {matrix_[0][i][j], matrix_[1][i][j], matrix_[2][i][j]};
}

std::array<std::vector<std::vector<std::complex<double>>>, 3> ImageFrequencyDomainRepresentation::GetElements() const {
    return matrix_;
}

void ImageFrequencyDomainRepresentation::SetElements(
    const std::array<std::vector<std::vector<std::complex<double>>>, 3>& matrix) {
    for (size_t color = 0; color < 3; ++color) {
        const size_t height = matrix[color].size();
        if (color > 0 && matrix[color - 1].size() != height) {
            throw InternalException(
                "trying to set image representation in frequency domain with different heights of matrices");
        }
        if (height != 0) {
            const size_t width = matrix[color][0].size();
            if (color > 0 && matrix[color - 1][0].size() != width) {
                throw InternalException(
                    "trying to set image representation in frequency domain with different lengths of rows of "
                    "matrices");
            }
            for (size_t i = 1; i < height; ++i) {
                if (matrix[color][i].size() != width) {
                    throw InternalException(
                        "trying to set image representation in frequency domain with different lengths of rows");
                }
            }
        }
    }

    matrix_ = matrix;
    height_ = matrix[0].size();
    if (height_ == 0) {
        width_ = 0;
    } else {
        width_ = matrix[0][0].size();
    }
}

void ImageFrequencyDomainRepresentation::SetElements(
    std::array<std::vector<std::vector<std::complex<double>>>, 3>&& matrix) {
    for (size_t color = 0; color < 3; ++color) {
        const size_t height = matrix[color].size();
        if (color > 0 && matrix[color - 1].size() != height) {
            throw InternalException(
                "trying to set image representation in frequency domain with different heights of matrices");
        }
        if (height != 0) {
            const size_t width = matrix[color][0].size();
            if (color > 0 && matrix[color - 1][0].size() != width) {
                throw InternalException(
                    "trying to set image representation in frequency domain with different lengths of rows of "
                    "matrices");
            }
            for (size_t i = 1; i < height; ++i) {
                if (matrix[color][i].size() != width) {
                    throw InternalException(
                        "trying to set image representation in frequency domain with different lengths of rows");
                }
            }
        }
    }

    matrix_ = std::move(matrix);
    height_ = matrix_[0].size();
    if (height_ == 0) {
        width_ = 0;
    } else {
        width_ = matrix_[0][0].size();
    }
}

ImageFrequencyDomainRepresentation ConvertToFrequencyDomainRepresentation(const Image& image) {
    std::array<std::vector<std::vector<std::complex<double>>>, 3> result;
    std::fill(result.begin(), result.end(),
              std::vector(image.GetHeight(), std::vector<std::complex<double>>(image.GetWidth())));

    for (size_t i = 0; i < image.GetHeight(); ++i) {
        for (size_t j = 0; j < image.GetWidth(); ++j) {
            result[0][i][j] = image.GetPixel(i, j).r;
            result[1][i][j] = image.GetPixel(i, j).g;
            result[2][i][j] = image.GetPixel(i, j).b;
        }
    }

    return ImageFrequencyDomainRepresentation(std::move(result));
}

Image ConvertToImage(const ImageFrequencyDomainRepresentation& fd, FFTComponent component, bool rearrange) {
    size_t height = fd.GetHeight();
    size_t width = fd.GetWidth();
    if (height == 0 || width == 0) {
        return Image();
    }

    std::vector result(height, std::vector<Color>(width));
    for (size_t i = 0; i < height; ++i) {
        for (size_t j = 0; j < width; ++j) {
            std::array<std::complex<double>, 3> element;
            if (rearrange) {
                element = fd.GetElement((i + height / 2) % height, (j + width / 2) % width);
            } else {
                element = fd.GetElement(i, j);
            }

            if (component == REAL_PART) {
                result[i][j].r = std::abs(element[0].real());
                result[i][j].g = std::abs(element[1].real());
                result[i][j].b = std::abs(element[2].real());
            } else if (component == IMAGINARY_PART) {
                result[i][j].r = std::abs(element[0].imag());
                result[i][j].g = std::abs(element[1].imag());
                result[i][j].b = std::abs(element[2].imag());
            } else if (component == MAGNITUDE) {
                result[i][j].r = std::abs(element[0]);
                result[i][j].g = std::abs(element[1]);
                result[i][j].b = std::abs(element[2]);
            } else if (component == PHASE) {
                result[i][j].r = std::arg(element[0]) / (2 * M_PI) + 1.0 / 2;
                result[i][j].g = std::arg(element[1]) / (2 * M_PI) + 1.0 / 2;
                result[i][j].b = std::arg(element[2]) / (2 * M_PI) + 1.0 / 2;
            } else {
                throw InternalException("unknown component given to ConvertToImage");
            }
        }
    }

    return Image(std::move(result));
}

size_t RoundUpToPowerOfTwo(size_t x) {
    if (x == 0) {
        throw InternalException("trying to round 0 up to the power of 2");
    }
    constexpr size_t ByteSize = 8;
    return static_cast<size_t>(1) << (sizeof(size_t) * ByteSize - __builtin_clzll(x - 1));
}

size_t FFTReverseIndex(size_t index, size_t n) {
    size_t result = 0;
    for (size_t i = 0; i < n; ++i) {
        if ((index >> i) & 1) {
            result |= (1 << (n - i - 1));
        }
    }
    return result;
}

void FFT(std::vector<std::complex<double>>& a, bool inverse) {
    size_t n = a.size();
    if (RoundUpToPowerOfTwo(n) != n) {
        throw InternalException("FFT argument must has length equal to power of 2");
    }
    constexpr size_t ByteSize = 8;
    size_t lg_n = sizeof(size_t) * ByteSize - __builtin_clzll(n) - 1;

    for (size_t i = 0; i < n; ++i) {
        size_t pair = FFTReverseIndex(i, lg_n);
        if (i < pair) {
            std::swap(a[i], a[pair]);
        }
    }

    for (size_t len = 2; len <= n; len *= 2) {
        std::complex<double> root1_len = std::polar(1.0, 2 * M_PI / static_cast<double>(len) * (inverse ? 1 : -1));
        for (size_t i = 0; i < n; i += len) {
            std::complex<double> pwr(1, 0);
            for (size_t j = i; j < i + len / 2; ++j) {
                std::complex<double> x = a[j];
                std::complex<double> y = pwr * a[j + len / 2];
                a[j] = x + y;
                a[j + len / 2] = x - y;
                pwr *= root1_len;
            }
        }
    }

    if (!inverse) {
        for (size_t i = 0; i < n; ++i) {
            a[i] /= static_cast<double>(n);
        }
    }
}

ImageFrequencyDomainRepresentation FFT(const Image& image) {
    size_t height = image.GetHeight();
    size_t width = image.GetWidth();
    if (height == 0 || width == 0) {
        return ImageFrequencyDomainRepresentation();
    }

    height = RoundUpToPowerOfTwo(height);
    width = RoundUpToPowerOfTwo(width);
    std::array<std::vector<std::vector<std::complex<double>>>, 3> result =
        ConvertToFrequencyDomainRepresentation(image).GetElements();
    for (size_t color = 0; color < 3; ++color) {
        result[color].resize(height);
        for (size_t i = 0; i < height; ++i) {
            result[color][i].resize(width);
        }
    }

    for (size_t color = 0; color < 3; ++color) {
        for (size_t i = 0; i < height; ++i) {
            FFT(result[color][i], false);
        }
        for (size_t j = 0; j < width; ++j) {
            std::vector<std::complex<double>> values(height);
            for (size_t i = 0; i < height; ++i) {
                values[i] = result[color][i][j];
            }
            FFT(values, false);
            for (size_t i = 0; i < height; ++i) {
                result[color][i][j] = values[i];
            }
        }
    }

    return ImageFrequencyDomainRepresentation(std::move(result));
}

Image InverseFFT(const ImageFrequencyDomainRepresentation& fd) {
    size_t height = fd.GetHeight();
    size_t width = fd.GetWidth();
    if (height == 0 || width == 0) {
        return Image();
    }

    height = RoundUpToPowerOfTwo(height);
    width = RoundUpToPowerOfTwo(width);

    std::array<std::vector<std::vector<std::complex<double>>>, 3> result = fd.GetElements();
    for (size_t color = 0; color < 3; ++color) {
        result[color].resize(height);
        for (size_t i = 0; i < height; ++i) {
            result[color][i].resize(width);
        }
    }

    for (size_t color = 0; color < 3; ++color) {
        for (size_t i = 0; i < height; ++i) {
            FFT(result[color][i], true);
        }
        for (size_t j = 0; j < width; ++j) {
            std::vector<std::complex<double>> values(height);
            for (size_t i = 0; i < height; ++i) {
                values[i] = result[color][i][j];
            }
            FFT(values, true);
            for (size_t i = 0; i < height; ++i) {
                result[color][i][j] = values[i];
            }
        }
    }

    return ConvertToImage(ImageFrequencyDomainRepresentation(std::move(result)), REAL_PART);
}