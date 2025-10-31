#include "gaussian_blur_factory.h"

#include "../exceptions.h"
#include "../filters/gaussian_blur_filter.h"

std::shared_ptr<BaseFilter> GaussianBlurFactory::Create(const std::vector<std::string>& params) {
    if (params.size() != 1) {
        throw UsageException("gaussian blur filter has exactly 1 parameter");
    }

    double sigma = 0.0;
    try {
        sigma = ConvertToDouble(params[0]);
    } catch (InternalException) {
        throw UsageException("could not parse gaussian blur filter parameter into number");
    }

    return std::make_shared<GaussianBlurFilter>(sigma);
}