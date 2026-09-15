#include "MyCatch2.hpp"

#include "biz/PriceBands.hpp"

TEST_CASE("price bands are correctly calculated", "[business]")
{
    auto bandValues = std::vector<int>{100, 500, 1'000, 2'000};
    auto bands = PriceBands{bandValues};

    SECTION("for a multi-level two-side book")
    {
        auto const book = Book
        {
            .ask = {{55.0, 100}, {60.0, 200}},
            .bid = {{45.0, 100}, {40.0, 50}},
        };

        auto const result = bands.compute(book);

        REQUIRE(result.ask.size() == 4);
        REQUIRE(result.bid.size() == 4);

        CHECK_THAT(result.ask[0].price, PRICE_MATCHER(55.55));
        CHECK_THAT(result.ask[0].quantity, QUANTITY_MATCHER(100));
        CHECK_THAT(result.bid[0].price, PRICE_MATCHER(44.55));
        CHECK_THAT(result.bid[0].quantity, QUANTITY_MATCHER(100));

        CHECK_THAT(result.ask[1].price, PRICE_MATCHER(57.75));
        CHECK_THAT(result.ask[1].quantity, QUANTITY_MATCHER(100));
        CHECK_THAT(result.bid[1].price, PRICE_MATCHER(42.75));
        CHECK_THAT(result.bid[1].quantity, QUANTITY_MATCHER(100));

        CHECK_THAT(result.ask[2].price, PRICE_MATCHER(60.50));
        CHECK_THAT(result.ask[2].quantity, QUANTITY_MATCHER(300));
        CHECK_THAT(result.bid[2].price, PRICE_MATCHER(40.50));
        CHECK_THAT(result.bid[2].quantity, QUANTITY_MATCHER(100));

        CHECK_THAT(result.ask[3].price, PRICE_MATCHER(66.00));
        CHECK_THAT(result.ask[3].quantity, QUANTITY_MATCHER(300));
        CHECK_THAT(result.bid[3].price, PRICE_MATCHER(36.00));
        CHECK_THAT(result.bid[3].quantity, QUANTITY_MATCHER(150));
    }

    SECTION("for a multi-level ask-side only book")
    {
        auto const book = Book
        {
            .ask = {{55.0, 500}, {60.0, 500}},
            .bid = {},
        };

        auto const result = bands.compute(book);

        REQUIRE(result.ask.size() == 4);
        REQUIRE(result.bid.size() == 0);

        CHECK_THAT(result.ask[0].price, PRICE_MATCHER(55.55));
        CHECK_THAT(result.ask[0].quantity, QUANTITY_MATCHER(500));

        CHECK_THAT(result.ask[1].price, PRICE_MATCHER(57.75));
        CHECK_THAT(result.ask[1].quantity, QUANTITY_MATCHER(500));

        CHECK_THAT(result.ask[2].price, PRICE_MATCHER(60.50));
        CHECK_THAT(result.ask[2].quantity, QUANTITY_MATCHER(1000));

        CHECK_THAT(result.ask[3].price, PRICE_MATCHER(66.00));
        CHECK_THAT(result.ask[3].quantity, QUANTITY_MATCHER(1000));
    }

    SECTION("for a multi-level bid-side only book")
    {
        auto const book = Book
        {
            .ask = {},
            .bid = {{45.0, 500}, {40.0, 500}},
        };

        auto const result = bands.compute(book);

        REQUIRE(result.ask.size() == 0);
        REQUIRE(result.bid.size() == 4);

        CHECK_THAT(result.bid[0].price, PRICE_MATCHER(44.55));
        CHECK_THAT(result.bid[0].quantity, QUANTITY_MATCHER(500));

        CHECK_THAT(result.bid[1].price, PRICE_MATCHER(42.75));
        CHECK_THAT(result.bid[1].quantity, QUANTITY_MATCHER(500));

        CHECK_THAT(result.bid[2].price, PRICE_MATCHER(40.50));
        CHECK_THAT(result.bid[2].quantity, QUANTITY_MATCHER(500));

        CHECK_THAT(result.bid[3].price, PRICE_MATCHER(36.00));
        CHECK_THAT(result.bid[3].quantity, QUANTITY_MATCHER(1000));
    }

    SECTION("for a single-level ask-side only book")
    {
        auto const book = Book
        {
            .ask = {{55.0, 500}},
            .bid = {},
        };

        auto const result = bands.compute(book);

        REQUIRE(result.ask.size() == 4);
        REQUIRE(result.bid.size() == 0);

        CHECK_THAT(result.ask[0].price, PRICE_MATCHER(55.55));
        CHECK_THAT(result.ask[0].quantity, QUANTITY_MATCHER(500));

        CHECK_THAT(result.ask[1].price, PRICE_MATCHER(57.75));
        CHECK_THAT(result.ask[1].quantity, QUANTITY_MATCHER(500));

        CHECK_THAT(result.ask[2].price, PRICE_MATCHER(60.50));
        CHECK_THAT(result.ask[2].quantity, QUANTITY_MATCHER(500));

        CHECK_THAT(result.ask[3].price, PRICE_MATCHER(66.00));
        CHECK_THAT(result.ask[3].quantity, QUANTITY_MATCHER(500));
    }

    SECTION("for a single-level bid-side only book")
    {
        auto const book = Book
        {
            .ask = {},
            .bid = {{45.0, 500}},
        };

        auto const result = bands.compute(book);

        REQUIRE(result.ask.size() == 0);
        REQUIRE(result.bid.size() == 4);

        CHECK_THAT(result.bid[0].price, PRICE_MATCHER(44.55));
        CHECK_THAT(result.bid[0].quantity, QUANTITY_MATCHER(500));

        CHECK_THAT(result.bid[1].price, PRICE_MATCHER(42.75));
        CHECK_THAT(result.bid[1].quantity, QUANTITY_MATCHER(500));

        CHECK_THAT(result.bid[2].price, PRICE_MATCHER(40.50));
        CHECK_THAT(result.bid[2].quantity, QUANTITY_MATCHER(500));

        CHECK_THAT(result.bid[3].price, PRICE_MATCHER(36.00));
        CHECK_THAT(result.bid[3].quantity, QUANTITY_MATCHER(500));
    }

    SECTION("for an empty book")
    {
        auto const book = Book
        {
            .ask = {},
            .bid = {},
        };

        auto const result = bands.compute(book);

        REQUIRE(result.ask.size() == 0);
        REQUIRE(result.bid.size() == 0);
    }
}
