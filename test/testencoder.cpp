#include "../src/encoder.h"

#include "catch.hpp"

#include <string>

TEST_CASE("Base64 happy path", "[Encoder]")
{
    std::string original = "[test] 1234";
    std::string expected = "W3Rlc3RdIDEyMzQ=";
    std::string actual = cppbackend::Encoder::toBase64(original);

    REQUIRE(expected == actual);
}

TEST_CASE("AES happy path", "[Encoder]")
{
    std::string original = "[test] 1234";
    std::string password = "SECRET_PASS*****";
    std::string expected = "vSIVAw46X7/VZEoBa29vkQ==";
    std::string actual = cppbackend::Encoder::toAES128(original, password);

    REQUIRE(expected == actual);
}
