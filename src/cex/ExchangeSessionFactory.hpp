#pragma once

#include "ExchangeSession.hpp"
#include "ExchangeSessionBinance.hpp"
#include "ExchangeSessionBitMEX.hpp"
#include "ExchangeSessionBybit.hpp"
#include "ExchangeSessionCoinbase.hpp"
#include "ExchangeSessionKraken.hpp"
#include "ExchangeSessionOKX.hpp"
#include "ExchangeConfiguration.hpp"
#include "MarketUpdate.hpp"

#include <boost/asio/ssl.hpp>

#include <algorithm>
#include <cctype>
#include <functional>
#include <memory>
#include <string>

namespace net = boost::asio;

class ExchangeSessionFactory
{
public:
    explicit ExchangeSessionFactory(net::io_context& ioctx, ssl::context& sslctx)
    : ioctx(ioctx), sslctx(sslctx)
    {
    }

    auto make(Exchange exchange) -> std::shared_ptr<ExchangeSession>
    {
        switch (exchange)
        {
        case Exchange::BINANCE:
            return std::make_shared<ExchangeSessionBinance>(ioctx.get(), sslctx.get(), configBinance);
        case Exchange::BITMEX:
            return std::make_shared<ExchangeSessionBitMEX>(ioctx.get(), sslctx.get(), configBitMEX);
        case Exchange::BYBIT:
            return std::make_shared<ExchangeSessionBybit>(ioctx.get(), sslctx.get(), configBybit);
        case Exchange::COINBASE:
            return std::make_shared<ExchangeSessionCoinbase>(ioctx.get(), sslctx.get(), configCoinbase);
        case Exchange::KRAKEN:
            return std::make_shared<ExchangeSessionKraken>(ioctx.get(), sslctx.get(), configKraken);
        case Exchange::OKX:
            return std::make_shared<ExchangeSessionOKX>(ioctx.get(), sslctx.get(), configOKX);
        default:
            return nullptr;
        }
    }

    auto make(std::string exchange) -> std::shared_ptr<ExchangeSession>
    {
        std::transform(exchange.begin(), exchange.end(), exchange.begin(), [](unsigned char c) {
            return std::toupper(c);
        });

        if (exchange == "BINANCE")
        {
            return make(Exchange::BINANCE);
        }
        else if (exchange == "BITMEX")
        {
            return make(Exchange::BITMEX);
        }
        else if (exchange == "BYBIT")
        {
            return make(Exchange::BYBIT);
        }
        else if (exchange == "COINBASE")
        {
            return make(Exchange::COINBASE);
        }
        else if (exchange == "KRAKEN")
        {
            return make(Exchange::KRAKEN);
        }
        else if (exchange == "OKX")
        {
            return make(Exchange::OKX);
        }
        else
        {
            return nullptr;
        }
    }

private:
    std::reference_wrapper<net::io_context> ioctx;
    std::reference_wrapper<ssl::context> sslctx;
};
