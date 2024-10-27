#ifndef RAFT_STORAGE_SQL
#define RAFT_STORAGE_SQL

#include <string>
#include <unordered_map>
#include <sqlite3.h>

namespace raft {

// All storage system operations happen either single-threadedly OR the 
// raft_mutex held. Therefore, there is no need for synchronization here.

class Storage {
public:
    Storage(const std::string& dir);
    ~Storage();

private:
    void add_statement(const char* name, const char* command);

    sqlite3 * db;
    std::unordered_map<std::string, sqlite3_stmt*> statements;

public:  // raft state
    bool save_state();
    void load_state();

    uint64_t term() { return _term; }
    void inc_term() { _term++; }
    void set_term(const uint64_t t) { _term = t; }

    const std::string& voted_for() { return _voted_for; }
    void set_voted_for(const std::string& v) { _voted_for = v; }

private: // raft state
    bool get_metadata(sqlite3_stmt* stmt, const char* n, std::string& o);

    uint64_t _term;
    std::string _voted_for;

public:  // raft logs
    // TODO
private: // raft logs
    // TODO
};

// SQL commands: persistent state
constexpr const char * PUT_METADATA = "put_metadata";
constexpr const char * CREATE_METADATA_CMD = "CREATE TABLE IF NOT EXISTS metadata (id VARCHAR(50) PRIMARY KEY, value VARCHAR(255));";
constexpr const char * GET_METADATA_CMD = "SELECT value FROM metadata WHERE id = ?;";
constexpr const char * PUT_METADATA_CMD = "INSERT OR REPLACE INTO metadata (id, value) VALUES (?, ?);";
} // namespace raft

#endif // RAFT_STORAGE_SQL
