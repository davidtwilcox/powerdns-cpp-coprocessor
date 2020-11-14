#pragma once

#include <string>

namespace cppbackend {
    class Encoder {
    public:
        static std::string toBase64(const std::string &s);
        static std::string toAES128(const std::string &s, const std::string& password);
    private:
        static constexpr int PASSWORD_LENGTH_128 = 16;
    };
}
