cmake_minimum_required(VERSION 3.7)
project(grpc_test)

set(CMAKE_CXX_STANDARD 14)

include(${CMAKE_SOURCE_DIR}/cmake/GRPCHelper.cmake)

find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)

include_directories(${Protobuf_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

pb_grpc_generate_cpp(PROTO_SRCS PROTO_HDRS GRPC_SRCS GRPC_HDRS ${CMAKE_SOURCE_DIR}/protos/simple.proto)

add_library(grpc_proto ${PROTO_SRCS} ${PROTO_HDRS} ${GRPC_SRCS} ${GRPC_HDRS})
target_link_libraries(grpc_proto gRPC::grpc++ gRPC::grpc++_reflection protobuf::libprotobuf)

foreach (_target grpc_server grpc_client)
    add_executable(${_target} "${_target}.cpp")
    target_link_libraries(${_target} grpc_proto)
    install(TARGETS ${_target} RUNTIME DESTINATION bin)
endforeach ()
