#include <iostream>

#include "controller.h"
#include "exceptions.h"
#include "io.h"
#include "parser.h"

const std::string HELP = R"(DESCRIPTION
    Small console application for applying filters on images.
    Filters are applied in the order they appear in the command.
    Supports only 24-bit uncompressed BMP files.

USAGE
    image_processor <input_path> <output_path> [-filter_name [<params>]]

ARGUMENTS
    input_path
    output_path

FILTERS
    -crop height, width        Crops the image to [height, width]. If image is smaller
                               than requested result by any axis, it stays the same
                               by this axis.
    -gs                        Turns the image into grayscale.
    -neg                       Inverts all colors.
    -sharp                     Sharpens the image.
    -edge threshold            Outlines edges from the image. Threshold has sense only
                               in range from 0 to 1. Higher threshold values produce
                               fewer detected edges.
    -blur sigma                Applies Gaussian blur with parameter sigma. Higher
                                sigma values produce a blurrier image.

    -fft-real [coefficient] [verbose]             Converts image into absolute values of real parts
                                                  of coefficients in frequency domain representation.
                                                  If [coefficient] is given, values are multiplied by it.
                                                  [verbose] should be either 0 or 1 and regulates printing
                                                  50 maximal values (without multiplication by
                                                  [coefficient]).
    -fft-imag [coefficient] [verbose]             Converts image into absolute values of imaginary parts
                                                  of coefficients in frequency domain representation. See -fft-real
                                                  for parameters description.
    -fft-magnitude [coefficient] [verbose]        Converts image into magnitudes of coefficients in frequency domain
                                                  representation. See -fft-real for parameters description.
    -fft-phase [coefficient] [verbose]            Converts image into phases of coefficients in frequency domain
                                                  representation, normalized to have values from 0 to 1.
                                                  See -fft-real for parameters description.

    -fft-lowpass threshold         Keeps frequencies inside rectangle [threshold * height, threshold * width]
                                   in the frequency domain representation of the image and removes others,
                                   then applies inverse Fast Fourier Transform. Threshold must be between 0 and 1.
                                   More the threshold, more frequencies are kept.
    -fft-highpass threshold        Removes frequencies inside rectangle [threshold * height, threshold * width]
                                   in the frequency domain representation of the image and keeps others,
                                   then applies inverse Fast Fourier Transform. Threshold must be between 0 and 1.
                                   More the threshold, fewer frequencies are removed.

    -fft-peaks threshold [safe_zone_height, safe_zone_width]        Removes peaks from frequency domain representation
                                                                    of the image, then applies inverse Fast Fourier
                                                                    Transform. Coefficients with magnitudes greater
                                                                    than threshold are considered peaks. Threshold must
                                                                    be between 0 and 1. If safe zone is specified,
                                                                    rectangle [safe_zone_height * height,
                                                                    safe_zone_width * width] on FFT of the image
                                                                    will not be processed.

EXAMPLES
    $ image_processor a.bmp ./results/b.bmp -crop 20 10 -neg
    $ image_processor a.bmp ./results/b.bmp -sharp -gs -edge 0.3
    $ image_processor a.bmp ./results/b.bmp -blur 4.2
    $ image_processor a.bmp ./results/b.bmp -fft-real 1000 1
    $ image_processor a.bmp ./results/b.bmp -fft-lowpass 0.01
    $ image_processor a.bmp ./results/b.bmp -fft-peaks 0.001 0.01 0.01)";

int main(int argc, char** argv) {
    if (argc == 1) {
        std::cout << HELP << std::endl;
        return 0;
    }
    try {
        const ParserResult params = Parse(argc, argv);
        Image image = ReadImage(params.input_path);
        const std::vector<std::shared_ptr<BaseFilter>> filters = CreateFilters(params.filters);
        ApplyFilters(image, filters);
        WriteImage(image, params.output_path);
    } catch (const ImageProcessorException& exc) {
        std::cerr << exc.what() << std::endl;
    } catch (const std::exception& exc) {
        std::cerr << "Unknown exception: " << exc.what() << " " << std::endl;
    }
    return 0;
}