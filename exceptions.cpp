#include "exceptions.h"

const char* ImageProcessorException::what() const noexcept {
    return message_.c_str();
}

SupportException::SupportException(const std::string& message) {
    message_ = "format is not supported: " + message;
}

UsageException::UsageException(const std::string& message) {
    message_ = "incorrect usage: " + message;
}

ReadException::ReadException(const std::string& message) {
    message_ = "could not read from file: " + message;
}

CorruptedFileException::CorruptedFileException(const std::string& message) {
    message_ = "corrupted file: " + message;
}

WriteException::WriteException(const std::string& message) {
    message_ = "could not write to file: " + message;
}

InternalException::InternalException(const std::string& message) {
    message_ = "internal exception: " + message;
}