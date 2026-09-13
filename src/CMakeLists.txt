include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_compile_options(-Wall -Wextra -Wfatal-errors -fmax-errors=1)

add_executable(internal-exchange internal-exchange.cpp)
target_link_libraries(internal-exchange PRIVATE
    nlohmann_json::nlohmann_json
    Boost::headers
    OpenSSL::SSL
    OpenSSL::Crypto
)

add_executable(aggregator aggregator.cpp)
target_link_libraries(aggregator PRIVATE
    nlohmann_json::nlohmann_json
    Boost::headers
    OpenSSL::SSL
    OpenSSL::Crypto
    marketdata_lib
)

add_executable(best-bid-offer best-bid-offer.cpp)
target_link_libraries(best-bid-offer PRIVATE
    nlohmann_json::nlohmann_json
    marketdata_lib
)

add_executable(notional-volume-bands notional-volume-bands.cpp)
target_link_libraries(notional-volume-bands PRIVATE
    nlohmann_json::nlohmann_json
    marketdata_lib
)

add_executable(price-bands price-bands.cpp)
target_link_libraries(price-bands PRIVATE
    nlohmann_json::nlohmann_json
    marketdata_lib
)

add_executable(viewer viewer.cpp)
target_link_libraries(viewer PRIVATE
    nlohmann_json::nlohmann_json
    Boost::headers
    OpenSSL::SSL
    OpenSSL::Crypto
)
