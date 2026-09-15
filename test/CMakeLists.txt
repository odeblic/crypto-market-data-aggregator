include_directories(${CMAKE_SOURCE_DIR}/src)

add_executable(aggregator-tests Aggregator.cpp)
target_link_libraries(aggregator-tests PRIVATE Catch2::Catch2WithMain)

add_executable(best-bid-offer-tests BestBidOffer.cpp)
target_link_libraries(best-bid-offer-tests PRIVATE Catch2::Catch2WithMain)

add_executable(price-bands-tests PriceBands.cpp)
target_link_libraries(price-bands-tests PRIVATE Catch2::Catch2WithMain)

add_executable(notional-volume-bands-tests NotionalVolumeBands.cpp)
target_link_libraries(notional-volume-bands-tests PRIVATE Catch2::Catch2WithMain)

include(ParseAndAddCatchTests)
include(Catch)

catch_discover_tests(best-bid-offer-tests)
catch_discover_tests(price-bands-tests)
catch_discover_tests(notional-volume-bands-tests)
