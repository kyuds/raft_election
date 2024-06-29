#include <filesystem>
#include <fstream>
#include <plog/Log.h>

#include "state.h"
#include "utils.h"

namespace raft {

bool State::initialize() {
    try {
        if (std::filesystem::create_directories(root)) {
            PLOGI << "Created directory: " << root;
        } else {
            PLOGI << "Using preexisting directory: " << root;
        }
        if (!std::filesystem::exists(pstate_file_path)) {
            PLOGI << "Creating missing pstate file.";
            set_term(0);
            set_voted_for(std::string(""));
            if (!save_pstate()) {
                throw std::runtime_error("Could not create pstate file.");
            }
        } else {
            return load_pstate();
        }
        return true;
    } catch (const std::filesystem::filesystem_error& e) {
        PLOGE << "There was an error with a filesystem operation " << e.what();
    } catch (const std::runtime_error& e) {
        PLOGE << "There was an error with saving the state " << e.what();
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
bool State::save_pstate() {
    std::ofstream file(pstate_file_path);
    if (!file) {
        PLOGF << "Couldn't find a pstate file";
        return false;
    }
    file << _term << "\n" << _voted_for << "\n";
    file.flush();
    file.close();
    PLOGI << "Commited pstate: " << _term << " " << _voted_for;
    return true;
}

bool State::load_pstate() {
    std::ifstream file(pstate_file_path);
    std::string line;
    bool success = true;
    if (!file.is_open()) return false;
    if (std::getline(file, line)) {
        try {
            // TODO: this will convert negative numbers.
            _term = std::stoull(line);
        } catch (const std::invalid_argument& e) {
            PLOGE << "Invalid argument: " << e.what();
            success = false;
        } catch (const std::out_of_range& e) {
            PLOGE << "Out of range: " << e.what();
            success = false;
        }
        if (std::getline(file, line)) {
            _voted_for = line;
        } else {
            PLOGE << "Couldn't find 'voted_for' in state file";
            success = false;
        }
    } else {
        PLOGE << "Empty state file";
        success = false;
    }
    file.close();
    PLOGI << "Loaded pstate: " << _term << " " << _voted_for;
    return success;
}

} // namespace raft
