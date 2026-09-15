#pragma once

#include <catch2/catch.hpp>
//#include <catch2/catch_test_macros.hpp>
//#include <catch2/matchers/catch_matchers_floating_point.hpp>

#define PRICE_TOLERANCE 0.001
#define PRICE_MATCHER(PRICE) Catch::Matchers::WithinAbs((PRICE), PRICE_TOLERANCE)

#define QUANTITY_TOLERANCE 0.0000001
#define QUANTITY_MATCHER(QUANTITY) Catch::Matchers::WithinAbs((QUANTITY), QUANTITY_TOLERANCE)
