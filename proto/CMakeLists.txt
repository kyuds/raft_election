# The following cmake file will generate code based on
# proto files under src/generated. All files are created
# under the single directory.

find_package(protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)

set(
    PROTO_FILES
    test.proto
    # add more proto files here.
)

add_library(proto_code ${PROTO_FILES})
target_link_libraries(
    proto_code
    PUBLIC
        protobuf::libprotobuf
        gRPC::grpc
        gRPC::grpc++
)

target_include_directories(
    proto_code
    PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR}
)

file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/src/generated)

protobuf_generate(
    TARGET
        proto_code
    LANGUAGE
        cpp
    PROTOC_OUT_DIR
        "${CMAKE_SOURCE_DIR}/src/generated"
)

# get_target_property(
#     grpc_cpp_plugin_location
#     gRPC::grpc_cpp_plugin LOCATION
# )

# protobuf_generate(
#     TARGET
#         proto_code
#     LANGUAGE
#         grpc
#     GENERATE_EXTENSIONS
#         .grpc.pb.h
#         .grpc.pb.cc
#     PLUGIN 
#         "protoc-gen-grpc=${grpc_cpp_plugin_location}"
# )
