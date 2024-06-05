#ifndef RAFT_UTILS
#define RAFT_UTILS

#include <string>
#include <vector>

namespace raft {

// Filesystem
std::string combine_paths(const std::string& p1, const std::string& p2);
std::string combine_paths(const std::string& p1, const char * p2);
std::vector<std::string> file_line_to_vec(const std::string& file);

} // namespace raft

#endif // RAFT_UTILS
