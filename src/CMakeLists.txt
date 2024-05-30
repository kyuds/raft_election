# generate proto files
include(cmake/proto.cmake)

add_executable(raft
    service.cc
    raft.hh
    raft.cc
    config.hh
    config.cc
)

target_include_directories(raft PUBLIC
    "${CMAKE_CURRENT_BINARY_DIR}"
)

target_link_libraries(raft
    grpc_proto
)
