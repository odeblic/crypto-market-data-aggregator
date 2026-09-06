# Add Catch2 as a dependency for unit tests

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.15.2
)

FetchContent_MakeAvailable(Catch2)
