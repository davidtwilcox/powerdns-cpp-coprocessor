#pragma once

#include <string>

#include "sqlite3.h"

namespace cppbackend {
    class Repository {
    public:
        Repository(const std::string& dbPath);
        ~Repository();

        std::string getTXTRecord(const std::string& domain, int platform) const;
    private:
        bool m_ready = false;
        sqlite3* m_database = nullptr;
    };
}
