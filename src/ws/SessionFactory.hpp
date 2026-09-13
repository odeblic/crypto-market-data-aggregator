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
#include <stdexcept>
#include <string>
#include <utility>

class SessionFactory
{
public:
    explicit SessionFactory(boost::asio::io_context& ioctx, boost::asio::ssl::context& sslctx, MarketDataSink& sink)
    : ioctx(ioctx), sslctx(sslctx), sink(sink)
    {
    }

    auto make(Exchange exchange) -> std::shared_ptr<ClientSession>
    {
        switch (exchange)
        {
        case Exchange::BINANCE:
            return makeExchangeSession<ExchangeSessionBinance>(configBinance);
        case Exchange::BITMEX:
            return makeExchangeSession<ExchangeSessionBitMEX>(configBitMEX);
        case Exchange::BYBIT:
            return makeExchangeSession<ExchangeSessionBybit>(configBybit);
        case Exchange::COINBASE:
            return makeExchangeSession<ExchangeSessionCoinbase>(configCoinbase);
        case Exchange::HYPERLIQUID:
            return makeExchangeSession<ExchangeSessionHyperliquid>(configHyperliquid);
        case Exchange::INTERNAL:
            return makeExchangeSession<ExchangeSessionInternal>(configInternal);
        case Exchange::KRAKEN:
            return makeExchangeSession<ExchangeSessionKraken>(configKraken);
        case Exchange::OKX:
            return makeExchangeSession<ExchangeSessionOKX>(configOKX);
        default:
            throw std::runtime_error("exchange could not be dealt with");
        }
    }

    auto make(std::string exchange) -> std::shared_ptr<ClientSession>
    {
        return make(fromString<Exchange>(std::move(exchange)));
    }

private:
    template <typename T>
    auto makeExchangeSession(ExchangeConfiguration config) -> std::shared_ptr<ClientSession>
    {
        return std::make_shared<T>(ioctx.get(), sslctx.get(), sink.get(), std::move(config));
    }

    std::reference_wrapper<boost::asio::io_context> ioctx;
    std::reference_wrapper<boost::asio::ssl::context> sslctx;
    std::reference_wrapper<MarketDataSink> sink;
};
