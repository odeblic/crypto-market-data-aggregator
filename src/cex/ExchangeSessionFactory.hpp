#pragma once

#include "cex/ExchangeSession.hpp"
#include "cex/ExchangeSessionBinance.hpp"
#include "cex/ExchangeSessionBitMEX.hpp"
#include "cex/ExchangeSessionBybit.hpp"
#include "cex/ExchangeSessionCoinbase.hpp"
#include "cex/ExchangeSessionKraken.hpp"
#include "cex/ExchangeSessionOKX.hpp"
#include "cex/ExchangeConfiguration.hpp"
#include "cex/MarketUpdate.hpp"
#include "cex/Utils.hpp"
#include "core/MarketDataSink.hpp"

#include <boost/asio/ssl.hpp>

#include <functional>
#include <memory>
#include <string>

namespace net = boost::asio;

class ExchangeSessionFactory
{
public:
    explicit ExchangeSessionFactory(net::io_context& ioctx, ssl::context& sslctx, MarketDataSink& sink)
    : ioctx(ioctx), sslctx(sslctx), sink(sink)
    {
    }

    auto make(Exchange exchange) -> std::shared_ptr<ExchangeSession>
    {
        switch (exchange)
        {
        case Exchange::BINANCE:
            return std::make_shared<ExchangeSessionBinance>(ioctx.get(), sslctx.get(), sink.get(), configBinance);
        case Exchange::BITMEX:
            return std::make_shared<ExchangeSessionBitMEX>(ioctx.get(), sslctx.get(), sink.get(), configBitMEX);
        case Exchange::BYBIT:
            return std::make_shared<ExchangeSessionBybit>(ioctx.get(), sslctx.get(), sink.get(), configBybit);
        case Exchange::COINBASE:
            return std::make_shared<ExchangeSessionCoinbase>(ioctx.get(), sslctx.get(), sink.get(), configCoinbase);
        case Exchange::KRAKEN:
            return std::make_shared<ExchangeSessionKraken>(ioctx.get(), sslctx.get(), sink.get(), configKraken);
        case Exchange::OKX:
            return std::make_shared<ExchangeSessionOKX>(ioctx.get(), sslctx.get(), sink.get(), configOKX);
        default:
            return nullptr;
        }
    }

    auto make(std::string exchange) -> std::shared_ptr<ExchangeSession>
    {
        return make(fromString<Exchange>(exchange));
    }

private:
    std::reference_wrapper<net::io_context> ioctx;
    std::reference_wrapper<ssl::context> sslctx;
    std::reference_wrapper<MarketDataSink> sink;
};
