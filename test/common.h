#pragma once

#include <string>

// Yes, this hard-coding directly to the test database is terrible
// This would never happen in a production project, even in a test
static const std::string DB_PATH {"/home/david/Code/ContentKeeper/CKTask/test/test.db"};
