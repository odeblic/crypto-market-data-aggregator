#include "MyCatch2.hpp"

#include "biz/Aggregator.hpp"

TEST_CASE("aggregate correctly market updates from different exchanges to build a consolidated book", "[business]")
{
    auto aggregator = Aggregator{};

    auto sendMarketUpdate = [&aggregator](Exchange exchange, Side side, double price, double quantity)
    {
        auto update = MarketUpdate
        {
            .price = price,
            .quantity = quantity,
            .ticker = "",
            .side = side,
            .exchange = exchange,
        };

        aggregator.onUpdate(update);
    };

    SECTION("for no market updates")
    {
        auto book = aggregator.generateBook();

        REQUIRE(book.ask.size() == 0);
        REQUIRE(book.bid.size() == 0);
    }

    SECTION("for one market update")
    {
        SECTION("on ask side")
        {
            sendMarketUpdate(Exchange::OKX, Side::ASK, 4500.0, 0.28);
            auto book = aggregator.generateBook();

            REQUIRE(book.ask.size() == 1);
            REQUIRE(book.bid.size() == 0);

            CHECK_THAT(book.ask[0].price, PRICE_MATCHER(4500.0));
            CHECK_THAT(book.ask[0].quantity, QUANTITY_MATCHER(0.28));
        }

        SECTION("on bid side")
        {
            sendMarketUpdate(Exchange::KRAKEN, Side::BID, 5300.0, 0.71);
            auto book = aggregator.generateBook();

            REQUIRE(book.ask.size() == 0);
            REQUIRE(book.bid.size() == 1);

            CHECK_THAT(book.bid[0].price, PRICE_MATCHER(5300.0));
            CHECK_THAT(book.bid[0].quantity, QUANTITY_MATCHER(0.71));
        }
    }

    SECTION("for two market updates")
    {
        SECTION("on ask side")
        {
            SECTION("at the same price")
            {
                SECTION("from the same exchange")
                {
                    sendMarketUpdate(Exchange::BINANCE, Side::ASK, 780.0, 1.25);
                    sendMarketUpdate(Exchange::BINANCE, Side::ASK, 780.0, 1.75);
                    auto book = aggregator.generateBook();

                    REQUIRE(book.ask.size() == 1);
                    REQUIRE(book.bid.size() == 0);

                    CHECK_THAT(book.ask[0].price, PRICE_MATCHER(780.0));
                    CHECK_THAT(book.ask[0].quantity, QUANTITY_MATCHER(1.75));
                }

                SECTION("from different exchanges")
                {
                    sendMarketUpdate(Exchange::BINANCE, Side::ASK, 780.0, 1.25);
                    sendMarketUpdate(Exchange::BYBIT, Side::ASK, 780.0, 1.75);
                    auto book = aggregator.generateBook();

                    REQUIRE(book.ask.size() == 1);
                    REQUIRE(book.bid.size() == 0);

                    CHECK_THAT(book.ask[0].price, PRICE_MATCHER(780.0));
                    CHECK_THAT(book.ask[0].quantity, QUANTITY_MATCHER(3.00));
                }
            }

            SECTION("at different prices")
            {
                SECTION("from the same exchange")
                {
                    sendMarketUpdate(Exchange::BINANCE, Side::ASK, 780.0, 2.25);
                    sendMarketUpdate(Exchange::BINANCE, Side::ASK, 790.0, 2.75);
                    auto book = aggregator.generateBook();

                    REQUIRE(book.ask.size() == 2);
                    REQUIRE(book.bid.size() == 0);

                    CHECK_THAT(book.ask[0].price, PRICE_MATCHER(780.0));
                    CHECK_THAT(book.ask[0].quantity, QUANTITY_MATCHER(2.25));

                    CHECK_THAT(book.ask[1].price, PRICE_MATCHER(790.0));
                    CHECK_THAT(book.ask[1].quantity, QUANTITY_MATCHER(2.75));
                }

                SECTION("from different exchanges")
                {
                    sendMarketUpdate(Exchange::BINANCE, Side::ASK, 780.0, 2.25);
                    sendMarketUpdate(Exchange::BYBIT, Side::ASK, 790.0, 2.75);
                    auto book = aggregator.generateBook();

                    REQUIRE(book.ask.size() == 2);
                    REQUIRE(book.bid.size() == 0);

                    CHECK_THAT(book.ask[0].price, PRICE_MATCHER(780.0));
                    CHECK_THAT(book.ask[0].quantity, QUANTITY_MATCHER(2.25));

                    CHECK_THAT(book.ask[1].price, PRICE_MATCHER(790.0));
                    CHECK_THAT(book.ask[1].quantity, QUANTITY_MATCHER(2.75));
                }
            }
        }

        SECTION("on bid side")
        {
            SECTION("at the same price")
            {
                SECTION("from the same exchange")
                {
                    sendMarketUpdate(Exchange::BINANCE, Side::BID, 830.0, 1.25);
                    sendMarketUpdate(Exchange::BINANCE, Side::BID, 830.0, 1.75);
                    auto book = aggregator.generateBook();

                    REQUIRE(book.ask.size() == 0);
                    REQUIRE(book.bid.size() == 1);

                    CHECK_THAT(book.bid[0].price, PRICE_MATCHER(830.0));
                    CHECK_THAT(book.bid[0].quantity, QUANTITY_MATCHER(1.75));
                }

                SECTION("from different exchanges")
                {
                    sendMarketUpdate(Exchange::BINANCE, Side::BID, 830.0, 1.25);
                    sendMarketUpdate(Exchange::BYBIT, Side::BID, 830.0, 1.75);
                    auto book = aggregator.generateBook();

                    REQUIRE(book.ask.size() == 0);
                    REQUIRE(book.bid.size() == 1);

                    CHECK_THAT(book.bid[0].price, PRICE_MATCHER(830.0));
                    CHECK_THAT(book.bid[0].quantity, QUANTITY_MATCHER(3.00));
                }
            }

            SECTION("at different prices")
            {
                SECTION("from the same exchange")
                {
                    sendMarketUpdate(Exchange::BINANCE, Side::BID, 830.0, 2.25);
                    sendMarketUpdate(Exchange::BINANCE, Side::BID, 820.0, 2.75);
                    auto book = aggregator.generateBook();

                    REQUIRE(book.ask.size() == 0);
                    REQUIRE(book.bid.size() == 2);

                    CHECK_THAT(book.bid[0].price, PRICE_MATCHER(830.0));
                    CHECK_THAT(book.bid[0].quantity, QUANTITY_MATCHER(2.25));

                    CHECK_THAT(book.bid[1].price, PRICE_MATCHER(820.0));
                    CHECK_THAT(book.bid[1].quantity, QUANTITY_MATCHER(2.75));
                }

                SECTION("from different exchanges")
                {
                    sendMarketUpdate(Exchange::BINANCE, Side::BID, 830.0, 2.25);
                    sendMarketUpdate(Exchange::BYBIT, Side::BID, 820.0, 2.75);
                    auto book = aggregator.generateBook();

                    REQUIRE(book.ask.size() == 0);
                    REQUIRE(book.bid.size() == 2);

                    CHECK_THAT(book.bid[0].price, PRICE_MATCHER(830.0));
                    CHECK_THAT(book.bid[0].quantity, QUANTITY_MATCHER(2.25));

                    CHECK_THAT(book.bid[1].price, PRICE_MATCHER(820.0));
                    CHECK_THAT(book.bid[1].quantity, QUANTITY_MATCHER(2.75));
                }
            }
        }
    }

    SECTION("for a sequence of several market updates")
    {
        SECTION("on ask side")
        {
            auto book = aggregator.generateBook();
            REQUIRE(book.ask.size() == 0);
            REQUIRE(book.bid.size() == 0);

            sendMarketUpdate(Exchange::BINANCE, Side::ASK, 22.0, 10.0);
            book = aggregator.generateBook();
            REQUIRE(book.ask.size() == 1);
            REQUIRE(book.bid.size() == 0);
            CHECK_THAT(book.ask[0].price, PRICE_MATCHER(22.0));
            CHECK_THAT(book.ask[0].quantity, QUANTITY_MATCHER(10.0));

            sendMarketUpdate(Exchange::BINANCE, Side::ASK, 23.0, 10.0);
            book = aggregator.generateBook();
            REQUIRE(book.ask.size() == 2);
            REQUIRE(book.bid.size() == 0);
            CHECK_THAT(book.ask[0].price, PRICE_MATCHER(22.0));
            CHECK_THAT(book.ask[0].quantity, QUANTITY_MATCHER(10.0));
            CHECK_THAT(book.ask[1].price, PRICE_MATCHER(23.0));
            CHECK_THAT(book.ask[1].quantity, QUANTITY_MATCHER(10.0));

            sendMarketUpdate(Exchange::KRAKEN, Side::ASK, 22.0, 20.0);
            book = aggregator.generateBook();
            REQUIRE(book.ask.size() == 2);
            REQUIRE(book.bid.size() == 0);
            CHECK_THAT(book.ask[0].price, PRICE_MATCHER(22.0));
            CHECK_THAT(book.ask[0].quantity, QUANTITY_MATCHER(30.0));
            CHECK_THAT(book.ask[1].price, PRICE_MATCHER(23.0));
            CHECK_THAT(book.ask[1].quantity, QUANTITY_MATCHER(10.0));

            sendMarketUpdate(Exchange::HYPERLIQUID, Side::ASK, 23.0, 50.0);
            book = aggregator.generateBook();
            REQUIRE(book.ask.size() == 2);
            REQUIRE(book.bid.size() == 0);
            CHECK_THAT(book.ask[0].price, PRICE_MATCHER(22.0));
            CHECK_THAT(book.ask[0].quantity, QUANTITY_MATCHER(30.0));
            CHECK_THAT(book.ask[1].price, PRICE_MATCHER(23.0));
            CHECK_THAT(book.ask[1].quantity, QUANTITY_MATCHER(60.0));

            sendMarketUpdate(Exchange::BINANCE, Side::ASK, 22.0, 0.0);
            book = aggregator.generateBook();
            REQUIRE(book.ask.size() == 2);
            REQUIRE(book.bid.size() == 0);
            CHECK_THAT(book.ask[0].price, PRICE_MATCHER(22.0));
            CHECK_THAT(book.ask[0].quantity, QUANTITY_MATCHER(20.0));
            CHECK_THAT(book.ask[1].price, PRICE_MATCHER(23.0));
            CHECK_THAT(book.ask[1].quantity, QUANTITY_MATCHER(60.0));

            sendMarketUpdate(Exchange::KRAKEN, Side::ASK, 22.0, 0.0);
            book = aggregator.generateBook();
            REQUIRE(book.ask.size() == 1);
            REQUIRE(book.bid.size() == 0);
            CHECK_THAT(book.ask[0].price, PRICE_MATCHER(23.0));
            CHECK_THAT(book.ask[0].quantity, QUANTITY_MATCHER(60.0));

            sendMarketUpdate(Exchange::HYPERLIQUID, Side::ASK, 23.0, 0.0);
            book = aggregator.generateBook();
            REQUIRE(book.ask.size() == 1);
            REQUIRE(book.bid.size() == 0);
            CHECK_THAT(book.ask[0].price, PRICE_MATCHER(23.0));
            CHECK_THAT(book.ask[0].quantity, QUANTITY_MATCHER(10.0));

            sendMarketUpdate(Exchange::BINANCE, Side::ASK, 23.0, 0.0);
            book = aggregator.generateBook();
            REQUIRE(book.ask.size() == 0);
            REQUIRE(book.bid.size() == 0);
        }

        SECTION("on bid side")
        {
            auto book = aggregator.generateBook();
            REQUIRE(book.ask.size() == 0);
            REQUIRE(book.bid.size() == 0);

            sendMarketUpdate(Exchange::BINANCE, Side::BID, 22.0, 10.0);
            book = aggregator.generateBook();
            REQUIRE(book.ask.size() == 0);
            REQUIRE(book.bid.size() == 1);
            CHECK_THAT(book.bid[0].price, PRICE_MATCHER(22.0));
            CHECK_THAT(book.bid[0].quantity, QUANTITY_MATCHER(10.0));

            sendMarketUpdate(Exchange::BINANCE, Side::BID, 23.0, 10.0);
            book = aggregator.generateBook();
            REQUIRE(book.ask.size() == 0);
            REQUIRE(book.bid.size() == 2);
            CHECK_THAT(book.bid[0].price, PRICE_MATCHER(23.0));
            CHECK_THAT(book.bid[0].quantity, QUANTITY_MATCHER(10.0));
            CHECK_THAT(book.bid[1].price, PRICE_MATCHER(22.0));
            CHECK_THAT(book.bid[1].quantity, QUANTITY_MATCHER(10.0));

            sendMarketUpdate(Exchange::KRAKEN, Side::BID, 22.0, 20.0);
            book = aggregator.generateBook();
            REQUIRE(book.ask.size() == 0);
            REQUIRE(book.bid.size() == 2);
            CHECK_THAT(book.bid[0].price, PRICE_MATCHER(23.0));
            CHECK_THAT(book.bid[0].quantity, QUANTITY_MATCHER(10.0));
            CHECK_THAT(book.bid[1].price, PRICE_MATCHER(22.0));
            CHECK_THAT(book.bid[1].quantity, QUANTITY_MATCHER(30.0));

            sendMarketUpdate(Exchange::HYPERLIQUID, Side::BID, 23.0, 50.0);
            book = aggregator.generateBook();
            REQUIRE(book.ask.size() == 0);
            REQUIRE(book.bid.size() == 2);
            CHECK_THAT(book.bid[0].price, PRICE_MATCHER(23.0));
            CHECK_THAT(book.bid[0].quantity, QUANTITY_MATCHER(60.0));
            CHECK_THAT(book.bid[1].price, PRICE_MATCHER(22.0));
            CHECK_THAT(book.bid[1].quantity, QUANTITY_MATCHER(30.0));

            sendMarketUpdate(Exchange::BINANCE, Side::BID, 22.0, 0.0);
            book = aggregator.generateBook();
            REQUIRE(book.ask.size() == 0);
            REQUIRE(book.bid.size() == 2);
            CHECK_THAT(book.bid[0].price, PRICE_MATCHER(23.0));
            CHECK_THAT(book.bid[0].quantity, QUANTITY_MATCHER(60.0));
            CHECK_THAT(book.bid[1].price, PRICE_MATCHER(22.0));
            CHECK_THAT(book.bid[1].quantity, QUANTITY_MATCHER(20.0));

            sendMarketUpdate(Exchange::KRAKEN, Side::BID, 22.0, 0.0);
            book = aggregator.generateBook();
            REQUIRE(book.ask.size() == 0);
            REQUIRE(book.bid.size() == 1);
            CHECK_THAT(book.bid[0].price, PRICE_MATCHER(23.0));
            CHECK_THAT(book.bid[0].quantity, QUANTITY_MATCHER(60.0));

            sendMarketUpdate(Exchange::HYPERLIQUID, Side::BID, 23.0, 0.0);
            book = aggregator.generateBook();
            REQUIRE(book.ask.size() == 0);
            REQUIRE(book.bid.size() == 1);
            CHECK_THAT(book.bid[0].price, PRICE_MATCHER(23.0));
            CHECK_THAT(book.bid[0].quantity, QUANTITY_MATCHER(10.0));

            sendMarketUpdate(Exchange::BINANCE, Side::BID, 23.0, 0.0);
            book = aggregator.generateBook();
            REQUIRE(book.ask.size() == 0);
            REQUIRE(book.bid.size() == 0);
        }
    }
}

