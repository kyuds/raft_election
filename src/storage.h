#ifndef RAFT_STORAGE_SQL
#define RAFT_STORAGE_SQL

#include <string>
#include <unordered_map>
#include <vector>
#include <sqlite3.h>

#include "log.pb.h"

namespace raft {

// All storage system operations happen either single-threadedly OR the 
// raft_mutex held. Therefore, there is no need for synchronization here.

class Storage {
public:
    Storage(const std::string& dir);
    ~Storage();

public:  // raft state
    bool save_state();
    void load_state();

    int64_t term() { return _term; }
    void inc_term() { _term++; }
    void set_term(const int64_t t) { assert(t >= 0); _term = t; }

    const std::string& voted_for() { return _voted_for; }
    void set_voted_for(const std::string& v) { _voted_for = v; }

private: // raft state
    int64_t _term;
    std::string _voted_for;

public:  // raft logs
    int64_t get_available_index() { return next_index; }
    int64_t get_start_index() { return start_index; }
    LogEntry * get_last_entry() { return last_entry; }

    int64_t append_log(const LogEntry& log);
    void write_log(int64_t index, const LogEntry& log);
    std::vector<LogEntry*> get_logs(int64_t start, int64_t end);
    LogEntry * get_log(int64_t index);

private: // raft logs
    int64_t start_index = 1;
    int64_t next_index = 1;
    LogEntry * last_entry = nullptr;

private: // db
    sqlite3 * db;
    std::unordered_map<std::string, sqlite3_stmt*> statements;

private: // helpers
    void add_statement(const char* name, const char* command);
    bool get_metadata(sqlite3_stmt* stmt, const char* n, std::string& o);
    void create_table(const char * cmd);
    void initialize_log_data();
};

} // namespace raft

#endif // RAFT_STORAGE_SQL
