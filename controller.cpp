#include "controller.h"

#include "exceptions.h"

std::vector<std::shared_ptr<BaseFilter>> CreateFilters(const std::vector<FilterInput>& filters_input) {
    std::vector<std::shared_ptr<BaseFilter>> filters;
    for (const FilterInput& filter : filters_input) {
        auto fabric = FACTORIES.find(filter.name);
        if (fabric == FACTORIES.end()) {
            throw UsageException("unknown argument " + filter.name);
        }
        filters.push_back(fabric->second->Create(filter.params));
    }
    return filters;
}

void ApplyFilters(Image& image, const std::vector<std::shared_ptr<BaseFilter>>& filters) {
    for (const auto& filter : filters) {
        filter->Apply(image);
    }
}
