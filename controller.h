#pragma once

#include "factories/base_factory.h"
#include "factories/crop_factory.h"
#include "factories/edge_factory.h"
#include "factories/fft_factories.h"
#include "factories/gaussian_blur_factory.h"
#include "factories/grayscale_factory.h"
#include "factories/negative_factory.h"
#include "factories/sharpening_factory.h"
#include "fft.h"
#include "filters/base_filter.h"
#include "parser.h"

#include <future>
#include <string>
#include <unordered_map>
#include <vector>

const std::unordered_map<std::string, std::shared_ptr<BaseFactory>> FACTORIES = {
    {"crop", std::make_shared<CropFactory>()},
    {"edge", std::make_shared<EdgeFactory>()},
    {"blur", std::make_shared<GaussianBlurFactory>()},
    {"gs", std::make_shared<GrayscaleFactory>()},
    {"neg", std::make_shared<NegativeFactory>()},
    {"sharp", std::make_shared<SharpeningFactory>()},
    {"fft-real", std::make_shared<FFTComponentFactory>(REAL_PART)},
    {"fft-imag", std::make_shared<FFTComponentFactory>(IMAGINARY_PART)},
    {"fft-magnitude", std::make_shared<FFTComponentFactory>(MAGNITUDE)},
    {"fft-phase", std::make_shared<FFTComponentFactory>(PHASE)},
    {"fft-lowpass", std::make_shared<FFTLowPassFactory>()},
    {"fft-highpass", std::make_shared<FFTHighPassFactory>()},
    {"fft-peaks", std::make_shared<FFTPeaksFactory>()}};

std::vector<std::shared_ptr<BaseFilter>> CreateFilters(const std::vector<FilterInput>& filters_input);

void ApplyFilters(Image& image, const std::vector<std::shared_ptr<BaseFilter>>& filters);