TEST_CASE("aggregate correctly quantities from different exchanges to build a price level", "[business]")
{
    auto quantities = Aggregator::Quantities{};
    CHECK(quantities.empty());
    CHECK_THAT(quantities.total(), QUANTITY_MATCHER(0.0));

    quantities.set(Exchange::BINANCE, 0.25);
    CHECK(!quantities.empty());
    CHECK_THAT(quantities.total(), QUANTITY_MATCHER(0.25));

    quantities.set(Exchange::KRAKEN, 0.75);
    CHECK(!quantities.empty());
    CHECK_THAT(quantities.total(), QUANTITY_MATCHER(1.00));

    quantities.set(Exchange::OKX, 1.50);
    CHECK(!quantities.empty());
    CHECK_THAT(quantities.total(), QUANTITY_MATCHER(2.50));

    quantities.set(Exchange::BINANCE, 1.25);
    CHECK(!quantities.empty());
    CHECK_THAT(quantities.total(), QUANTITY_MATCHER(3.50));

    quantities.set(Exchange::KRAKEN, 0.25);
    CHECK(!quantities.empty());
    CHECK_THAT(quantities.total(), QUANTITY_MATCHER(3.00));

    quantities.set(Exchange::OKX, 0.0);
    CHECK(!quantities.empty());
    CHECK_THAT(quantities.total(), QUANTITY_MATCHER(1.50));

    quantities.reset(Exchange::BINANCE);
    CHECK(!quantities.empty());
    CHECK_THAT(quantities.total(), QUANTITY_MATCHER(0.25));

    quantities.reset(Exchange::KRAKEN);
    CHECK(quantities.empty());
    CHECK_THAT(quantities.total(), QUANTITY_MATCHER(0.0));
}
