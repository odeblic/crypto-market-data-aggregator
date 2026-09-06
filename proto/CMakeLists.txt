set(PROTO_FILE "${CMAKE_CURRENT_SOURCE_DIR}/marketdata.proto")
set(PROTO_SRCS "${CMAKE_CURRENT_BINARY_DIR}/marketdata.pb.cc")
set(PROTO_HDRS "${CMAKE_CURRENT_BINARY_DIR}/marketdata.pb.h")
set(GRPC_SRCS "${CMAKE_CURRENT_BINARY_DIR}/marketdata.grpc.pb.cc")
set(GRPC_HDRS "${CMAKE_CURRENT_BINARY_DIR}/marketdata.grpc.pb.h")

add_custom_command(
    OUTPUT ${PROTO_SRCS} ${PROTO_HDRS} ${GRPC_SRCS} ${GRPC_HDRS}
    COMMAND $<TARGET_FILE:protobuf::protoc>
    ARGS --grpc_out ${CMAKE_CURRENT_BINARY_DIR}
         --cpp_out ${CMAKE_CURRENT_BINARY_DIR}
         --plugin=protoc-gen-grpc=$<TARGET_FILE:grpc_cpp_plugin>
         -I "${CMAKE_CURRENT_SOURCE_DIR}"
         "${PROTO_FILE}"
    DEPENDS "${PROTO_FILE}" protobuf::protoc grpc_cpp_plugin
    VERBATIM
)

add_library(marketdata_lib ${PROTO_SRCS} ${GRPC_SRCS})
target_link_libraries(marketdata_lib PUBLIC gRPC::grpc++ protobuf::libprotobuf)
target_include_directories(marketdata_lib PUBLIC "${CMAKE_CURRENT_BINARY_DIR}")
