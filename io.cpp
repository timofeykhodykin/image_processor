#include "io.h"

#include "exceptions.h"

#include <fstream>
#include <iostream>
#include <string>

#include <cstdint>

BinaryReader::BinaryReader() {
}

BinaryReader::BinaryReader(const std::string& filename) : in_(filename, std::ios::binary) {
}

template <std::integral T>
void BinaryReader::Read(T& value) {
    value = 0;
    for (int i = 0; i < sizeof(T); i++) {
        char buffer = '\0';
        try {
            in_.read(&buffer, 1);
        } catch (const std::exception& exc) {
            throw ReadException(exc.what());
        }
        if (in_.eof()) {
            throw ReadException("reached end of file");
        }
        constexpr uint8_t ByteSize = 8;
        value |= static_cast<uint8_t>(buffer) << (i * ByteSize);
    }
}

void BinaryReader::Skip(size_t bytes_count) {
    char buffer = '\0';
    while (bytes_count--) {
        try {
            in_.read(&buffer, 1);
        } catch (const std::exception& exc) {
            throw ReadException(exc.what());
        }
        if (in_.eof()) {
            throw ReadException("reached end of file");
        }
    }
}

BinaryWriter::BinaryWriter() {
}

BinaryWriter::BinaryWriter(const std::string& filename) : out_(filename, std::ios::binary) {
}

template <std::integral T>
void BinaryWriter::Write(const T& value) {
    try {
        for (size_t i = 0; i < sizeof(T); i++) {
            constexpr uint8_t ByteSize = 8;
            constexpr uint8_t ByteMask = 0xFF;
            char c = static_cast<char>(static_cast<uint8_t>(value >> (i * ByteSize)) & ByteMask);
            out_.write(&c, 1);
        }
    } catch (const std::exception& exc) {
        throw WriteException(exc.what());
    }
}

void BinaryWriter::WriteZero(size_t bytes_count) {
    try {
        constexpr char Zero = '\0';
        while (bytes_count--) {
            out_.write(&Zero, 1);
        }
    } catch (const std::exception& exc) {
        throw WriteException(exc.what());
    }
}

Image ReadImage(const std::string& filename) {
    BinaryReader reader;
    try {
        reader = BinaryReader(filename);
    } catch (const std::exception& exc) {
        throw ReadException(exc.what());
    }

    char bf_type1 = 0;
    char bf_type2 = 0;
    uint32_t bf_size = 0;
    uint16_t bf_reserved1 = 0;
    uint16_t bf_reserved2 = 0;
    uint32_t bf_off_bits = 0;

    uint32_t bi_size = 0;
    uint32_t bi_width = 0;
    int32_t bi_height = 0;
    uint16_t bi_planes = 0;
    uint16_t bi_bit_count = 0;
    uint32_t bi_compression = 0;
    uint32_t bi_size_image = 0;
    uint32_t bi_x_pels_per_meter = 0;
    uint32_t bi_y_pels_per_meter = 0;
    uint32_t bi_clr_used = 0;
    uint32_t bi_clr_important = 0;

    reader.Read(bf_type1);
    if (bf_type1 != 'B') {
        throw CorruptedFileException("file must start with B");
    }
    reader.Read(bf_type2);
    if (bf_type2 != 'M') {
        throw CorruptedFileException("file must start with BM");
    }

    reader.Read(bf_size);
    reader.Read(bf_reserved1);
    reader.Read(bf_reserved2);
    if (bf_reserved1 != 0 || bf_reserved2 != 0) {
        throw CorruptedFileException("reserved fields are not zero");
    }

    reader.Read(bf_off_bits);

    constexpr size_t MinImageHeaderSize = 40;
    reader.Read(bi_size);
    if (bi_size < MinImageHeaderSize) {
        throw CorruptedFileException("declared image header length must be at least 40 bytes");
    }

    reader.Read(bi_width);
    reader.Read(bi_height);

    reader.Read(bi_planes);
    if (bi_planes != 1) {
        throw CorruptedFileException("biPlanes must be equal to 1");
    }

    constexpr size_t BitsPerPixel = 24;
    reader.Read(bi_bit_count);
    if (bi_bit_count != BitsPerPixel) {
        throw SupportException("files with encoding different from 24 bits per pixel are not supported");
    }

    reader.Read(bi_compression);
    if (bi_compression != 0) {
        throw SupportException("compressed files are not supported");
    }

    reader.Read(bi_size_image);
    if (bi_size_image != 0 && bi_size_image != 3 * std::abs(bi_height) * bi_width) {
        throw CorruptedFileException("incorrect size of the image: " + std::to_string(bi_size_image) + ", should be" +
                                     std::to_string(3 * std::abs(bi_height) * bi_width));
    }

    reader.Read(bi_x_pels_per_meter);
    reader.Read(bi_y_pels_per_meter);

    reader.Read(bi_clr_used);
    if (bi_clr_used != 0) {
        throw SupportException("files with custom color map are not supported");
    }

    reader.Read(bi_clr_important);
    if (bi_clr_important != 0) {
        throw SupportException("files with custom significant colors are not supported");
    }

    constexpr size_t FileHeaderSize = 54;
    if (bf_off_bits < FileHeaderSize) {
        throw CorruptedFileException("bfOffBits must be at least the size of the header");
    }
    reader.Skip(bf_off_bits - FileHeaderSize);

    std::vector pixels(std::abs(bi_height), std::vector<Color>(bi_width));

    constexpr size_t DwordSize = 4;
    const size_t padding = ((bi_width * 3 + DwordSize - 1) / DwordSize) * DwordSize - bi_width * 3;
    for (size_t i = 0; i < std::abs(bi_height); ++i) {
        for (size_t j = 0; j < bi_width; ++j) {
            uint8_t r = 0;
            uint8_t g = 0;
            uint8_t b = 0;
            reader.Read(b);
            reader.Read(g);
            reader.Read(r);

            constexpr double ColorMaxValue = 255.0;
            pixels[(bi_height >= 0 ? bi_height - i - 1 : i)][j] =
                Color(static_cast<double>(r) / ColorMaxValue, static_cast<double>(g) / ColorMaxValue,
                      static_cast<double>(b) / ColorMaxValue);
        }
        reader.Skip(padding);
    }

    bool reached_eof = false;
    try {
        reader.Skip(1);
    } catch (const ReadException& exc) {
        if (std::string(exc.what()) == "could not read from file: reached end of file") {
            reached_eof = true;
        } else {
            throw ReadException(exc.what());
        }
    }

    if (!reached_eof) {
        throw CorruptedFileException("file has extra bytes in the end");
    }

    if (bf_size != bf_off_bits + std::abs(bi_height) * (3 * bi_width + padding)) {
        throw CorruptedFileException("incorrect declared size of the file");
    }

    return Image(std::move(pixels));
}

