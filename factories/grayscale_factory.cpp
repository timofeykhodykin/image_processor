#include "grayscale_factory.h"

#include "../exceptions.h"
#include "../filters/grayscale_filter.h"

std::shared_ptr<BaseFilter> GrayscaleFactory::Create(const std::vector<std::string>& params) {
    if (!params.empty()) {
        throw UsageException("grayscale filter has no parameters");
    }

    return std::make_shared<GrayscaleFilter>();
}