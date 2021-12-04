
set(TARGET reflection_client)
add_executable(${TARGET} "${TARGET}.cpp")
target_link_libraries(${TARGET} grpc_proto)
install(TARGETS ${TARGET} RUNTIME DESTINATION bin)