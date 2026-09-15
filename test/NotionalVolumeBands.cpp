#include "MyCatch2.hpp"

#include "biz/NotionalVolumeBands.hpp"

TEST_CASE("notional volume bands are correctly calculated", "[business]")
{
    auto bandValues = std::vector<double>{1'000., 5'000., 10'000., 20'000., 50'000.};
    auto bands = NotionalVolumeBands{bandValues};

    auto const book = Book
    {
        .ask = {{55.0, 100}, {60.0, 200}},
        .bid = {{45.0, 100}, {40.0, 50}},
    };

    auto const result = bands.compute(book);

    REQUIRE(result.ask.size() == 3);
    REQUIRE(result.bid.size() == 2);

    CHECK_THAT(result.ask[0].price, PRICE_MATCHER(55.0));
    CHECK_THAT(result.ask[0].quantity, QUANTITY_MATCHER(1'000 / 55.0));
    CHECK_THAT(result.bid[0].price, PRICE_MATCHER(45.0));
    CHECK_THAT(result.bid[0].quantity, QUANTITY_MATCHER(1'000 / 45.0));

    CHECK_THAT(result.ask[1].price, PRICE_MATCHER(55.0));
    CHECK_THAT(result.ask[1].quantity, QUANTITY_MATCHER(5'000 / 55.0));
    CHECK_THAT(result.bid[1].price, PRICE_MATCHER((100 * 45.0 + 500) / (100 + 500 / 40.0)));
    CHECK_THAT(result.bid[1].quantity, QUANTITY_MATCHER(100 + 500 / 40.0));

    CHECK_THAT(result.ask[2].price, PRICE_MATCHER((100 * 55.0 + 4500) / (100 + 4500 / 60.0)));
    CHECK_THAT(result.ask[2].quantity, QUANTITY_MATCHER(100 + 4500 / 60.0));
}
