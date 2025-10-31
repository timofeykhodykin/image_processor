#pragma once

#include "base_factory.h"
#include "../fft.h"

class FFTComponentFactory : public BaseFactory {
public:
    explicit FFTComponentFactory(FFTComponent component);

    std::shared_ptr<BaseFilter> Create(const std::vector<std::string>& params) override;

private:
    FFTComponent component_;
};

class FFTLowPassFactory : public BaseFactory {
public:
    std::shared_ptr<BaseFilter> Create(const std::vector<std::string>& params) override;
};

class FFTHighPassFactory : public BaseFactory {
public:
    std::shared_ptr<BaseFilter> Create(const std::vector<std::string>& params) override;
};

class FFTPeaksFactory : public BaseFactory {
public:
    std::shared_ptr<BaseFilter> Create(const std::vector<std::string>& params) override;
};