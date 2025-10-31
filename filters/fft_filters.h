#pragma once

#include "../fft.h"
#include "base_filter.h"

class FFTComponentFilter : public BaseFilter {
public:
    explicit FFTComponentFilter(FFTComponent type, double coefficient, bool verbose);

    void Apply(Image& image) const override;

private:
    FFTComponent type_;
    double coefficient_ = 1.0;
    bool verbose_ = false;
};

class FFTLowPassFilter : public BaseFilter {
public:
    explicit FFTLowPassFilter(double threshold);

    void Apply(Image& image) const override;

private:
    double threshold_ = 1.0;
};

class FFTHighPassFilter : public BaseFilter {
public:
    explicit FFTHighPassFilter(double threshold);

    void Apply(Image& image) const override;

private:
    double threshold_ = 1.0;
};

class FFTPeaksFilter : public BaseFilter {
public:
    explicit FFTPeaksFilter(double threshold);
    FFTPeaksFilter(double threshold, double safe_height, double safe_width);

    void Apply(Image& image) const override;

private:
    double threshold_ = 1.0;
    double safe_height_ = 0.0;
    double safe_width_ = 0.0;
};