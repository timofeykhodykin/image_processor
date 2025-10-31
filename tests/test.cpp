#define CATCH_CONFIG_MAIN
#include <catch2/catch_test_macros.hpp>
#include "catch2/matchers/catch_matchers.hpp"
#include "catch2/matchers/catch_matchers_exception.hpp"

#include "../controller.h"
#include "../exceptions.h"
#include "../factories/crop_factory.h"
#include "../factories/edge_factory.h"
#include "../parser.h"

TEST_CASE("Parser: positional arguments") {
    SECTION("No arguments given") {
        char* argv[] = {(char*)"image_processor"};
        REQUIRE_THROWS_AS(Parse(1, argv), UsageException);
    }

    SECTION("One argument given") {
        char* argv[] = {(char*)"image_processor", (char*)"a.bmp"};
        REQUIRE_THROWS_AS(Parse(2, argv), UsageException);
    }

    SECTION("More than two positional arguments") {
        char* argv[] = {
            (char*)"image_processor", (char*)"a.bmp", (char*)"b.bmp", (char*)"crop", (char*)"3", (char*)"5"};
        REQUIRE_THROWS_AS(Parse(6, argv), UsageException);
    }
}

TEST_CASE("Parser") {
    char* argv[] = {(char*)"image_processor",
                    (char*)"a.bmp",
                    (char*)"b.bmp",
                    (char*)"-crop",
                    (char*)"1",
                    (char*)"abc",
                    (char*)"-0.3",
                    (char*)"-edge",
                    (char*)"-blur",
                    (char*)"*&?",
                    (char*)"-"};
    REQUIRE(Parse(11, argv) ==
            ParserResult("a.bmp", "b.bmp",
                         {FilterInput("crop", {"1", "abc"}), FilterInput("0.3", {}), FilterInput("edge", {}),
                          FilterInput("blur", {"*&?"}), FilterInput("", {})}));
}

TEST_CASE("Controller: creating filters") {
    SECTION("crop 20 10 + gs + edge 0.3 + neg + blur 2 + sharp + fft-filters... + abcd") {
        REQUIRE_THROWS_MATCHES(
            CreateFilters({FilterInput("crop", {"20", "10"}), FilterInput("gs", {}), FilterInput("edge", {"0.3"}),
                           FilterInput("neg", {}), FilterInput("blur", {"2"}), FilterInput("sharp", {}),
                           FilterInput("fft-real", {}), FilterInput("fft-imag", {}), FilterInput("fft-magnitude", {}),
                           FilterInput("fft-phase", {}), FilterInput("fft-lowpass", {"1"}),
                           FilterInput("fft-highpass", {"1"}), FilterInput("fft-peaks", {"0.01"}),
                           FilterInput("abcd", {})}),
            UsageException, Catch::Matchers::Message("incorrect usage: unknown argument abcd"));
    }
}

TEST_CASE("Image") {
    SECTION("Correct pixels with normalization, out of bound indexes") {
        size_t height = 3, width = 2;
        std::vector<std::vector<Color>> pixels = {{{0, 0.5, 1.0}, {0.23, 1.2, -0.1}},
                                                  {{0.3, 0.17, 0.9}, {0.6896, 0.23, 0.75}},
                                                  {{1.0, 1.0, 1.0}, {0.0, 0.0, 0.0}}};
        std::vector<std::vector<Color>> norm_pixels = {{{0, 0.5, 1.0}, {0.23, 1.0, 0.0}},
                                                       {{0.3, 0.17, 0.9}, {0.6896, 0.23, 0.75}},
                                                       {{1.0, 1.0, 1.0}, {0.0, 0.0, 0.0}}};

        Image image(pixels);

        REQUIRE(image.GetHeight() == height);
        REQUIRE(image.GetWidth() == width);
        for (size_t i = 0; i < height; ++i) {
            for (size_t j = 0; j < width; ++j) {
                REQUIRE(image.GetPixel(i, j) == norm_pixels[i][j]);
            }
        }

        REQUIRE_THROWS_AS(image.GetPixel(3, 0), InternalException);
        REQUIRE_THROWS_AS(image.GetPixel(0, 2), InternalException);
        REQUIRE_THROWS_AS(image.GetPixel(-1, -1), InternalException);
    }

    SECTION("Different lengths of rows") {
        std::vector<std::vector<Color>> pixels = {
            {{0, 0.5, 1.0}, {0.23, 1.2, -0.1}}, {{0.3, 0.17, 0.9}, {0.6896, 0.23, 0.75}}, {{1.0, 1.0, 1.0}}};

        REQUIRE_THROWS_AS(Image(pixels), InternalException);
    }
}

