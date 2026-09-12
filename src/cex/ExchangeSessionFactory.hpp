#pragma once

#include "cex/ExchangeSessionBinance.hpp"
#include "cex/ExchangeSessionBitMEX.hpp"
#include "cex/ExchangeSessionBybit.hpp"
#include "cex/ExchangeSessionCoinbase.hpp"
#include "cex/ExchangeSessionHyperliquid.hpp"
#include "cex/ExchangeSessionInternal.hpp"
#include "cex/ExchangeSessionKraken.hpp"
#include "cex/ExchangeSessionOKX.hpp"
#include "cex/ExchangeConfiguration.hpp"
#include "core/MarketDataSink.hpp"
#include "core/MarketUpdate.hpp"
#include "core/Utils.hpp"
#include "ws/ClientSession.hpp"

#include <boost/asio/io_context.hpp>
#include <boost/asio/ssl/context.hpp>

#include <functional>
#include <memory>
#include <string>

class ExchangeSessionFactory
{
public:
    explicit ExchangeSessionFactory(boost::asio::io_context& ioctx, boost::asio::ssl::context& sslctx, MarketDataSink& sink)
    : ioctx(ioctx), sslctx(sslctx), sink(sink)
    {
    }

    auto make(Exchange exchange) -> std::shared_ptr<ClientSession>
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
        case Exchange::HYPERLIQUID:
            return std::make_shared<ExchangeSessionHyperliquid>(ioctx.get(), sslctx.get(), sink.get(), configHyperliquid);
        case Exchange::INTERNAL:
            return std::make_shared<ExchangeSessionInternal>(ioctx.get(), sslctx.get(), sink.get(), configInternal);
        case Exchange::KRAKEN:
            return std::make_shared<ExchangeSessionKraken>(ioctx.get(), sslctx.get(), sink.get(), configKraken);
        case Exchange::OKX:
            return std::make_shared<ExchangeSessionOKX>(ioctx.get(), sslctx.get(), sink.get(), configOKX);
        default:
            return nullptr;
        }
    }

    auto make(std::string exchange) -> std::shared_ptr<ClientSession>
    {
        return make(fromString<Exchange>(exchange));
    }

private:
    std::reference_wrapper<boost::asio::io_context> ioctx;
    std::reference_wrapper<boost::asio::ssl::context> sslctx;
    std::reference_wrapper<MarketDataSink> sink;
};
