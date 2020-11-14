#include "../src/repository.h"
#include "common.h"

#include "catch.hpp"

#include <string>

void emptyPathConstructor()
{
    cppbackend::Repository repository("");
}

void badPathConstructor()
{
    cppbackend::Repository repository("/this/path/does/not/exist.db");
}

TEST_CASE("Constructor unhappy path", "[Repository]")
{
    REQUIRE_THROWS(emptyPathConstructor());

    REQUIRE_THROWS(badPathConstructor());
}

TEST_CASE("Query happy path", "[Repository]")
{
    cppbackend::Repository repository(DB_PATH);

    std::string domain{"canberra"};
    int platform = 2;
    std::string expected{"[bob] 33"};

    auto actual = repository.getTXTRecord(domain, platform);
    REQUIRE(expected == actual);
}

TEST_CASE("Query unhappy path", "[Repository]")
{
    cppbackend::Repository repository(DB_PATH);

    SECTION("Domain doesn't exist")
    {
        auto actual = repository.getTXTRecord("notarealdomain", 1);
        REQUIRE(actual.empty());
    }

    SECTION("Platform doesn't exist")
    {
        auto actual = repository.getTXTRecord("hobart", 1);
        REQUIRE(actual.empty());
    }
}
