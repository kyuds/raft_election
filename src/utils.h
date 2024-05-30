#ifndef RAFT_UTILS
#define RAFT_UTILS

#include <string>
#include <vector>

std::string combine_paths(const std::string& p1, const std::string& p2);
std::vector<std::string> file_line_to_vec(const std::string& file);

#endif // RAFT_UTILS
