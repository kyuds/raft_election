#include "storage.h"

#include <stdexcept>
#include <plog/Log.h>
#include "utils.h"
#include "sqlcmd.h"

#define RAFT_SQL_DB "raftsql.db"
#define RAFT_METADATA_TERM "term"
#define RAFT_METADATA_VOTED_FOR "voted_for"

namespace raft {

Storage::Storage(const std::string& dir) {
    // open sqlite3 db
    int result = sqlite3_open(combine_paths(dir, RAFT_SQL_DB).c_str(), &db);
    if (result) {
        PLOGE << "Error initializing SQLite: " << sqlite3_errmsg(db) << ".";
        throw std::runtime_error("Unable to initialize SQLite3.");
    } else {
        PLOGI << "Successfully initialized SQLite DB.";
    }

    // create tables
    create_table(CREATE_METADATA_CMD);
    create_table(CREATE_LOG_CMD);

    // initialize all statements
    add_statement(PUT_METADATA, PUT_METADATA_CMD);
}

Storage::~Storage() {
    for (const auto& pair : statements) 
        sqlite3_finalize(pair.second);
    if (db) sqlite3_close(db);
}

// persistent state

bool Storage::save_state() {
    // get sqlite3 statement
    if (statements.find(PUT_METADATA) == statements.end()) {
        PLOGE << "Could not find put_metadata statement.";
        return false;
    }
    sqlite3_stmt* stmt = statements[PUT_METADATA];

    // state-saving is done via a transaction
    int rc;
    char * err_msg = nullptr;
    rc = sqlite3_exec(db, "BEGIN TRANSACTION;", nullptr, nullptr, &err_msg);
    if (rc != SQLITE_OK) {
        PLOGE << "Error beginning transaction: " << err_msg << ".";
        sqlite3_free(err_msg);
        return false;
    }
    
    try {
        // save term
        sqlite3_reset(stmt);
        sqlite3_bind_text(stmt, 1, RAFT_METADATA_TERM, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, std::to_string(_term).c_str(), -1, SQLITE_STATIC);

        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE) {
            throw std::runtime_error("Failed to execute: " + std::string(sqlite3_errmsg(db)));
        }

        // save voted_for
        sqlite3_reset(stmt);
        sqlite3_bind_text(stmt, 1, RAFT_METADATA_VOTED_FOR, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, _voted_for.c_str(), -1, SQLITE_STATIC);

        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE) {
            throw std::runtime_error("Failed to execute: " + std::string(sqlite3_errmsg(db)));
        }

        // commit
        rc = sqlite3_exec(db, "COMMIT;", nullptr, nullptr, &err_msg);
        if (rc != SQLITE_OK) {
            std::string log_msg = "Failed to commit: " + std::string(err_msg);
            sqlite3_free(err_msg);
            throw std::runtime_error(log_msg);
        }

        PLOGI << "saved term " << _term << " and voted_for " << _voted_for
              << " to persistent storage.";
        return true;
    } catch (const std::runtime_error& e) {
        PLOGE << "error saving term " << _term << " and voted_for <"
              << _voted_for << "> to persistent storage. Message: "
              << e.what();
        sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
    }
    return false;
}

void Storage::load_state() {
    // called only once, when node initially starts. Otherwise, term and vote
    // information is persisted to disk, HOWEVER, will also be concurrently
    // updated in memory.
    // will default to term 0 and voted_for "" when EITHER key does not exist.

    // prepare:
    std::string term, voted_for;
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, GET_METADATA_CMD, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        throw std::runtime_error("Unable to create SQLite statement");
    }

    // get metadata:
    if (!get_metadata(stmt, RAFT_METADATA_TERM, term) ||
        !get_metadata(stmt, RAFT_METADATA_VOTED_FOR, voted_for)) {
        term = "0";
        voted_for = "";
        PLOGW << "defaulting to term 0 and voted for <none>.";
    }
    _term = std::stoull(term);
    _voted_for = voted_for;
    PLOGI << "loaded term: " << std::to_string(_term) 
          << " and voted_for: <" << _voted_for << ">";

    // finalize statement:
    sqlite3_finalize(stmt);

    // get start, end indicies and latest log entry
    initialize_log_data();
}

// log entries



// helpers

void Storage::add_statement(const char* name, const char* command) {
    sqlite3_stmt* statement;
    int rc = sqlite3_prepare_v2(db, command, -1, &statement, nullptr);
    if (rc != SQLITE_OK) {
        PLOGE << "Error creating sqlite statement: " << sqlite3_errmsg(db)
              << "; on command: " << command << ".";
        throw std::runtime_error("Unable to create SQLite statement");
    }
    statements[name] = statement;
}

bool Storage::get_metadata(sqlite3_stmt* stmt, const char* n, std::string& o) {
    sqlite3_reset(stmt);
    int rc = sqlite3_bind_text(stmt, 1, n, -1, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        throw std::runtime_error("Unable to bind SQLite statement");
    }
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        const unsigned char * value = sqlite3_column_text(stmt, 0);
        if (value) {
            o = reinterpret_cast<const char*>(value);
            return true;
        }
    } else if (rc != SQLITE_DONE) {
        throw std::runtime_error("Error executing sql statement");
    }
    return false;
}

void Storage::create_table(const char * cmd) {
    int rc;
    char * err_msg = nullptr;
    rc = sqlite3_exec(db, cmd, nullptr, nullptr, &err_msg);
    if (rc != SQLITE_OK) {
        PLOGE << "Error creating table: " << err_msg << ".";
        sqlite3_free(err_msg);
        throw std::runtime_error("Unable to create SQLite3 table.");
    }
}

void Storage::initialize_log_data() {
    int rc;
    sqlite3_stmt * stmt;

    // start, end indicies
    if (sqlite3_prepare_v2(db, MIN_MAX_LOG_CMD, -1, &stmt, nullptr) != SQLITE_OK) {
        PLOGE << "Failed to prepare statement: " << sqlite3_errmsg(db) << ";"
              << " on command: " << MIN_MAX_LOG_CMD << ".";
        throw std::runtime_error("Unable to create SQLite statement");
    }
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        start_index = sqlite3_column_int64(stmt, 0);
        next_index = sqlite3_column_int64(stmt, 1) + 1; // next available
    }
    sqlite3_finalize(stmt);

    // last log entry
    if (sqlite3_prepare_v2(db, LATEST_LOG_CMD, -1, &stmt, nullptr) != SQLITE_OK) {
        PLOGE << "Failed to prepare statement: " << sqlite3_errmsg(db) << ";"
              << " on command: " << LATEST_LOG_CMD << ".";
        throw std::runtime_error("Unable to create SQLite statement");
    }
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        const unsigned char * raw = sqlite3_column_text(stmt, 0);
        if (raw) {
            const char * cast = reinterpret_cast<const char *>(raw);
            if (!deserialize_log(cast, last_entry)) {
                throw std::runtime_error("Unable to deserialize entry");
            }
        }
    }
    sqlite3_finalize(stmt);
}

} // namespace raft
