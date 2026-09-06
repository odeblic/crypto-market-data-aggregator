# Add Boost as a dependency for Websockets

FetchContent_Declare(
    Boost
    URL https://github.com/boostorg/boost/releases/download/boost-1.84.0/boost-1.84.0.tar.gz
)

FetchContent_MakeAvailable(Boost)