TEST_CASE("Crop factory") {
    CropFactory factory;

    SECTION("No parameters given") {
        REQUIRE_THROWS_AS(factory.Create({}), UsageException);
    }

    SECTION("One parameters given") {
        REQUIRE_THROWS_AS(factory.Create({"1"}), UsageException);
    }

    SECTION("two parameters given") {
        REQUIRE_NOTHROW(factory.Create({"12", "0"}));
    }

    SECTION("Three parameters given") {
        REQUIRE_THROWS_AS(factory.Create({"100", "200", "10"}), UsageException);
    }

    SECTION("Given not non-negative integer parameters") {
        REQUIRE_THROWS_AS(factory.Create({"2", "0.1"}), UsageException);
        REQUIRE_THROWS_AS(factory.Create({"-2", "4"}), UsageException);
        REQUIRE_THROWS_AS(factory.Create({"100a", "10"}), UsageException);
        REQUIRE_THROWS_AS(factory.Create({"1000000000", "100000000000"}), UsageException);
        REQUIRE_THROWS_AS(factory.Create({"100000000000000000000000", "10000000000000000000000"}), UsageException);
    }
}

TEST_CASE("Edge factory") {
    EdgeFactory factory;
    SECTION("No parameters given") {
        REQUIRE_THROWS_AS(factory.Create({}), UsageException);
    }

    SECTION("One parameter given") {
        REQUIRE_NOTHROW(factory.Create({"12"}));
        REQUIRE_NOTHROW(factory.Create({"-2"}));
        REQUIRE_NOTHROW(factory.Create({"1.79"}));
    }

    SECTION("Two parameters given") {
        REQUIRE_THROWS_AS(factory.Create({"100", "200"}), UsageException);
    }

    SECTION("Given not a number") {
        REQUIRE_THROWS_AS(factory.Create({"a"}), UsageException);
        REQUIRE_THROWS_AS(factory.Create({"-2.01asdf"}), UsageException);
        REQUIRE_THROWS_AS(factory.Create({"10l"}), UsageException);
    }
}

TEST_CASE("FFT component factory") {
    for (FFTComponent component : {REAL_PART, IMAGINARY_PART, MAGNITUDE, PHASE}) {
        SECTION("FFT " + COMPONENT_NAMES.find(component)->second + " factory") {
            FFTComponentFactory factory(component);

            SECTION("No parameters given") {
                REQUIRE_NOTHROW(factory.Create({}));
            }

            SECTION("One parameter given") {
                REQUIRE_NOTHROW(factory.Create({"12"}));
                REQUIRE_NOTHROW(factory.Create({"-2"}));
                REQUIRE_NOTHROW(factory.Create({"1.79"}));
            }

            SECTION("Two parameters given") {
                REQUIRE_NOTHROW(factory.Create({"12", "0"}));
                REQUIRE_NOTHROW(factory.Create({"-1.2", "1"}));
            }

            SECTION("Given not a number") {
                REQUIRE_THROWS_AS(factory.Create({"a"}), UsageException);
                REQUIRE_THROWS_AS(factory.Create({"-2.01asdf"}), UsageException);
                REQUIRE_THROWS_AS(factory.Create({"10l"}), UsageException);
            }

            SECTION("Incorrect second parameter") {
                REQUIRE_THROWS_AS(factory.Create({"12", "2"}), UsageException);
                REQUIRE_THROWS_AS(factory.Create({"-1.2", "f"}), UsageException);
            }
        }
    }
}

TEST_CASE("FFT low pass factory") {
    FFTLowPassFactory factory;
    SECTION("No parameters given") {
        REQUIRE_THROWS_AS(factory.Create({}), UsageException);
    }

    SECTION("One parameter given") {
        REQUIRE_NOTHROW(factory.Create({"0.0"}));
        REQUIRE_NOTHROW(factory.Create({"-0.0"}));
        REQUIRE_NOTHROW(factory.Create({"0.3"}));
        REQUIRE_NOTHROW(factory.Create({"1.0"}));
    }

    SECTION("Two parameters given") {
        REQUIRE_THROWS_AS(factory.Create({"12", "0"}), UsageException);
    }

    SECTION("Given not a number") {
        REQUIRE_THROWS_AS(factory.Create({"a"}), UsageException);
        REQUIRE_THROWS_AS(factory.Create({"-2.01asdf"}), UsageException);
        REQUIRE_THROWS_AS(factory.Create({"10l"}), UsageException);
    }

    SECTION("Not between 0 and 1") {
        REQUIRE_THROWS_AS(factory.Create({"-0.1"}), UsageException);
        REQUIRE_THROWS_AS(factory.Create({"1.1"}), UsageException);
    }
}

