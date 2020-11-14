#include "../src/backend.h"
#include "common.h"

#include "catch.hpp"
#include "fmt/format.h"

#include <iostream>
#include <sstream>
#include <string>

TEST_CASE("Handshake happy path", "[Backend]")
{
    cppbackend::Backend backend(DB_PATH);
    std::istringstream stream("HELO\t1");
    cppbackend::InputResult handshakeResponse = backend.performHandshake(stream);
    REQUIRE(handshakeResponse.getSuccess() == true);
    REQUIRE(handshakeResponse.getMessage() == "OK\tCPP backend starting");
    REQUIRE(backend.getAbiVersion() == 1);
}

TEST_CASE("Handshake negative path", "[Backend]")
{
    SECTION("Invalid ABI version")
    {
        cppbackend::Backend backend(DB_PATH);
        std::istringstream stream("HELO\t0");
        cppbackend::InputResult handshakeResponse = backend.performHandshake(stream);
        REQUIRE(handshakeResponse.getSuccess() == false);
        REQUIRE(handshakeResponse.getMessage()
                == cppbackend::Backend::RESPONSE_FAIL);
    }

    SECTION("Invalid request")
    {
        cppbackend::Backend backend(DB_PATH);
        std::istringstream stream("INVALID\tREQUEST");
        cppbackend::InputResult handshakeResponse = backend.performHandshake(stream);
        REQUIRE(handshakeResponse.getSuccess() == false);
        REQUIRE(handshakeResponse.getMessage() == cppbackend::Backend::RESPONSE_FAIL);
    }
}

TEST_CASE("Perform query happy path", "[Backend]")
{
    cppbackend::Backend backend(DB_PATH);

    SECTION("TXT record qname")
    {
        const std::string qname{"2.canberra.testnet"};
        const std::string expected{"W2JvYl0gMzM="};
        std::string actual{};
        bool result =  backend.performQuery(qname, actual);

        REQUIRE(result);
        REQUIRE(expected == actual);
    }

    SECTION("AES epoch qname")
    {
        const std::string qname{"2.canberra.oc.testnet"};
        std::string actual{};
        bool result =  backend.performQuery(qname, actual);

        REQUIRE(result);
        REQUIRE_FALSE(actual.empty());
    }
}

