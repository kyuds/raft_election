# generate proto files
include(cmake/proto.cmake)

add_executable(
    client
    client.cc
)

target_include_directories(client PUBLIC
    "${CMAKE_CURRENT_BINARY_DIR}"
)

target_link_libraries(client
    grpc_proto
)

add_executable(
    server
    server.cc
)

target_include_directories(server PUBLIC
    "${CMAKE_CURRENT_BINARY_DIR}"
)

target_link_libraries(server
    grpc_proto
)

add_executable(
    raft
    service.cc
    raft.hh
    raft.cc
    config.hh
    config.cc
)

target_include_directories(server PUBLIC
    "${CMAKE_CURRENT_BINARY_DIR}"
)
