#ifndef RAFT_UTILS
#define RAFT_UTILS

#include <string>

namespace raft {

// Filesystem
std::string combine_paths(const std::string& p1, const std::string& p2);
void create_directories_if_needed(const std::string& path);

} // namespace raft

#endif // RAFT_UTILS