TEST_CASE("Perform query negative path", "[Backend]")
{
    cppbackend::Backend backend(DB_PATH);

    SECTION("TXT record empty qname")
    {
        const std::string qname;
        const std::string expected;
        std::string actual{};
        bool result =  backend.performQuery(qname, actual);

        REQUIRE_FALSE(result);
        REQUIRE(expected == actual);
    }

    SECTION("TXT record invalid platform qname 1")
    {
        const std::string qname{"10.canberra.testnet"};
        const std::string expected;
        std::string actual{};
        bool result =  backend.performQuery(qname, actual);

        REQUIRE_FALSE(result);
        REQUIRE(expected == actual);
    }

    SECTION("TXT record invalid platform qname 2")
    {
        const std::string qname{"invalid.canberra.testnet"};
        const std::string expected;
        std::string actual{};
        bool result =  backend.performQuery(qname, actual);

        REQUIRE_FALSE(result);
        REQUIRE(expected == actual);
    }

    SECTION("TXT record invalid domain qname")
    {
        const std::string qname{"2.invalid.testnet"};
        const std::string expected;
        std::string actual{};
        bool result =  backend.performQuery(qname, actual);

        REQUIRE_FALSE(result);
        REQUIRE(expected == actual);
    }

    SECTION("Epoch record invalid qname")
    {
        const std::string qname{"2.canberra.oc.com"};
        const std::string expected;
        std::string actual{};
        bool result =  backend.performQuery(qname, actual);

        REQUIRE_FALSE(result);
        REQUIRE(expected == actual);
    }

    SECTION("Epoch record invalid platform qname 1")
    {
        const std::string qname{"10.canberra.oc.testnet"};
        const std::string expected;
        std::string actual{};
        bool result =  backend.performQuery(qname, actual);

        REQUIRE_FALSE(result);
        REQUIRE(expected == actual);
    }

    SECTION("Epoch record invalid platform qname 2")
    {
        const std::string qname{"invalid.canberra.oc.testnet"};
        const std::string expected;
        std::string actual{};
        bool result =  backend.performQuery(qname, actual);

        REQUIRE_FALSE(result);
        REQUIRE(expected == actual);
    }

    SECTION("Epoch record invalid domain qname")
    {
        const std::string qname{"2.invalid.oc.testnet"};
        const std::string expected;
        std::string actual{};
        bool result =  backend.performQuery(qname, actual);

        REQUIRE_FALSE(result);
        REQUIRE(expected == actual);
    }

    SECTION("Epoch record invalid qname")
    {
        const std::string qname{"2.canberra.oc.com"};
        const std::string expected;
        std::string actual{};
        bool result =  backend.performQuery(qname, actual);

        REQUIRE_FALSE(result);
        REQUIRE(expected == actual);
    }

    SECTION("Invalid qname 1")
    {
        const std::string qname{"2.canberra.com.au.testnet"};
        const std::string expected;
        std::string actual{};
        bool result =  backend.performQuery(qname, actual);

        REQUIRE_FALSE(result);
        REQUIRE(expected == actual);
    }

    SECTION("Invalid qname 1")
    {
        const std::string qname{"2.canberra"};
        const std::string expected;
        std::string actual{};
        bool result =  backend.performQuery(qname, actual);

        REQUIRE_FALSE(result);
        REQUIRE(expected == actual);
    }

    SECTION("Invalid qname 2")
    {
        const std::string qname{"2.canberra."};
        const std::string expected;
        std::string actual{};
        bool result =  backend.performQuery(qname, actual);

        REQUIRE_FALSE(result);
        REQUIRE(expected == actual);
    }

    SECTION("Invalid qname 3")
    {
        const std::string qname{"2.canberra.."};
        const std::string expected;
        std::string actual{};
        bool result =  backend.performQuery(qname, actual);

        REQUIRE_FALSE(result);
        REQUIRE(expected == actual);
    }
}

