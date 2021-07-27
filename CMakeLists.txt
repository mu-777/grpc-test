cmake_minimum_required(VERSION 3.7)
project(grpc_test)

set(CMAKE_CXX_STANDARD 14)

find_package(gRPC CONFIG REQUIRED)
find_package(Protobuf CONFIG REQUIRED)

include_directories(${Protobuf_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(_GRPC_GRPCPP gRPC::grpc++)
set(_PROTOBUF_PROTOC $<TARGET_FILE:protobuf::protoc>)
set(_PROTOBUF_LIBPROTOBUF protobuf::libprotobuf)
set(_REFLECTION gRPC::grpc++_reflection)
set(_GRPC_CPP_PLUGIN_EXE $<TARGET_FILE:gRPC::grpc_cpp_plugin>)

get_filename_component(PROTOFILE "protos/simple.proto" ABSOLUTE)
get_filename_component(PROTOFILE_DIR "${PROTOFILE}" DIRECTORY)

# CMAKE_CURRENT_BINARY_DIR is only acceptable for the output directory
# https://stackoverflow.com/questions/32715225/cmakelist-to-generate-cc-and-h-files-from-proto-file-under-a-specific-folder/32721897
set(PROTO_SRCS "${CMAKE_CURRENT_BINARY_DIR}/simple.pb.cc")
set(PROTO_HDRS "${CMAKE_CURRENT_BINARY_DIR}/simple.pb.h")
set(GRPC_SRCS "${CMAKE_CURRENT_BINARY_DIR}/simple.grpc.pb.cc")
set(GRPC_HDRS "${CMAKE_CURRENT_BINARY_DIR}/simple.grpc.pb.h")
add_custom_command(
        OUTPUT "${PROTO_SRCS}" "${PROTO_HDRS}" "${GRPC_SRCS}" "${GRPC_HDRS}"
        COMMAND ${_PROTOBUF_PROTOC}
        ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}" --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${PROTOFILE_DIR}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXE}"
        "${PROTOFILE}"
        DEPENDS "${PROTOFILE}")

# https://www.programmersought.com/article/412335673/
# grpc_generate_cpp(GRPC_SRCS GRPC_HDRS ${PROTO_SRC_DIR} ${hw_proto})
# protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTOFILE})


add_library(GRPC_PROTO ${PROTO_SRCS} ${PROTO_HDRS} ${GRPC_SRCS} ${GRPC_HDRS})
target_link_libraries(GRPC_PROTO ${_REFLECTION} ${_GRPC_GRPCPP} ${_PROTOBUF_LIBPROTOBUF})

foreach (_target grpc_server grpc_client)
    add_executable(${_target} "${_target}.cpp")
    target_link_libraries(${_target}
            GRPC_PROTO
            ${_REFLECTION}
            ${_GRPC_GRPCPP}
            ${_PROTOBUF_LIBPROTOBUF})
    install(TARGETS ${_target} DESTINATION ${CMAKE_CACHEFILE_DIR}/install)
endforeach ()
