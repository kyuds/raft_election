#include <iostream>

#include "config.h"

using Config = raft::Config;

void test_config(int argc, char ** argv);

int main(int argc, char ** argv) {

    test_config(argc, argv);

    return 0;
}

///

void test_config(int argc, char ** argv) {
    Config * config = Config::default_config(argv[1], argv[2], argv[3], argv[4]);
    std::cout << config->info();
}
