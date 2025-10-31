include(FetchContent)

FetchContent_Declare(
        catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.5.2
)

FetchContent_MakeAvailable(catch2)

add_executable(tests test.cpp
        ../exceptions.cpp
        ../parser.cpp
        ../io.cpp
        ../image.cpp
        ../controller.cpp
        ../fft.cpp

        ../filters/base_filter.cpp
        ../filters/crop_filter.cpp
        ../filters/edge_filter.cpp
        ../filters/gaussian_blur_filter.cpp
        ../filters/grayscale_filter.cpp
        ../filters/negative_filter.cpp
        ../filters/sharpening_filter.cpp
        ../filters/fft_filters.cpp
        ../filters/matrix_filter.cpp

        ../factories/base_factory.cpp
        ../factories/crop_factory.cpp
        ../factories/edge_factory.cpp
        ../factories/gaussian_blur_factory.cpp
        ../factories/grayscale_factory.cpp
        ../factories/negative_factory.cpp
        ../factories/sharpening_factory.cpp
        ../factories/fft_factories.cpp)

target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)