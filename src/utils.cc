#include <filesystem>

#include "utils.h"

namespace raft {

std::string combine_paths(const std::string& p1, const std::string& p2) {
    std::filesystem::path fp1(p1);
    std::filesystem::path fp2(p2);
    return (fp1 / fp2).string();
}

void create_directories_if_needed(const std::string& path) {
    std::filesystem::create_directories(path);
}

} // namespace raft
