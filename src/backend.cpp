#include "backend.h"
#include "encoder.h"
#include "repository.h"

#include "fmt/core.h"
#include <sstream>
#include <iostream>
#include <string>
#include <chrono>

namespace cppbackend {
    Backend::Backend(const std::string& dbPath)
        : m_repository(dbPath)
    {
    }

    InputResult Backend::performHandshake(std::istream& input)
    {
        std::string line;
        std::getline(input, line);

        if (line == HANDSHAKE_REQUEST_ABI1 ||
            line == HANDSHAKE_REQUEST_ABI2 ||
            line == HANDSHAKE_REQUEST_ABI3)
        {
            if (line == HANDSHAKE_REQUEST_ABI1) {
                m_abi = 1;
            } else if (line == HANDSHAKE_REQUEST_ABI2) {
                m_abi = 2;
            } else if (line == HANDSHAKE_REQUEST_ABI3) {
                m_abi = 3;
            }

            const auto banner = fmt::format("{}CPP backend starting",
                                            HANDSHAKE_RESPONSE_SUCCESS);
            writeResponse(banner);

            return InputResult(true, banner);
        }
        else
        {
            writeLog(fmt::format("Received '{}'", line));

            const auto banner = RESPONSE_FAIL;
            writeResponse(banner);

            return InputResult(false, banner);
        }
    }

    std::vector<InputResult> Backend::readFromInput(std::istream& input) const
    {
        std::vector<InputResult> results{};

        std::string line;
        while (std::getline(input, line))
        {
            writeLog(fmt::format("Received '{}'", line));

            std::vector<std::string> parsed = Backend::split(line, '\t');
            if (parsed.size() != Backend::getABIParameterCount(m_abi))
            {
                writeResponse("LOG\tReceived unparseable line");

                const auto banner = RESPONSE_FAIL;
                writeResponse(banner);

                results.emplace_back(InputResult{false, banner});

                continue;
            }

            const auto type = parsed[0];
            const auto qname = parsed[1];
            const auto qclass = parsed[2];
            const auto qtype = parsed[3];
            const auto id = parsed[4];

            if (type != "Q")
            {
                writeResponse(fmt::format("LOG\tReceived a bad request type: '{}'", type));

                const auto banner = RESPONSE_FAIL;
                writeResponse(banner);
                results.emplace_back(InputResult{false, banner});

                continue;
            }

            if (qtype != "TXT")
            {
                writeResponse(fmt::format("LOG\tReceived a '{}' type message. Can only process 'TXT' type messages", qtype));

                const auto banner = RESPONSE_FAIL;
                writeResponse(banner);
                results.emplace_back(InputResult{false, banner});

                continue;
            }

            std::string output{};
            if (!performQuery(qname, output))
            {
                writeResponse(fmt::format("LOG\tqname '{}' is invalid", qname));

                const auto banner = RESPONSE_FAIL;
                writeResponse(banner);
                results.emplace_back(InputResult{false, banner});

                continue;
            }

            auto banner = formatResponse(qname, qclass, id, output, m_abi);
            writeResponse(banner);
            results.emplace_back(InputResult{true, banner});

            writeLog("End of data");

            banner = RESPONSE_END;
            writeResponse(banner);
            results.emplace_back(InputResult{true, banner});
        }

        return results;
    }

    int Backend::getABIParameterCount(int abiVersion)
    {

        if (abiVersion < MIN_ABI_VERSION || abiVersion > MAX_ABI_VERSION)
        {
            throw std::invalid_argument(
                    fmt::format("ABI version must be from {} to {}, inclusive. Received {}.",
                                MIN_ABI_VERSION, MAX_ABI_VERSION, abiVersion)
            );
        }
        return ABI_PARAMS[abiVersion - 1];
    }

    std::vector<std::string> Backend::split(const std::string& s, char delimiter)
    {
        std::vector<std::string> tokens;
        std::string token;
        std::istringstream tokenStream(s);
        while (std::getline(tokenStream, token, delimiter))
        {
            tokens.push_back(token);
        }
        return tokens;
    }

    std::string Backend::formatResponse(const std::string& qname,
                                        const std::string& qclass,
                                        const std::string& id,
                                        const std::string& data,
                                        int abiVersion)
    {
        std::string output;
        if (abiVersion == 1 || abiVersion == 2)
        {
            output = fmt::format("DATA\t{}\t{}\tTXT\t3600\t{}\t\"{}\"",
                                 qname, qclass, id, data);
        }
        else if (abiVersion == 3)
        {
            output = fmt::format("DATA\t21\t1\t{}\t{}\tTXT\t3600\t{}\t\"{}\"",
                                 qname, qclass, id, data);
        }

        return output;
    }

    void Backend::writeLog(const std::string& message)
    {
        std::cerr << message << std::endl;
    }

    void Backend::writeResponse(const std::string& message)
    {
        std::cout << message << std::endl;
    }

    bool Backend::performQuery(const std::string& qname, std::string& out) const
    {
        if (qname.empty())
        {
            return false;
        }

        const auto parts = split(qname, '.');
        const auto numParts = parts.size();

        // TODO: De-duplicate this code - move into a lambda
        if (numParts == 3 &&
            parts[2] == "testnet")
        {
            const auto platform = parts[0];
            int platformNbr = 0;
            try {
                platformNbr = std::stoi(platform);
                if (platformNbr < 1 || platformNbr > 5)
                {
                    return false;
                }
            } catch (...) {
                return false;
            }

            const auto domain = parts[1];

            const auto txtRecord = m_repository.getTXTRecord(domain, platformNbr);
            if (txtRecord.empty())
            {
                return false;
            }

            out = Encoder::toBase64(txtRecord);
            return true;
        }
        else if (numParts == 4 &&
                (parts[2] == "oc" && parts[3] == "testnet"))
        {
            const auto platform = parts[0];
            int platformNbr = 0;
            try {
                platformNbr = std::stoi(platform);
                if (platformNbr < 1 || platformNbr > 5)
                {
                    return false;
                }
            } catch (...) {
                return false;
            }

            const auto domain = parts[1];

            const auto txtRecord = m_repository.getTXTRecord(domain, platformNbr);
            if (txtRecord.empty())
            {
                return false;
            }

            const auto now = std::chrono::system_clock::now();
            const auto epoch = now.time_since_epoch();
            const auto seconds = std::chrono::duration_cast<std::chrono::seconds>(epoch);

            out = Encoder::toAES128(
                    fmt::format("{}", seconds.count()),
                    PASSWORD);
            return true;
        }

        // Not 3 or 4 parts as expected, so an invalid request
        return false;
    }
}