void WriteImage(const Image& image, const std::string& filename) {
    BinaryWriter writer;
    try {
        writer = BinaryWriter(filename);
    } catch (const std::exception& exc) {
        throw WriteException(exc.what());
    }

    constexpr size_t DwordSize = 4;
    constexpr size_t FileHeaderSize = 54;
    constexpr size_t ImageHeaderSize = 40;
    constexpr size_t BitsPerPixel = 24;
    const size_t padding = ((image.GetWidth() * 3 + DwordSize - 1) / DwordSize) * DwordSize - image.GetWidth() * 3;

    writer.Write('B');  // bfType
    writer.Write('M');  // bfType
    writer.Write(
        static_cast<uint32_t>(FileHeaderSize + image.GetHeight() * (image.GetWidth() * 3 + padding)));  // bfSize
    writer.Write(static_cast<uint16_t>(0));                                                             // bfReserved1
    writer.Write(static_cast<uint16_t>(0));                                                             // bfReserved2
    writer.Write(static_cast<uint32_t>(FileHeaderSize));                                                // bfOffBits

    writer.Write(static_cast<uint32_t>(ImageHeaderSize));                           // biSize
    writer.Write(static_cast<uint32_t>(image.GetWidth()));                          // biWidth
    writer.Write(static_cast<int32_t>(-image.GetHeight()));                         // biHeight
    writer.Write(static_cast<uint16_t>(1));                                         // biPlanes
    writer.Write(static_cast<uint16_t>(BitsPerPixel));                              // biBitCount
    writer.Write(static_cast<uint32_t>(0));                                         // biCompression
    writer.Write(static_cast<uint32_t>(3 * image.GetHeight() * image.GetWidth()));  // biSizeImage
    writer.Write(static_cast<uint32_t>(0));                                         // biXPelsPerMeter
    writer.Write(static_cast<uint32_t>(0));                                         // biYPelsPerMeter
    writer.Write(static_cast<uint32_t>(0));                                         // biClrUsed
    writer.Write(static_cast<uint32_t>(0));                                         // biClrImportant

    for (size_t i = 0; i < image.GetHeight(); ++i) {
        for (size_t j = 0; j < image.GetWidth(); ++j) {
            const Color color = image.GetPixel(i, j);
            constexpr uint8_t ColorMaxValue = 255;
            writer.Write(static_cast<uint8_t>(color.b * ColorMaxValue));
            writer.Write(static_cast<uint8_t>(color.g * ColorMaxValue));
            writer.Write(static_cast<uint8_t>(color.r * ColorMaxValue));
        }
        writer.WriteZero(padding);
    }
}