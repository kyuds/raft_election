#include <iostream>

#include "raft.h"
#include "utils.h"

namespace raft {

Raft::Raft(std::string _name, 
            std::string _address, 
            const std::string& conf_file,
            const std::string& member_file) {
    name = _name;
    address = _address;
    conf = new Config(name, conf_file);
    members = file_line_to_vec(member_file);

    std::cout << conf->get_storage_dir() << std::endl;
    for (auto const& m : members) {
        std::cout << "<" << m << ">" << std::endl;
    }
}

} // namespace raft
