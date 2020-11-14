#include "repository.h"
#include "fmt/format.h"
#include <stdexcept>
#include <vector>

namespace cppbackend {
    Repository::Repository(const std::string& dbPath)
    {
        if (dbPath.empty())
        {
            throw std::invalid_argument("Database path cannot be empty");
        }

        auto result = sqlite3_open_v2(dbPath.c_str(),
                                      &m_database,
                                      SQLITE_OPEN_READONLY,
                                      nullptr);
        if (result != SQLITE_OK)
        {
            // TODO: Create custom exception
            throw std::runtime_error("Error opening database");
        }

        m_ready = true;
    }

    Repository::~Repository()
    {
        if (m_database && m_ready)
        {
            sqlite3_close_v2(m_database);
            m_ready = false;
        }
    }

    std::string Repository::getTXTRecord(const std::string &domain, int platform) const
    {
        const std::string query =
                fmt::format(
                        "SELECT txt FROM platform JOIN domain ON platform.domain_id = domain.id WHERE domain.name=\"{}\" AND platform.nbr={}",
                        domain,
                        platform);
        std::vector<std::string> txtRecords{};

        sqlite3_stmt* statement;
        if (sqlite3_prepare_v2(m_database, query.c_str(), -1, &statement, 0) == SQLITE_OK)
        {
            int cols = sqlite3_column_count(statement);
            if (cols != 1)
            {
                // TODO: Create custom exception
                throw std::runtime_error(fmt::format("Error in query results. Expected 1 column, received {}", cols));
            }

            int result = sqlite3_step(statement);
            while (result == SQLITE_ROW)
            {
                txtRecords.emplace_back(reinterpret_cast<const char*>(sqlite3_column_text(statement, 0)));
                result = sqlite3_step(statement);
            }

            sqlite3_finalize(statement);
        }

        if (txtRecords.empty())
        {
            return "";
        }
        else if (txtRecords.size() == 1)
        {
            return txtRecords[0];
        }
        else
        {
            throw std::runtime_error(fmt::format("Error in query results. Expected 1 row, received {}", txtRecords.size()));
        }
    }
}
