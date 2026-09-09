#pragma once

#include "marketdata.pb.h"
#include "marketdata.grpc.pb.h"

#include <grpcpp/grpcpp.h>

#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

class MarketDataService final : public marketdata::Provider::Service
{
    auto StreamMarketDataSnapshots(
        grpc::ServerContext * context,
        marketdata::Request const * request,
        grpc::ServerWriter<marketdata::Snapshot> * writer) -> grpc::Status override
    {
        std::cout << "New subscription for symbol: " << request->symbol() << std::endl;

        marketdata::Snapshot snapshot;
        snapshot.set_symbol(request->symbol().empty() ? "BTC-USD" : request->symbol());

        auto addPriceLevel = [](marketdata::PriceLevel * level, double price, double quantity)
        {
            level->set_price(price);
            level->set_quantity(quantity);
        };

        addPriceLevel(snapshot.add_bids(), 50000.0, 1.5);
        addPriceLevel(snapshot.add_bids(), 49990.0, 2.0);
        addPriceLevel(snapshot.add_bids(), 49980.0, 5.5);

        addPriceLevel(snapshot.add_asks(), 50010.0, 0.8);
        addPriceLevel(snapshot.add_asks(), 50020.0, 3.1);
        addPriceLevel(snapshot.add_asks(), 50030.0, 4.0);

        while (!context->IsCancelled())
        {
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
};
