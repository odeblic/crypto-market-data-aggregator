add_executable(unit_tests Test.cpp)
target_link_libraries(unit_tests PRIVATE Catch2::Catch2WithMain)

include(ParseAndAddCatchTests)
include(Catch)

catch_discover_tests(unit_tests)

set_target_properties(unit_tests PROPERTIES OUTPUT_NAME "unit-tests")
