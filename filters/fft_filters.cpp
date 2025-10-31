#include "fft_filters.h"

#include "crop_filter.h"

#include <algorithm>
#include <iostream>

FFTComponentFilter::FFTComponentFilter(FFTComponent type, double coefficient, bool verbose)
    : type_(type), coefficient_(coefficient), verbose_(verbose) {
}

void FFTComponentFilter::Apply(Image& image) const {
    auto pixels = ConvertToImage(FFT(image), type_, true).GetPixels();
    std::vector<double> values;
    for (size_t i = 0; i < pixels.size(); ++i) {
        for (size_t j = 0; j < pixels[i].size(); ++j) {
            if (verbose_) {
                values.push_back(pixels[i][j].r);
                values.push_back(pixels[i][j].g);
                values.push_back(pixels[i][j].b);
            }
            pixels[i][j] = pixels[i][j] * coefficient_;
        }
    }

    if (verbose_) {
        std::cout << "maximal values: ";
        std::sort(values.rbegin(), values.rend());
        constexpr size_t VerboseLength = 50;
        for (size_t i = 0; i < std::min(VerboseLength, values.size()); ++i) {
            std::cout << values[i] << " ";
        }
        if (values.size() > VerboseLength) {
            std::cout << "...";
        }
        std::cout << std::endl;
    }

    image.SetPixels(std::move(pixels));
}

size_t GetDistToOrigin(const size_t i, const size_t j, const size_t height, const size_t width) {
    return std::max(std::min(i, height - i - 1), std::min(j, width - j - 1));
}

FFTLowPassFilter::FFTLowPassFilter(const double threshold) : threshold_(threshold) {
}

void FFTLowPassFilter::Apply(Image& image) const {
    if (image.GetHeight() == 0 || image.GetWidth() == 0) {
        return;
    }

    auto fft = FFT(image).GetElements();
    const size_t height = fft[0].size();
    const size_t width = fft[0][0].size();
    const size_t new_height = static_cast<size_t>(std::round(static_cast<double>(height) * threshold_));
    const size_t new_width = static_cast<size_t>(std::round(static_cast<double>(width) * threshold_));

    for (size_t color = 0; color < 3; ++color) {
        for (size_t i = 0; i < height; ++i) {
            for (size_t j = 0; j < width; ++j) {
                if (std::min(i, height - i - 1) > new_height || std::min(j, width - j - 1) > new_width) {
                    fft[color][i][j] = 0;
                }
            }
        }
    }

    const CropFilter crop(image.GetHeight(), image.GetWidth());

    Image result = InverseFFT(ImageFrequencyDomainRepresentation(fft));
    crop.Apply(result);
    image.SetPixels(result.GetPixels());
}

FFTHighPassFilter::FFTHighPassFilter(const double threshold) : threshold_(threshold) {
}

void FFTHighPassFilter::Apply(Image& image) const {
    if (image.GetHeight() == 0 || image.GetWidth() == 0) {
        return;
    }

    auto fft = FFT(image).GetElements();
    const size_t height = fft[0].size();
    const size_t width = fft[0][0].size();
    const size_t new_height = static_cast<size_t>(std::round(static_cast<double>(height) * threshold_));
    const size_t new_width = static_cast<size_t>(std::round(static_cast<double>(width) * threshold_));

    for (size_t color = 0; color < 3; ++color) {
        for (size_t i = 0; i < height; ++i) {
            for (size_t j = 0; j < width; ++j) {
                if (std::min(i, height - i - 1) < new_height && std::min(j, width - j - 1) < new_width) {
                    fft[color][i][j] = 0;
                }
            }
        }
    }

    const CropFilter crop(image.GetHeight(), image.GetWidth());

    Image result = InverseFFT(ImageFrequencyDomainRepresentation(fft));
    crop.Apply(result);
    image.SetPixels(result.GetPixels());
}

FFTPeaksFilter::FFTPeaksFilter(const double threshold) : threshold_(threshold) {
}

FFTPeaksFilter::FFTPeaksFilter(const double threshold, const double safe_height, const double safe_width)
    : threshold_(threshold), safe_height_(safe_height), safe_width_(safe_width) {
}

void FFTPeaksFilter::Apply(Image& image) const {
    if (image.GetHeight() == 0 || image.GetWidth() == 0) {
        return;
    }

    auto fft = FFT(image).GetElements();
    const size_t height = fft[0].size();
    const size_t width = fft[0][0].size();
    const size_t safe_height = static_cast<size_t>(std::round(static_cast<double>(height) * safe_height_));
    const size_t safe_width = static_cast<size_t>(std::round(static_cast<double>(width) * safe_width_));

    for (size_t color = 0; color < 3; ++color) {
        for (size_t i = 0; i < fft[color].size(); ++i) {
            for (size_t j = 0; j < fft[color][i].size(); ++j) {
                if (std::min(i, height - i - 1) < safe_height && std::min(j, width - j - 1) < safe_width) {
                    continue;
                }
                if (std::abs(fft[color][i][j]) > threshold_) {
                    fft[color][i][j] = 0;
                }
            }
        }
    }

    const CropFilter crop(image.GetHeight(), image.GetWidth());

    Image result = InverseFFT(ImageFrequencyDomainRepresentation(fft));
    crop.Apply(result);
    image.SetPixels(result.GetPixels());
}