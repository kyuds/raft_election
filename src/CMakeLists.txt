# generate proto files
include(cmake/proto.cmake)

add_executable(raft
    cli.cc
    raft.h
    raft.cc
    rpc.h
    rpc.cc
    storage.h
    storage.cc
    config.h
    config.cc
    utils.h
    utils.cc
)

target_include_directories(raft PUBLIC
    "${CMAKE_CURRENT_BINARY_DIR}"
)

# include json parser for ConfigParser
FetchContent_Declare(json URL ${JSON_LIB})
FetchContent_MakeAvailable(json)

target_link_libraries(raft
    PUBLIC
        grpc_proto
    PRIVATE
        ${JSON_LIB_LINK}
)
