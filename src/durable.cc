#include <filesystem>
#include <fstream>
#include <iostream>

#include "durable.h"
#include "utils.h"

// (TODO: kyuds) create logging module for errors. 

namespace raft {

bool Durable::initialize() {
    try {
        if (std::filesystem::create_directories(root)) {
            std::cout << "Created directory " << root << std::endl;
        } else {
            std::cout << "Using preexisting directory " << root << std::endl;
        }
        if (!std::filesystem::exists(pstate_file_path)) {
            std::cout << "Creating missing pstate file." << std::endl;
            if (!save_pstate(pstate_t {PSTATE_TM_INIT, PSTATE_VF_INIT})) {
                throw std::runtime_error("Could not create pstate file.");
            }
        }
        return true;
    } catch (const std::filesystem::filesystem_error& e) {
        std::cout << "filesystem gg" << std::endl;
    } catch (const std::runtime_error& e) {
        std::cout << "pstate file gg" << std::endl;
    }
    return false;
}

// structure of a pstate file:
// in raft, there are three persistent states: current term, voted candidate,
// and the log. Unlike the log, the current term and voted candidate is bound
// in size, so we store that in a single pstate file.
// file contents:
// 0                --> 1st line
// localhost::10001 --> 2nd line (empty if null)
bool Durable::save_pstate(const pstate_t& pstate) {
    std::ofstream file(pstate_file_path);
    if (!file) {
        return false;
    }
    file << pstate.term << "\n" << pstate.voted_for << "\n";
    file.flush();
    file.close();
    return true;
}

bool Durable::load_pstate(pstate_t& pstate) {
    std::ifstream file(pstate_file_path);
    std::string line;
    bool success = true;
    if (!file.is_open()) return false;
    if (std::getline(file, line)) {
        try {
            pstate.term = std::stoull(line);
        } catch (const std::invalid_argument& e) {
            std::cerr << "Invalid argument: " << e.what() << '\n';
            success = false;
        } catch (const std::out_of_range& e) {
            std::cerr << "Out of range: " << e.what() << '\n';
            success = false;
        }
        if (std::getline(file, line)) {
            pstate.voted_for = line;
        } else {
            success = false;
        }
    } else {
        success = false;
    }
    file.close();
    return success;
}

} // namespace raft
