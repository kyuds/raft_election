#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <plog/Log.h>

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

std::vector<std::string> get_lines(const std::string& file) {
    std::vector<std::string> lines;
    std::ifstream f(file);
    std::string line;

    if (!f.is_open())
        throw std::runtime_error("Peer file not found.");

    while(std::getline(f, line)) {
        if (line.length() != 0)
            lines.push_back(std::string(line));
    }
    return lines;
}

std::string vector_to_string(const std::vector<std::string>& vec) {
    std::ostringstream oss;
    oss << "[ ";
    for (const auto& str : vec) {
        oss << str << " ";
    }
    oss << "]";
    return oss.str();
}

bool serialize_log(const LogEntry& log, std::string* output) {
    bool ret = log.SerializeToString(output);
    if (!ret) {
        PLOGE << "Failed to serialize log entry.";
    }
    return ret;
}

bool deserialize_log(const std::string& input, LogEntry* log) {
    bool ret = log->ParseFromString(input);
    if (!ret) {
        PLOGE << "Failed to deserialize log entry: " << input << ".";
    }
    return ret;
}

} // namespace raft