TEST_CASE("Read from input happy path - TXT records", "[Backend]")
{
    cppbackend::Backend backend(DB_PATH);

    SECTION("Single TXT record query ABI version 1")
    {
        std::istringstream handshakeStream("HELO\t1");
        std::istringstream queryStream("Q\t2.canberra.testnet\tIN\tTXT\t1\t192.168.0.1");

        auto handshakeResponse = backend.performHandshake(handshakeStream);
        REQUIRE(handshakeResponse.getSuccess());

        auto pipeResponses = backend.readFromInput(queryStream);
        REQUIRE(pipeResponses.size() == 2);
        REQUIRE(pipeResponses[0].getSuccess());
        REQUIRE(pipeResponses[0].getMessage() == "DATA\t2.canberra.testnet\tIN\tTXT\t3600\t1\t\"W2JvYl0gMzM=\"");
        REQUIRE(pipeResponses[1].getSuccess());
        REQUIRE(pipeResponses[1].getMessage() == cppbackend::Backend::RESPONSE_END);
    }

    SECTION("Multiple TXT records query ABI version 1")
    {
        std::istringstream handshakeStream("HELO\t1");
        std::istringstream queryStream("Q\t2.canberra.testnet\tIN\tTXT\t1\t192.168.0.1\nQ\t2.canberra.testnet\tIN\tTXT\t2\t192.168.0.2\nQ\t2.canberra.testnet\tIN\tTXT\t3\t192.168.0.3");

        auto handshakeResponse = backend.performHandshake(handshakeStream);
        REQUIRE(handshakeResponse.getSuccess());

        auto pipeResponses = backend.readFromInput(queryStream);
        REQUIRE(pipeResponses.size() == 6);
        for (int i = 1; i < 6; i += 2)
        {
            REQUIRE(pipeResponses[i - 1].getSuccess());
            REQUIRE(pipeResponses[i].getSuccess());
            REQUIRE(pipeResponses[i - 1].getMessage() == fmt::format("DATA\t2.canberra.testnet\tIN\tTXT\t3600\t{}\t\"W2JvYl0gMzM=\"", (i + 1) / 2));
            REQUIRE(pipeResponses[i].getMessage() == cppbackend::Backend::RESPONSE_END);
        }
    }

    SECTION("Single TXT record query ABI version 2")
    {
        std::istringstream handshakeStream("HELO\t2");
        std::istringstream queryStream("Q\t2.canberra.testnet\tIN\tTXT\t1\t192.168.0.1\t10.1.1.1");

        auto handshakeResponse = backend.performHandshake(handshakeStream);
        REQUIRE(handshakeResponse.getSuccess());

        auto pipeResponses = backend.readFromInput(queryStream);
        REQUIRE(pipeResponses.size() == 2);
        REQUIRE(pipeResponses[0].getSuccess());
        REQUIRE(pipeResponses[0].getMessage() == "DATA\t2.canberra.testnet\tIN\tTXT\t3600\t1\t\"W2JvYl0gMzM=\"");
        REQUIRE(pipeResponses[1].getSuccess());
        REQUIRE(pipeResponses[1].getMessage() == cppbackend::Backend::RESPONSE_END);
    }

    SECTION("Multiple TXT records query ABI version 2")
    {
        std::istringstream handshakeStream("HELO\t2");
        std::istringstream queryStream("Q\t2.canberra.testnet\tIN\tTXT\t1\t192.168.0.1\t10.1.1.1\nQ\t2.canberra.testnet\tIN\tTXT\t2\t192.168.0.2\t10.1.1.1\nQ\t2.canberra.testnet\tIN\tTXT\t3\t192.168.0.3\t10.1.1.1");

        auto handshakeResponse = backend.performHandshake(handshakeStream);
        REQUIRE(handshakeResponse.getSuccess());

        auto pipeResponses = backend.readFromInput(queryStream);
        REQUIRE(pipeResponses.size() == 6);
        for (int i = 1; i < 6; i += 2)
        {
            REQUIRE(pipeResponses[i - 1].getSuccess());
            REQUIRE(pipeResponses[i].getSuccess());
            REQUIRE(pipeResponses[i - 1].getMessage() == fmt::format("DATA\t2.canberra.testnet\tIN\tTXT\t3600\t{}\t\"W2JvYl0gMzM=\"", (i + 1) / 2));
            REQUIRE(pipeResponses[i].getMessage() == cppbackend::Backend::RESPONSE_END);
        }
    }

    SECTION("Single TXT record query ABI version 3")
    {
        std::istringstream handshakeStream("HELO\t3");
        std::istringstream queryStream("Q\t2.canberra.testnet\tIN\tTXT\t1\t192.168.0.1\t10.1.1.1\t0.0.0.0/0");

        auto handshakeResponse = backend.performHandshake(handshakeStream);
        REQUIRE(handshakeResponse.getSuccess());

        auto pipeResponses = backend.readFromInput(queryStream);
        REQUIRE(pipeResponses.size() == 2);
        REQUIRE(pipeResponses[0].getSuccess());
        REQUIRE(pipeResponses[0].getMessage() == "DATA\t21\t1\t2.canberra.testnet\tIN\tTXT\t3600\t1\t\"W2JvYl0gMzM=\"");
        REQUIRE(pipeResponses[1].getSuccess());
        REQUIRE(pipeResponses[1].getMessage() == cppbackend::Backend::RESPONSE_END);
    }

    SECTION("Multiple TXT records query ABI version 3")
    {
        std::istringstream handshakeStream("HELO\t3");
        std::istringstream queryStream("Q\t2.canberra.testnet\tIN\tTXT\t1\t192.168.0.1\t10.1.1.1\t0.0.0.0/0\nQ\t2.canberra.testnet\tIN\tTXT\t2\t192.168.0.2\t10.1.1.1\t0.0.0.0/0\nQ\t2.canberra.testnet\tIN\tTXT\t3\t192.168.0.3\t10.1.1.1\t0.0.0.0/0");

        auto handshakeResponse = backend.performHandshake(handshakeStream);
        REQUIRE(handshakeResponse.getSuccess());

        auto pipeResponses = backend.readFromInput(queryStream);
        REQUIRE(pipeResponses.size() == 6);
        for (int i = 1; i < 6; i += 2)
        {
            REQUIRE(pipeResponses[i - 1].getSuccess());
            REQUIRE(pipeResponses[i].getSuccess());
            REQUIRE(pipeResponses[i - 1].getMessage() == fmt::format("DATA\t21\t1\t2.canberra.testnet\tIN\tTXT\t3600\t{}\t\"W2JvYl0gMzM=\"", (i + 1) / 2));
            REQUIRE(pipeResponses[i].getMessage() == cppbackend::Backend::RESPONSE_END);
        }
    }
}

