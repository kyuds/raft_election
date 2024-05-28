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
#    ${_GRPC_GRPC}
#    ${_GRPC_GRPCPP}
#    ${_GRPC_REFLECTION}
#    ${_PROTOBUF_LIBPROTOBUF}
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
#    ${_GRPC_GRPC}
#    ${_GRPC_GRPCPP}
#    ${_GRPC_REFLECTION}
#    ${_PROTOBUF_LIBPROTOBUF}
)
