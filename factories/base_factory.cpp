#include "base_factory.h"

#include "../exceptions.h"

BaseFactory::~BaseFactory() {
}

size_t ConvertToSizeT(const std::string& s) {
    size_t pos = 0;
    size_t result = 0;
    try {
        result = static_cast<size_t>(std::stoull(s, &pos));
    } catch (std::exception) {
        throw InternalException("cannot convert \"" + s + "\" to non-negative integer");
    }
    if (pos != s.size()) {
        throw InternalException("cannot convert \"" + s + "\" to non-negative integer");
    }
    return result;
}

double ConvertToDouble(const std::string& s) {
    size_t pos = 0;
    double result = 0;
    try {
        result = std::stod(s, &pos);
    } catch (std::exception) {
        throw InternalException("cannot convert \"" + s + "\" to number");
    }
    if (pos != s.size()) {
        throw InternalException("cannot convert \"" + s + "\" to number");
    }
    return result;
}