TEST_CASE("Read from input happy path - epoch records", "[Backend]")
{
    cppbackend::Backend backend(DB_PATH);

    SECTION("Single epoch record query ABI version 1")
    {
        std::istringstream handshakeStream("HELO\t1");
        std::istringstream queryStream("Q\t2.canberra.oc.testnet\tIN\tTXT\t1\t192.168.0.1");

        auto handshakeResponse = backend.performHandshake(handshakeStream);
        REQUIRE(handshakeResponse.getSuccess());

        auto pipeResponses = backend.readFromInput(queryStream);
        REQUIRE(pipeResponses.size() == 2);
        REQUIRE(pipeResponses[0].getSuccess());
        REQUIRE(pipeResponses[0].getMessage().substr(0, 42) == "DATA\t2.canberra.oc.testnet\tIN\tTXT\t3600\t1\t\"");
        REQUIRE(pipeResponses[1].getSuccess());
        REQUIRE(pipeResponses[1].getMessage() == cppbackend::Backend::RESPONSE_END);
    }

    SECTION("Multiple epoch records query ABI version 1")
    {
        std::istringstream handshakeStream("HELO\t1");
        std::istringstream queryStream("Q\t2.canberra.oc.testnet\tIN\tTXT\t1\t192.168.0.1\nQ\t2.canberra.oc.testnet\tIN\tTXT\t2\t192.168.0.2\nQ\t2.canberra.oc.testnet\tIN\tTXT\t3\t192.168.0.3");

        auto handshakeResponse = backend.performHandshake(handshakeStream);
        REQUIRE(handshakeResponse.getSuccess());

        auto pipeResponses = backend.readFromInput(queryStream);
        REQUIRE(pipeResponses.size() == 6);
        for (int i = 1; i < 6; i += 2)
        {
            REQUIRE(pipeResponses[i - 1].getSuccess());
            REQUIRE(pipeResponses[i].getSuccess());
            REQUIRE(pipeResponses[i - 1].getMessage().substr(0, 42) == fmt::format("DATA\t2.canberra.oc.testnet\tIN\tTXT\t3600\t{}\t\"", (i + 1) / 2));
            REQUIRE(pipeResponses[i].getMessage() == cppbackend::Backend::RESPONSE_END);
        }
    }

    SECTION("Single epoch record query ABI version 2")
    {
        std::istringstream handshakeStream("HELO\t2");
        std::istringstream queryStream("Q\t2.canberra.oc.testnet\tIN\tTXT\t1\t192.168.0.1\t10.1.1.1");

        auto handshakeResponse = backend.performHandshake(handshakeStream);
        REQUIRE(handshakeResponse.getSuccess());

        auto pipeResponses = backend.readFromInput(queryStream);
        REQUIRE(pipeResponses.size() == 2);
        REQUIRE(pipeResponses[0].getSuccess());
        REQUIRE(pipeResponses[0].getMessage().substr(0, 42) == "DATA\t2.canberra.oc.testnet\tIN\tTXT\t3600\t1\t\"");
        REQUIRE(pipeResponses[1].getSuccess());
        REQUIRE(pipeResponses[1].getMessage() == cppbackend::Backend::RESPONSE_END);
    }

    SECTION("Multiple epoch records query ABI version 2")
    {
        std::istringstream handshakeStream("HELO\t2");
        std::istringstream queryStream("Q\t2.canberra.oc.testnet\tIN\tTXT\t1\t192.168.0.1\t10.1.1.1\nQ\t2.canberra.oc.testnet\tIN\tTXT\t2\t192.168.0.2\t10.1.1.1\nQ\t2.canberra.oc.testnet\tIN\tTXT\t3\t192.168.0.3\t10.1.1.1");

        auto handshakeResponse = backend.performHandshake(handshakeStream);
        REQUIRE(handshakeResponse.getSuccess());

        auto pipeResponses = backend.readFromInput(queryStream);
        REQUIRE(pipeResponses.size() == 6);
        for (int i = 1; i < 6; i += 2)
        {
            REQUIRE(pipeResponses[i - 1].getSuccess());
            REQUIRE(pipeResponses[i].getSuccess());
            REQUIRE(pipeResponses[i - 1].getMessage().substr(0, 42) == fmt::format("DATA\t2.canberra.oc.testnet\tIN\tTXT\t3600\t{}\t\"", (i + 1) / 2));
            REQUIRE(pipeResponses[i].getMessage() == cppbackend::Backend::RESPONSE_END);
        }
    }

    SECTION("Single epoch record query ABI version 3")
    {
        std::istringstream handshakeStream("HELO\t3");
        std::istringstream queryStream("Q\t2.canberra.oc.testnet\tIN\tTXT\t1\t192.168.0.1\t10.1.1.1\t0.0.0.0/0");

        auto handshakeResponse = backend.performHandshake(handshakeStream);
        REQUIRE(handshakeResponse.getSuccess());

        auto pipeResponses = backend.readFromInput(queryStream);
        REQUIRE(pipeResponses.size() == 2);
        REQUIRE(pipeResponses[0].getSuccess());
        REQUIRE(pipeResponses[0].getMessage().substr(0, 47) == "DATA\t21\t1\t2.canberra.oc.testnet\tIN\tTXT\t3600\t1\t\"");
        REQUIRE(pipeResponses[1].getSuccess());
        REQUIRE(pipeResponses[1].getMessage() == cppbackend::Backend::RESPONSE_END);
    }

    SECTION("Multiple epoch records query ABI version 3")
    {
        std::istringstream handshakeStream("HELO\t3");
        std::istringstream queryStream("Q\t2.canberra.oc.testnet\tIN\tTXT\t1\t192.168.0.1\t10.1.1.1\t0.0.0.0/0\nQ\t2.canberra.oc.testnet\tIN\tTXT\t2\t192.168.0.2\t10.1.1.1\t0.0.0.0/0\nQ\t2.canberra.oc.testnet\tIN\tTXT\t3\t192.168.0.3\t10.1.1.1\t0.0.0.0/0");

        auto handshakeResponse = backend.performHandshake(handshakeStream);
        REQUIRE(handshakeResponse.getSuccess());

        auto pipeResponses = backend.readFromInput(queryStream);
        REQUIRE(pipeResponses.size() == 6);
        for (int i = 1; i < 6; i += 2)
        {
            REQUIRE(pipeResponses[i - 1].getSuccess());
            REQUIRE(pipeResponses[i].getSuccess());
            REQUIRE(pipeResponses[i - 1].getMessage().substr(0, 47) == fmt::format("DATA\t21\t1\t2.canberra.oc.testnet\tIN\tTXT\t3600\t{}\t\"", (i + 1) / 2));
            REQUIRE(pipeResponses[i].getMessage() == cppbackend::Backend::RESPONSE_END);
        }
    }
}

