#pragma once

#include "image.h"

#include <fstream>
#include <string>

class BinaryReader {
public:
    BinaryReader();
    explicit BinaryReader(const std::string& filename);

    template <std::integral T>
    void Read(T& value);

    void Skip(size_t bytes_count);

private:
    std::ifstream in_;
};

class BinaryWriter {
public:
    BinaryWriter();
    explicit BinaryWriter(const std::string& filename);

    template <std::integral T>
    void Write(const T& value);

    void WriteZero(size_t bytes_count);

private:
    std::ofstream out_;
};

Image ReadImage(const std::string& filename);

void WriteImage(const Image& image, const std::string& filename);