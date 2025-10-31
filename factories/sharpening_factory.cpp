#include "sharpening_factory.h"

#include "../exceptions.h"
#include "../filters/sharpening_filter.h"

std::shared_ptr<BaseFilter> SharpeningFactory::Create(const std::vector<std::string>& params) {
    if (!params.empty()) {
        throw UsageException("sharpening filter has no parameters");
    }

    return std::make_shared<SharpeningFilter>();
}