# TODO: add proto folder

add_executable(
    raft
    cli.cc
    config.h
    config.cc
)

target_include_directories(raft PUBLIC
    "${CMAKE_CURRENT_BINARY_DIR}"
)

target_link_libraries(raft
    PRIVATE
        plog
)
