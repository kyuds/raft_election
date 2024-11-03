#ifndef RAFT_SQL
#define RAFT_SQL

// persistent state
constexpr const char * PUT_METADATA = "put_metadata";
constexpr const char * CREATE_METADATA_CMD = "CREATE TABLE IF NOT EXISTS metadata (id VARCHAR(50) PRIMARY KEY, value VARCHAR(255));";
constexpr const char * GET_METADATA_CMD = "SELECT value FROM metadata WHERE id = ?;";
constexpr const char * PUT_METADATA_CMD = "INSERT OR REPLACE INTO metadata (id, value) VALUES (?, ?);";

// log store
constexpr const char * GET_LOG = "get_log";
constexpr const char * GET_LOGS = "get_logs";
constexpr const char * APPEND_LOG = "append_log";
constexpr const char * TRIM_LOG = "trim_log";
constexpr const char * CREATE_LOG_CMD = "CREATE TABLE IF NOT EXISTS logs (id INTEGER PRIMARY KEY, entry TEXT);";
constexpr const char * MIN_MAX_LOG_CMD = "SELECT MIN(id), MAX(id) FROM logs;";
constexpr const char * LATEST_LOG_CMD = "SELECT entry FROM logs ORDER BY id desc LIMIT 1;";
constexpr const char * GET_LOG_CMD = "SELECT entry FROM logs WHERE id = ?;";
constexpr const char * GET_LOGS_CMD = "SELECT entry FROM logs WHERE ? <= id AND id < ? ORDER BY id;";
constexpr const char * APPEND_LOG_CMD = "INSERT INTO logs (id, entry) VALUES (?, ?);";
constexpr const char * TRIM_LOG_CMD = "DELETE FROM logs WHERE id >= ?";

#endif // RAFT_SQL
