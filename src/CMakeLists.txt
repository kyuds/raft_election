# grpc compiler
include(proto/rpc.cmake)

add_executable(
    raft
    cli.cc
    callback.h
    config.h
    config.cc
    raft.h
    raft.cc
    rpc.h
    rpc.cc
    sqlcmd.h
    storage.h
    storage.cc
    tasks/electiontask.h
    tasks/electiontask.cc
    tasks/synctask.h
    tasks/synctask.cc
    utils.h
    utils.cc
)

target_include_directories(raft PUBLIC
    "${CMAKE_CURRENT_BINARY_DIR}"
)

target_link_libraries(raft
    PRIVATE
        plog
        sqlite3
        grpc_proto
)
