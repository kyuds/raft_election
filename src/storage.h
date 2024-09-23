#ifndef RAFT_STORAGE
#define RAFT_STORAGE

#include <memory>
#include <string>

#include <rocksdb/db.h>

#define RAFT_TERM_KEY "term"
#define RAFT_VOTED_FOR_KEY "voted_for"

namespace raft {

class Storage {
public:
    Storage(const std::string& dir);
    ~Storage();

private:
    rocksdb::DB * db;

public: // raft state
    bool save_state();
    void load_state();

    uint64_t term() { return _term; }
    void inc_term() { _term++; }
    void set_term(const uint64_t t) { _term = t; }

    const std::string& voted_for() { return _voted_for; }
    void set_voted_for(const std::string& v) { _voted_for = v; }

private: // raft state
    uint64_t _term;
    std::string _voted_for;
};

} // namespace raft

#endif // RAFT_STORAGE
