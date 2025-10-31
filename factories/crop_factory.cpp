#include "crop_factory.h"

#include "../exceptions.h"
#include "../filters/crop_filter.h"

std::shared_ptr<BaseFilter> CropFactory::Create(const std::vector<std::string>& params) {
    if (params.size() != 2) {
        throw UsageException("crop filter has exactly 2 parameters");
    }

    size_t height = 0;
    size_t width = 0;
    try {
        height = ConvertToSizeT(params[0]);
        width = ConvertToSizeT(params[1]);
    } catch (InternalException) {
        throw UsageException("could not parse crop filter parameters into non-negative integers");
    }
    if (static_cast<double>(width) > static_cast<double>(SIZE_MAX) / static_cast<double>(height)) {
        throw UsageException("image after crop filter is too large");
    }

    return std::make_shared<CropFilter>(height, width);
}