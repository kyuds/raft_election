#ifndef RAFT_SQL
#define RAFT_SQL

// persistent state
constexpr const char * PUT_METADATA = "put_metadata";
constexpr const char * CREATE_METADATA_CMD = "CREATE TABLE IF NOT EXISTS metadata (id VARCHAR(50) PRIMARY KEY, value VARCHAR(255));";
constexpr const char * GET_METADATA_CMD = "SELECT value FROM metadata WHERE id = ?;";
constexpr const char * PUT_METADATA_CMD = "INSERT OR REPLACE INTO metadata (id, value) VALUES (?, ?);";

// log store
constexpr const char * CREATE_LOG_CMD = "CREATE TABLE IF NOT EXISTS logs (id INTEGER PRIMARY KEY, entry TEXT);";
constexpr const char * MIN_MAX_LOG_CMD = "SELECT MIN(id), MAX(id) FROM logs;";
constexpr const char * LATEST_LOG_CMD = "SELECT entry FROM logs ORDER BY id desc LIMIT 1;";


#endif // RAFT_SQL
