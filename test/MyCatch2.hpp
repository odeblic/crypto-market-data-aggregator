#pragma once

#if __has_include(<catch2/catch_test_macros.hpp>)
    // Catch2 v3+ (Modern, fast compilation header)
    #include <catch2/catch_test_macros.hpp>
    #include <catch2/matchers/catch_matchers_floating_point.hpp>
    //#include <catch2/matchers/catch_matchers_all.hpp>
#elif __has_include(<catch2/catch.hpp>)
    // Catch2 v2 (Legacy monolithic header)
    #include <catch2/catch.hpp>
#else
    #error "Catch2 header not found!"
#endif

#define PRICE_TOLERANCE 0.001
#define PRICE_MATCHER(PRICE) Catch::Matchers::WithinAbs((PRICE), PRICE_TOLERANCE)

#define QUANTITY_TOLERANCE 0.0000001
#define QUANTITY_MATCHER(QUANTITY) Catch::Matchers::WithinAbs((QUANTITY), QUANTITY_TOLERANCE)
