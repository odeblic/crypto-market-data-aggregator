#include "MyCatch2.hpp"

#include "biz/Statistics.hpp"

TEST_CASE("statistics are correctly calculated", "[business]")
{
    auto statistics = Statistics{};

    SECTION("for a multi-level two-side book")
    {
        auto const book = Book
        {
            .ask = {{101.0, 15}, {102.0, 25}},
            .bid = {{100.0, 10}, {99.0, 20}, {98.0, 30}},
        };

        auto const result = statistics.compute(book);

        REQUIRE(result.currentAskPrice);
        CHECK_THAT(result.currentAskPrice.value(), PRICE_MATCHER(101.0));

        REQUIRE(result.currentBidPrice);
        CHECK_THAT(result.currentBidPrice.value(), PRICE_MATCHER(100.0));

        REQUIRE(result.spread);
        CHECK_THAT(result.spread.value(), PRICE_MATCHER(1.0));

        REQUIRE(result.crossedBook);
        CHECK(!result.crossedBook.value());

        REQUIRE(result.lowestAskPrice);
        CHECK_THAT(result.lowestAskPrice.value(), PRICE_MATCHER(101.0));

        REQUIRE(result.highestBidPrice);
        CHECK_THAT(result.highestBidPrice.value(), PRICE_MATCHER(100.0));

        CHECK(result.askLevelCount == 2);
        CHECK(result.bidLevelCount == 3);
    }

    SECTION("for an empty book")
    {
        auto const book = Book
        {
        };

        auto const result = statistics.compute(book);

        CHECK(!result.currentAskPrice);
        CHECK(!result.currentBidPrice);
        CHECK(!result.spread);
        CHECK(!result.crossedBook);
        CHECK(!result.lowestAskPrice);
        CHECK(!result.highestBidPrice);
        CHECK(result.askLevelCount == 0);
        CHECK(result.bidLevelCount == 0);
    }
}
