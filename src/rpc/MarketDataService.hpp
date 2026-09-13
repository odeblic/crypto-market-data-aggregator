#pragma once

#include "core/Book.hpp"

#include "marketdata.pb.h"
#include "marketdata.grpc.pb.h"

#include <grpcpp/grpcpp.h>

#include <chrono>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <utility>

class MarketDataService final : public marketdata::Provider::Service
{
public:
    auto StreamMarketDataSnapshots(
        grpc::ServerContext * context,
        marketdata::Request const * request,
        grpc::ServerWriter<marketdata::Snapshot> * writer) -> grpc::Status override
    {
        std::cout << "New subscription for symbol: " << request->symbol() << std::endl;

        while (!context->IsCancelled())
        {
            auto snapshot = createSnapshot();

            if (!writer->Write(snapshot))
            {
                break; // broken pipe (client disconnected)
                //return Status(grpc::StatusCode::CANCELLED, "Client disconnected");
            }

            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        std::cout << "End of subscription for symbol: " << request->symbol() << std::endl;
        return grpc::Status::OK;
    }

    void updateBook(std::unique_ptr<Book> book, std::string symbol = "BTC/USDT")
    {
        std::scoped_lock<std::mutex> lock(mutex);
        this->symbol = symbol;
        this->book = std::move(book);
    }

private:
    auto createSnapshot() const -> marketdata::Snapshot
    {
        std::scoped_lock<std::mutex> lock(mutex);
        marketdata::Snapshot snapshot;
        snapshot.set_symbol(symbol);

        auto addPriceLevel = [](marketdata::PriceLevel * level, double price, double quantity)
        {
            level->set_price(price);
            level->set_quantity(quantity);
        };

        for (auto const& quote : book->ask)
        {
            addPriceLevel(snapshot.add_asks(), quote.price, quote.quantity);
        }

        for (auto const& quote : book->bid)
        {
            addPriceLevel(snapshot.add_bids(), quote.price, quote.quantity);
        }

        return snapshot;
    }

    std::string symbol;
    std::unique_ptr<Book> book;
    std::mutex mutable mutex;
};
