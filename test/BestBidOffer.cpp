#include "MyCatch2.hpp"

#include "biz/BestBidOffer.hpp"

TEST_CASE("top bid and offer are correctly extracted", "[business]")
{
    SECTION("for a multi-level two-side book")
    {
        auto const book = Book
        {
            .ask = {{101.0, 15}, {102.0, 25}},
            .bid = {{100.0, 10}, {99.0, 20}, {98.0, 30}},
        };

        auto const result = computeBestBidOffer(book);

        REQUIRE(result.ask.size() == 1);
        REQUIRE(result.bid.size() == 1);

        CHECK_THAT(result.ask[0].price, PRICE_MATCHER(101.0));
        CHECK_THAT(result.ask[0].quantity, QUANTITY_MATCHER(15));
        CHECK_THAT(result.bid[0].price, PRICE_MATCHER(100.0));
        CHECK_THAT(result.bid[0].quantity, QUANTITY_MATCHER(10));
    }

    SECTION("for a single-level two-side book")
    {
        auto const book = Book
        {
            .ask = {{51.0, 250}},
            .bid = {{50.0, 350}},
        };

        auto const result = computeBestBidOffer(book);

        REQUIRE(result.ask.size() == 1);
        REQUIRE(result.bid.size() == 1);

        CHECK_THAT(result.ask[0].price, PRICE_MATCHER(51.0));
        CHECK_THAT(result.ask[0].quantity, QUANTITY_MATCHER(250));
        CHECK_THAT(result.bid[0].price, PRICE_MATCHER(50.0));
        CHECK_THAT(result.bid[0].quantity, QUANTITY_MATCHER(350));
    }

    SECTION("for a single-level ask-side only book")
    {
        auto const book = Book
        {
            .ask = {{88.0, 1000}, {99.0, 2000}},
            .bid = {},
        };

        auto const result = computeBestBidOffer(book);

        REQUIRE(result.ask.size() == 1);
        REQUIRE(result.bid.size() == 0);

        CHECK_THAT(result.ask[0].price, PRICE_MATCHER(88.0));
        CHECK_THAT(result.ask[0].quantity, QUANTITY_MATCHER(1000));
    }

    SECTION("for a single-level bid-side only book")
    {
        auto const book = Book
        {
            .ask = {},
            .bid = {{88.0, 1000}, {77.0, 2000}},
        };

        auto const result = computeBestBidOffer(book);

        REQUIRE(result.ask.size() == 0);
        REQUIRE(result.bid.size() == 1);

        CHECK_THAT(result.bid[0].price, PRICE_MATCHER(88.0));
        CHECK_THAT(result.bid[0].quantity, QUANTITY_MATCHER(1000));
    }

    SECTION("for an empty book")
    {
        auto const book = Book
        {
        };

        auto const result = computeBestBidOffer(book);

        REQUIRE(result.ask.size() == 0);
        REQUIRE(result.bid.size() == 0);
    }
}
