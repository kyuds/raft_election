#include <fstream>
#include <filesystem>

#include "utils.h"

namespace raft {

std::string combine_paths(const std::string& p1, const std::string& p2) {
    std::filesystem::path fp1(p1);
    std::filesystem::path fp2(p2);
    return (fp1 / fp2).string();
}

std::string combine_paths(const std::string& p1, const char * p2) {
    std::filesystem::path fp1(p1);
    std::filesystem::path fp2(p2);
    return (fp1 / fp2).string();
}

std::vector<std::string> file_line_to_vec(const std::string& file) {
    std::vector<std::string> lines;
    std::ifstream f(file);
    std::string line;

    if (!f.is_open()) {
        // (TODO: kyuds) error here.
    }

    while(std::getline(f, line)) {
        if (line.length() != 0)
            lines.push_back(std::string(line));
    }

    return lines;
}

} // namespace raft