TEST_CASE("Read from input - unhappy path", "[Backend]")
{
    cppbackend::Backend backend(DB_PATH);

    SECTION("Invalid query - wrong length for ABI 1")
    {
        std::istringstream handshakeStream("HELO\t1");
        std::istringstream queryStream("Q\t2.canberra.oc.testnet\tIN\tTXT\t1\t192.168.0.1\t10.1.1.1");

        auto handshakeResponse = backend.performHandshake(handshakeStream);
        REQUIRE(handshakeResponse.getSuccess());

        auto pipeResponses = backend.readFromInput(queryStream);
        REQUIRE_FALSE(pipeResponses[0].getSuccess());
        REQUIRE(pipeResponses[0].getMessage() == cppbackend::Backend::RESPONSE_FAIL);
    }

    SECTION("Invalid query - wrong length for ABI 2")
    {
        std::istringstream handshakeStream("HELO\t2");
        std::istringstream queryStream("Q\t2.canberra.oc.testnet\tIN\tTXT\t1\t192.168.0.1\t10.1.1.1\t0.0.0.0/0");

        auto handshakeResponse = backend.performHandshake(handshakeStream);
        REQUIRE(handshakeResponse.getSuccess());

        auto pipeResponses = backend.readFromInput(queryStream);
        REQUIRE_FALSE(pipeResponses[0].getSuccess());
        REQUIRE(pipeResponses[0].getMessage() == cppbackend::Backend::RESPONSE_FAIL);
    }

    SECTION("Invalid query - wrong length for ABI 3")
    {
        std::istringstream handshakeStream("HELO\t3");
        std::istringstream queryStream("Q\t2.canberra.oc.testnet\tIN\tTXT\t1\t192.168.0.1\t10.1.1.1\t0.0.0.0/0\t123456");

        auto handshakeResponse = backend.performHandshake(handshakeStream);
        REQUIRE(handshakeResponse.getSuccess());

        auto pipeResponses = backend.readFromInput(queryStream);
        REQUIRE_FALSE(pipeResponses[0].getSuccess());
        REQUIRE(pipeResponses[0].getMessage() == cppbackend::Backend::RESPONSE_FAIL);
    }

    SECTION("Invalid query - invalid type")
    {
        std::istringstream handshakeStream("HELO\t1");
        std::istringstream queryStream("R\t2.canberra.oc.testnet\tIN\tTXT\t1\t192.168.0.1\t10.1.1.1");

        auto handshakeResponse = backend.performHandshake(handshakeStream);
        REQUIRE(handshakeResponse.getSuccess());

        auto pipeResponses = backend.readFromInput(queryStream);
        REQUIRE_FALSE(pipeResponses[0].getSuccess());
        REQUIRE(pipeResponses[0].getMessage() == cppbackend::Backend::RESPONSE_FAIL);
    }

    SECTION("Invalid query - invalid qtype")
    {
        std::istringstream handshakeStream("HELO\t1");
        std::istringstream queryStream("Q\t2.canberra.oc.testnet\tIN\tSOA\t1\t192.168.0.1\t10.1.1.1");

        auto handshakeResponse = backend.performHandshake(handshakeStream);
        REQUIRE(handshakeResponse.getSuccess());

        auto pipeResponses = backend.readFromInput(queryStream);
        REQUIRE_FALSE(pipeResponses[0].getSuccess());
        REQUIRE(pipeResponses[0].getMessage() == cppbackend::Backend::RESPONSE_FAIL);
    }

    SECTION("Invalid query - invalid qname")
    {
        std::istringstream handshakeStream("HELO\t1");
        std::istringstream queryStream("Q\t2.canberra.au\tIN\tTXT\t1\t192.168.0.1\t10.1.1.1");

        auto handshakeResponse = backend.performHandshake(handshakeStream);
        REQUIRE(handshakeResponse.getSuccess());

        auto pipeResponses = backend.readFromInput(queryStream);
        REQUIRE_FALSE(pipeResponses[0].getSuccess());
        REQUIRE(pipeResponses[0].getMessage() == cppbackend::Backend::RESPONSE_FAIL);
    }
}