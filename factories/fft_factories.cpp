#include "fft_factories.h"

#include "../filters/fft_filters.h"

FFTComponentFactory::FFTComponentFactory(FFTComponent component) : component_(component) {
}

std::shared_ptr<BaseFilter> FFTComponentFactory::Create(const std::vector<std::string>& params) {
    if (params.size() > 2) {
        throw UsageException("fft " + COMPONENT_NAMES.find(component_)->second +
                             " filter can have 0, 1 or 2 parameters");
    }

    double coefficient = 1.0;
    if (!params.empty()) {
        try {
            coefficient = ConvertToDouble(params[0]);
        } catch (InternalException) {
            throw UsageException("could not parse fft " + COMPONENT_NAMES.find(component_)->second +
                                 " filter parameter into number");
        }
    }

    bool verbose = false;
    if (params.size() == 2) {
        if (params[1] == "0") {
            verbose = false;
        } else if (params[1] == "1") {
            verbose = true;
        } else {
            throw UsageException("fft " + COMPONENT_NAMES.find(component_)->second +
                                 " filter second parameter must be equal to 0 or 1");
        }
    }

    return std::make_shared<FFTComponentFilter>(component_, coefficient, verbose);
}

std::shared_ptr<BaseFilter> FFTLowPassFactory::Create(const std::vector<std::string>& params) {
    if (params.size() != 1) {
        throw UsageException("fft low pass filter has exactly 1 parameter");
    }

    double threshold = 1.0;
    try {
        threshold = ConvertToDouble(params[0]);
    } catch (InternalException) {
        throw UsageException("could not parse fft low pass filter parameter into number");
    }
    if (threshold < 0.0 || 1.0 < threshold) {
        throw UsageException("fft low pass filter parameter must be between 0 and 1");
    }

    return std::make_shared<FFTLowPassFilter>(threshold);
}

std::shared_ptr<BaseFilter> FFTHighPassFactory::Create(const std::vector<std::string>& params) {
    if (params.size() != 1) {
        throw UsageException("fft high pass filter has exactly 1 parameter");
    }

    double threshold = 1.0;
    try {
        threshold = ConvertToDouble(params[0]);
    } catch (InternalException) {
        throw UsageException("could not parse fft high pass filter parameter into number");
    }
    if (threshold < 0.0 || 1.0 < threshold) {
        throw UsageException("fft high pass filter parameter must be between 0 and 1");
    }

    return std::make_shared<FFTHighPassFilter>(threshold);
}

std::shared_ptr<BaseFilter> FFTPeaksFactory::Create(const std::vector<std::string>& params) {
    if (params.size() != 1 && params.size() != 3) {
        throw UsageException("fft peaks filter can have 1 or 3 parameters");
    }

    double threshold = 1.0;
    try {
        threshold = ConvertToDouble(params[0]);
    } catch (InternalException) {
        throw UsageException("could not parse fft peaks filter parameter into number");
    }
    if (threshold < 0.0 || 1.0 < threshold) {
        throw UsageException("fft peaks filter parameter must be between 0 and 1");
    }

    if (params.size() == 3) {
        double safe_height = 0.0;
        double safe_width = 0.0;
        try {
            safe_height = ConvertToDouble(params[1]);
            safe_width = ConvertToDouble(params[2]);
        } catch (InternalException) {
            throw UsageException("could not parse fft peaks filter parameters into numbers");
        }
        if (std::min(safe_height, safe_width) < 0.0 || 1.0 < std::max(safe_height, safe_width)) {
            throw UsageException("fft peaks filter parameters must be between 0 and 1");
        }
        return std::make_shared<FFTPeaksFilter>(threshold, safe_height, safe_width);
    }

    return std::make_shared<FFTPeaksFilter>(threshold);
}
