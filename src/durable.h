#ifndef RAFT_DURABLE
#define RAFT_DURABLE

#include <string>

#include "common.h"
#include "utils.h"

namespace raft {

class Durable {
    public:
        Durable(const std::string& _root)
            : root(_root),
              pstate_file_path(combine_paths(_root, RAFT_PSTATE_FILE)) {}
        ~Durable() {}

        // setup
        bool initialize();

        // pstate
        bool save_pstate(const pstate_t& pstate);
        bool load_pstate(pstate_t& pstate);

        // log

        // snapshot

    private:
        const std::string root;
        const std::string pstate_file_path;
};

} // namespace raft

#endif // RAFT_DURABLE
