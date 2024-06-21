#ifndef RAFT_STATE
#define RAFT_STATE

#include <string>
#include "utils.h"

#define PSTATE_FILE "pstate.txt"

namespace raft {

class State {
    public:
        State(const std::string& _root)
            : root(_root),
              pstate_file_path(combine_paths(_root, PSTATE_FILE)) {}
        ~State() {}

        // setup
        bool initialize();

        // persistent state
        bool save_pstate();
        bool load_pstate();

        uint64_t term() { return _term; }
        const std::string& voted_for() { return _voted_for; }
        void set_term(const uint64_t t) { _term = t; }
        void set_voted_for(const std::string& v) { _voted_for = v; }

        // log

        // snapshot
    
    private:
        uint64_t _term;
        std::string _voted_for;

    private:
        const std::string root;
        const std::string pstate_file_path;
};

} // namespace raft

#endif // RAFT_STATE
