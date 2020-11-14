#pragma once

#include "repository.h"

#include <string>
#include <utility>
#include <vector>
#include <iostream>

namespace cppbackend {
    class InputResult {
    public:
        InputResult(bool success, std::string  message)
            : m_success{success},
              m_message{std::move(message)}
        {};

        [[nodiscard]] bool getSuccess() const { return m_success; }
        [[nodiscard]] std::string getMessage() const { return m_message; }
    private:
        const bool m_success;
        const std::string m_message;
    };

    class Backend {
    public:
        explicit Backend(const std::string& dbPath);
        ~Backend() = default;

        [[nodiscard]] InputResult performHandshake(std::istream& input);
        [[nodiscard]] std::vector<InputResult> readFromInput(std::istream& input) const;

        [[nodiscard]] bool performQuery(const std::string& qname, std::string& out) const;

        [[nodiscard]] inline int getAbiVersion() const { return m_abi; }

        static inline std::string const HANDSHAKE_REQUEST_ABI1 = "HELO\t1";
        static inline std::string const HANDSHAKE_REQUEST_ABI2 = "HELO\t2";
        static inline std::string const HANDSHAKE_REQUEST_ABI3 = "HELO\t3";
        static inline std::string const HANDSHAKE_RESPONSE_SUCCESS = "OK\t";
        static inline std::string const RESPONSE_FAIL = "FAIL";
        static inline std::string const RESPONSE_END = "END";
    private:
        static constexpr int MIN_ABI_VERSION = 1;
        static constexpr int MAX_ABI_VERSION = 3;
        static constexpr int ABI_PARAMS[] = {6, 7, 8};

        // Yep, put the password in the source code. Terrible idea, especially in the
        // header. This is a proof of concept project, not production code. Forgive me.
        // Also, the password is padded with * to bring it to the minimum 16 characters
        // required for AES-128.
        static inline std::string const PASSWORD = "SECRET_PASS*****";

        int m_abi = 0;
        Repository m_repository;

        static int getABIParameterCount(int abiVersion);
        static std::vector<std::string> split(const std::string& s, char delimiter);
        static std::string formatResponse(const std::string& qname,
                                          const std::string& qclass,
                                          const std::string& id,
                                          const std::string& data,
                                          int abiVersion);
        static void writeLog(const std::string& message);
        static void writeResponse(const std::string& message);
    };
}
