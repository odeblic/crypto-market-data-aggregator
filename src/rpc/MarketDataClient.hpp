#pragma once

#include "marketdata.pb.h"
#include "marketdata.grpc.pb.h"

#include <grpcpp/grpcpp.h>

#include <iostream>
#include <memory>
#include <string>

class MarketDataClient
{
public:
    MarketDataClient(std::string const& host, int port)
    {
        auto const target = host + ":" + std::to_string(port);
        auto const channel = grpc::CreateChannel(target, grpc::InsecureChannelCredentials());
        stub = marketdata::Provider::NewStub(channel);
    }

    void StreamMarketDataSnapshots(std::string const& symbol, uint32_t depth = 5)
    {
        marketdata::Request request;
        request.set_symbol(symbol);
        request.set_depth(depth);

        grpc::ClientContext context;
        marketdata::Snapshot snapshot;

        std::unique_ptr<grpc::ClientReader<marketdata::Snapshot>> const reader(
            stub->StreamMarketDataSnapshots(&context, request)
        );

        while (reader->Read(&snapshot))
        {
            onMarketDataSnapshot(snapshot);
        }

        grpc::Status const status = reader->Finish();

        if (!status.ok())
        {
            std::cerr << "StreamMarketDataSnapshots RPC failed: " 
                      << status.error_message() 
                      << " (code " << status.error_code() << ")" 
                      << std::endl;
        }
    }

    void StreamMarketDataUpdates(std::string const& symbol, uint32_t depth = 5)
    {
        marketdata::Request request;
        request.set_symbol(symbol);
        request.set_depth(depth);

        grpc::ClientContext context;
        marketdata::Update update;

        std::unique_ptr<grpc::ClientReader<marketdata::Update>> const reader(
            stub->StreamMarketDataUpdates(&context, request)
        );

        while (reader->Read(&update))
        {
            onMarketDataUpdate(update);
        }

        grpc::Status const status = reader->Finish();

        if (!status.ok())
        {
            std::cerr << "StreamMarketDataUpdates RPC failed: " 
                      << status.error_message() 
                      << " (code " << status.error_code() << ")" 
                      << std::endl;
        }
    }

    virtual void onMarketDataSnapshot(marketdata::Snapshot const& snapshot) const
    {
        std::cout << "snapshot for symbol: " << snapshot.symbol() << "\n";

        for (const auto& ask : snapshot.asks())
        {
            std::cout << "ASK " << ask.price() << " " << ask.quantity() << "\n";
        }

        for (const auto& bid : snapshot.bids())
        {
            std::cout << "BID " << bid.price() << " " << bid.quantity() << "\n";
        }
    }

    virtual void onMarketDataUpdate(marketdata::Update const& update) const
    {
        std::cout << "update for symbol: " << update.symbol()
                  << " | " << (update.side() == marketdata::Side::BID ? "BID" : "ASK")
                  << " | price: " << update.price()
                  << " | quantity: " << update.quantity() 
                  << std::endl;
    }

private:
    std::unique_ptr<marketdata::Provider::Stub> stub;
};
