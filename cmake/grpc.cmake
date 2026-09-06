# Disable gRPC tests and unneeded components to speed up build times

set(gRPC_BUILD_TESTS OFF CACHE INTERNAL "")
set(gRPC_BUILD_CODEGEN ON CACHE INTERNAL "")
set(gRPC_INSTALL OFF CACHE INTERNAL "")
set(protobuf_INSTALL OFF CACHE INTERNAL "")
set(utf8_range_INSTALL OFF CACHE INTERNAL "")
set(utf8_range_ENABLE_INSTALL OFF CACHE INTERNAL "")

set(ABSL_PROPAGATE_CXX_STD ON CACHE INTERNAL "")

# Add gRPC as a dependency for RPC services

FetchContent_Declare(
  gRPC
  GIT_REPOSITORY https://github.com/grpc/grpc
  GIT_TAG        v1.83.1
)

FetchContent_MakeAvailable(gRPC)

# Normalize namespaces

if(TARGET grpc++ AND NOT TARGET gRPC::grpc++)
    add_library(gRPC::grpc++ ALIAS grpc++)
endif()

if(TARGET libprotobuf AND NOT TARGET protobuf::libprotobuf)
    add_library(protobuf::libprotobuf ALIAS libprotobuf)
endif()
