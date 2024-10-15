#ifndef RAFT_UTILS
#define RAFT_UTILS

#include <string>
#include <vector>

namespace raft {

// Filesystem
std::string combine_paths(const std::string& p1, const std::string& p2);
void create_directories_if_needed(const std::string& path);

// File
std::vector<std::string> get_lines(const std::string& f);

// vectors (for peers)
std::string vector_to_string(const std::vector<std::string>& vec);

} // namespace raft

#endif // RAFT_UTILS
