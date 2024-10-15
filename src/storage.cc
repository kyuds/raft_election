#include <iostream>
#include <stdexcept>

#include <plog/Log.h>
#include <rocksdb/options.h>

#include "storage.h"
#include "utils.h"

namespace raft {

Storage::Storage(const std::string& dir) {
    rocksdb::Options options;
    options.create_if_missing = true;
    rocksdb::Status status = rocksdb::DB::Open(
        options, combine_paths(dir, "raft.db"), &db);
    if (status.ok()) {
        PLOGI << "RocksDB instance created successfully.";
    } else {
        PLOGE << "Error opening RocksDB: " << status.ToString();
        throw std::runtime_error("Unable to open RocksDB");
    }
}

Storage::~Storage() {
    if (db) {
        rocksdb::Status status = db->Close();
        PLOGI << "closing rocksdb with status: " << status.ToString();
        delete db;
    }
}

bool Storage::save_state() {
    rocksdb::WriteBatch batch;
    batch.Put(RAFT_TERM_KEY, std::to_string(_term));
    batch.Put(RAFT_VOTED_FOR_KEY, _voted_for);
    rocksdb::Status status = db->Write(rocksdb::WriteOptions(), &batch);
    if (status.ok()) {
        PLOGI << "saved term " << _term << " and voted_for " << _voted_for
              << " to persistent storage.";
    } else {
        PLOGE << "error saving term " << _term << " and voted_for <"
              << _voted_for << "> to persistent storage. Status: "
              << status.ToString();
    }
    return status.ok();
}

void Storage::load_state() {
    // called only once, when node initially starts. Otherwise, term and vote
    // information is persisted to disk, HOWEVER, will also be concurrently
    // updated in memory.
    // will default to term 0 and voted_for "" when EITHER key does not exist.

    std::string term, voted_for;
    rocksdb::Status s1, s2;
    
    s1 = db->Get(rocksdb::ReadOptions(), RAFT_TERM_KEY, &term);
    s2 = db->Get(rocksdb::ReadOptions(), RAFT_VOTED_FOR_KEY, &voted_for);
    
    if (!s1.ok() || !s2.ok()) {
        term = "0";
        voted_for = "";
        PLOGW << "defaulting to term 0 and voted for <none>. s1: "
              << s1.ToString() << ", s2: " << s2.ToString();
    }
    
    _term = std::stoull(term);
    _voted_for = voted_for;
    PLOGI << "loaded term: " << std::to_string(_term) 
          << " and voted_for: <" << _voted_for << ">";
}

} // namespace raft
