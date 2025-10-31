#include "negative_factory.h"

#include "../exceptions.h"
#include "../filters/negative_filter.h"

std::shared_ptr<BaseFilter> NegativeFactory::Create(const std::vector<std::string>& params) {
    if (!params.empty()) {
        throw UsageException("negative has no parameters");
    }

    return std::make_shared<NegativeFilter>();
}