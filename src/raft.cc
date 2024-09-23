#include <iostream>

#include <plog/Log.h>
#include "plog/Initializers/RollingFileInitializer.h"

#include "raft.h"
#include "storage.h"

namespace raft {

Raft::Raft(Config * config)
    : name(config->get_name())
    , address(config->get_address())
    , min_election_timeout(config->get_min_election_timeout())
    , max_election_timeout(config->get_max_election_timeout())
    , heartbeat(config->get_heartbeat())
{
    create_directories_if_needed(config->get_storage_dir());
    plog::init(plog::info, 
               combine_paths(config->get_storage_dir(), "log.txt").c_str(), 
               1000000, 5);
    storage = std::make_unique<Storage>(config->get_storage_dir());

    std::cout << storage->term() << std::endl;
    storage->inc_term();
    storage->save_state();

    PLOGI << "Initialized raft node with config:\n" << config->info();
}

} // namespace raft
