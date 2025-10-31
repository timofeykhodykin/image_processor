#include "edge_factory.h"

#include "../exceptions.h"
#include "../filters/edge_filter.h"

std::shared_ptr<BaseFilter> EdgeFactory::Create(const std::vector<std::string>& params) {
    if (params.size() != 1) {
        throw UsageException("edge filter has exactly 1 parameter");
    }

    double threshold = 0.0;
    try {
        threshold = ConvertToDouble(params[0]);
    } catch (InternalException) {
        throw UsageException("could not parse edge filter parameter into number");
    }

    return std::make_shared<EdgeFilter>(threshold);
}