TEST_CASE("FFT high pass factory") {
    FFTHighPassFactory factory;
    SECTION("No parameters given") {
        REQUIRE_THROWS_AS(factory.Create({}), UsageException);
    }

    SECTION("One parameter given") {
        REQUIRE_NOTHROW(factory.Create({"0.0"}));
        REQUIRE_NOTHROW(factory.Create({"-0.0"}));
        REQUIRE_NOTHROW(factory.Create({"0.3"}));
        REQUIRE_NOTHROW(factory.Create({"1.0"}));
    }

    SECTION("Two parameters given") {
        REQUIRE_THROWS_AS(factory.Create({"12", "0"}), UsageException);
    }

    SECTION("Given not a number") {
        REQUIRE_THROWS_AS(factory.Create({"a"}), UsageException);
        REQUIRE_THROWS_AS(factory.Create({"-2.01asdf"}), UsageException);
        REQUIRE_THROWS_AS(factory.Create({"10l"}), UsageException);
    }

    SECTION("Not between 0 and 1") {
        REQUIRE_THROWS_AS(factory.Create({"-0.1"}), UsageException);
        REQUIRE_THROWS_AS(factory.Create({"1.1"}), UsageException);
    }
}

TEST_CASE("FFT peaks factory") {
    FFTPeaksFactory factory;
    SECTION("No parameters given") {
        REQUIRE_THROWS_AS(factory.Create({}), UsageException);
    }

    SECTION("One parameter given") {
        REQUIRE_NOTHROW(factory.Create({"0.0"}));
        REQUIRE_NOTHROW(factory.Create({"-0.0"}));
        REQUIRE_NOTHROW(factory.Create({"0.3"}));
        REQUIRE_NOTHROW(factory.Create({"1.0"}));
    }

    SECTION("Two parameters given") {
        REQUIRE_THROWS_AS(factory.Create({"12", "0"}), UsageException);
    }

    SECTION("Three parameters given") {
        REQUIRE_NOTHROW(factory.Create({"0.0", "0.0", "0.0"}));
        REQUIRE_NOTHROW(factory.Create({"-0.0", "-0.0", "-0.0"}));
        REQUIRE_NOTHROW(factory.Create({"0.3", "0.3", "0.3"}));
        REQUIRE_NOTHROW(factory.Create({"1.0", "1.0", "1.0"}));
    }

    SECTION("Given not a number") {
        REQUIRE_THROWS_AS(factory.Create({"a"}), UsageException);
        REQUIRE_THROWS_AS(factory.Create({"-2.01asdf"}), UsageException);
        REQUIRE_THROWS_AS(factory.Create({"10l"}), UsageException);
        REQUIRE_THROWS_AS(factory.Create({"0.3", "a", "a"}), UsageException);
        REQUIRE_THROWS_AS(factory.Create({"0.3", "-2.01asdf", "-2.01asdf"}), UsageException);
        REQUIRE_THROWS_AS(factory.Create({"0.3", "10l", "10l"}), UsageException);
    }

    SECTION("Not between 0 and 1") {
        REQUIRE_THROWS_AS(factory.Create({"-0.1"}), UsageException);
        REQUIRE_THROWS_AS(factory.Create({"1.1"}), UsageException);
        REQUIRE_THROWS_AS(factory.Create({"0.3", "-0.1", "-0.1"}), UsageException);
        REQUIRE_THROWS_AS(factory.Create({"0.3", "1.1", "1.1"}), UsageException);
    }
}

TEST_CASE("Gaussian blur factory") {
    GaussianBlurFactory factory;
    SECTION("No parameters given") {
        REQUIRE_THROWS_AS(factory.Create({}), UsageException);
    }

    SECTION("One parameter given") {
        REQUIRE_NOTHROW(factory.Create({"12"}));
        REQUIRE_NOTHROW(factory.Create({"-2"}));
        REQUIRE_NOTHROW(factory.Create({"1.79"}));
    }

    SECTION("Two parameters given") {
        REQUIRE_THROWS_AS(factory.Create({"100", "200"}), UsageException);
    }

    SECTION("Given not a number") {
        REQUIRE_THROWS_AS(factory.Create({"a"}), UsageException);
        REQUIRE_THROWS_AS(factory.Create({"-2.01asdf"}), UsageException);
        REQUIRE_THROWS_AS(factory.Create({"10l"}), UsageException);
    }
}

TEST_CASE("Grayscale factory") {
    GrayscaleFactory factory;
    SECTION("No parameters given") {
        REQUIRE_NOTHROW(factory.Create({}));
    }

    SECTION("One parameter given") {
        REQUIRE_THROWS_AS(factory.Create({"1"}), UsageException);
    }
}

TEST_CASE("Negative factory") {
    NegativeFactory factory;
    SECTION("No parameters given") {
        REQUIRE_NOTHROW(factory.Create({}));
    }

    SECTION("One parameter given") {
        REQUIRE_THROWS_AS(factory.Create({"1"}), UsageException);
    }
}

TEST_CASE("Sharpening factory") {
    SharpeningFactory factory;
    SECTION("No parameters given") {
        REQUIRE_NOTHROW(factory.Create({}));
    }

    SECTION("One parameter given") {
        REQUIRE_THROWS_AS(factory.Create({"1"}), UsageException);
    }
}