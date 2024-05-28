# The following cmake file will generate code based on
# proto files under src/generated. All files are created
# under the single directory.

find_package(protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)
find_package(Threads REQUIRED)

# proto files to compile (no extensions)
set(PROTO_SERVICES
    test
    test2
    # add more proto file names (eg: test.proto -> test)
)

# gRPC configurations
set(_LIBPROTOBUF libprotobuf)
set(_GRPC_GRPC gRPC::grpc)
set(_GRPC_GRPCPP gRPC::grpc++)
set(_GRPC_REFLECTION gRPC::grpc++_reflection)

# finding compiler
get_target_property(
    _GRPC_CPP_PLUGIN_EXECUTABLE
    gRPC::grpc_cpp_plugin LOCATION
)
find_program(_PROTOC protoc)

# list of files generated (added dynamically)
set(PROTO_GEN_FILES "")

# iterate through all protobuf services and generate
# cpp files and grpc cpp files. 
foreach(_proto_service ${PROTO_SERVICES})
    add_custom_command(
        OUTPUT
            "${CMAKE_CURRENT_BINARY_DIR}/${_proto_service}.pb.h"
            "${CMAKE_CURRENT_BINARY_DIR}/${_proto_service}.pb.cc"
            "${CMAKE_CURRENT_BINARY_DIR}/${_proto_service}.grpc.pb.h"
            "${CMAKE_CURRENT_BINARY_DIR}/${_proto_service}.grpc.pb.cc"
        COMMAND ${_PROTOC}
        ARGS
            --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
            --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
            -I "${CMAKE_SOURCE_DIR}/src/proto"
            --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
            "${CMAKE_SOURCE_DIR}/src/proto/${_proto_service}.proto"
        DEPENDS "${CMAKE_SOURCE_DIR}/src/proto/${_proto_service}.proto")
    list(
        APPEND PROTO_GEN_FILES
            "${CMAKE_CURRENT_BINARY_DIR}/${_proto_service}.pb.h"
            "${CMAKE_CURRENT_BINARY_DIR}/${_proto_service}.pb.cc"
            "${CMAKE_CURRENT_BINARY_DIR}/${_proto_service}.grpc.pb.h"
            "${CMAKE_CURRENT_BINARY_DIR}/${_proto_service}.grpc.pb.cc"
    )
endforeach()

include_directories("${CMAKE_CURRENT_BINARY_DIR}")

add_library(grpc_proto
    ${PROTO_GEN_FILES}
)

target_link_libraries(grpc_proto
    ${_GRPC_GRPC}
    ${_GRPC_GRPCPP}
    ${_GRPC_REFLECTION}
    ${_PROTOBUF_LIBPROTOBUF}
)
