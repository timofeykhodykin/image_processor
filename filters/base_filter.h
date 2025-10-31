#pragma once

#include "../image.h"

class BaseFilter {
public:
    virtual void Apply(Image& image) const = 0;

    virtual ~BaseFilter();
};