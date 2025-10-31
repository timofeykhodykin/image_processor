#pragma once

#include <exception>
#include <string>

class ImageProcessorException : public std::exception {
public:
    const char* what() const noexcept final;

protected:
    std::string message_;
};

class SupportException : public ImageProcessorException {
public:
    explicit SupportException(const std::string& message);
};

class UsageException : public ImageProcessorException {
public:
    explicit UsageException(const std::string& message);
};

class ReadException : public ImageProcessorException {
public:
    explicit ReadException(const std::string& message);
};

class CorruptedFileException : public ImageProcessorException {
public:
    explicit CorruptedFileException(const std::string& message);
};

class WriteException : public ImageProcessorException {
public:
    explicit WriteException(const std::string& message);
};

class InternalException : public ImageProcessorException {
public:
    explicit InternalException(const std::string& message);
};