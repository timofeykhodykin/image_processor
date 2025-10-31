#pragma once

#include "exceptions.h"
#include "image.h"

#include <array>
#include <complex>
#include <unordered_map>
#include <vector>

class ImageFrequencyDomainRepresentation {
public:
    ImageFrequencyDomainRepresentation();

    explicit ImageFrequencyDomainRepresentation(
        const std::array<std::vector<std::vector<std::complex<double>>>, 3>& matrix);
    explicit ImageFrequencyDomainRepresentation(std::array<std::vector<std::vector<std::complex<double>>>, 3>&& matrix);

    size_t GetHeight() const;
    size_t GetWidth() const;

    std::array<std::complex<double>, 3> GetElement(size_t i, size_t j) const;
    std::array<std::vector<std::vector<std::complex<double>>>, 3> GetElements() const;

    void SetElements(const std::array<std::vector<std::vector<std::complex<double>>>, 3>& matrix);
    void SetElements(std::array<std::vector<std::vector<std::complex<double>>>, 3>&& matrix);

private:
    std::array<std::vector<std::vector<std::complex<double>>>, 3> matrix_;
    size_t height_ = 0, width_ = 0;
};

enum FFTComponent { REAL_PART, IMAGINARY_PART, MAGNITUDE, PHASE };

const std::unordered_map<FFTComponent, std::string> COMPONENT_NAMES = {
    {REAL_PART, "real part"}, {IMAGINARY_PART, "imaginary part"}, {MAGNITUDE, "magnitude"}, {PHASE, "phase"}};

ImageFrequencyDomainRepresentation ConvertToFrequencyDomainRepresentation(const Image& image);

Image ConvertToImage(const ImageFrequencyDomainRepresentation& fd, FFTComponent component, bool rearrange = false);

ImageFrequencyDomainRepresentation FFT(const Image& image);

Image InverseFFT(const ImageFrequencyDomainRepresentation& fd);