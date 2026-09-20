#pragma once

#include "core/Book.hpp"
#include "core/Display.hpp"

#include "marketdata.pb.h"
#include "marketdata.grpc.pb.h"

#include <grpcpp/grpcpp.h>

#include <chrono>
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
        LOG_DEBUG("New subscription for symbol: " + request->symbol());

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

        LOG_DEBUG("End of subscription for symbol: " + request->symbol());
        return grpc::Status::OK;
    }

    void overwriteBook(Book book, std::string symbol)
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

        for (auto const& quote : book.ask)
        {
            addPriceLevel(snapshot.add_asks(), quote.price, quote.quantity);
        }

        for (auto const& quote : book.bid)
        {
            addPriceLevel(snapshot.add_bids(), quote.price, quote.quantity);
        }

        return snapshot;
    }

    std::string symbol;
    Book book;
    std::mutex mutable mutex;
};
