#pragma once

#include "core/Book.hpp"
#include "core/Display.hpp"
#include "rpc/MarketDataHandler.hpp"

#include "marketdata.pb.h"
#include "marketdata.grpc.pb.h"

#include <grpcpp/grpcpp.h>

#include <functional>
#include <iostream>
#include <memory>
#include <string>

class MarketDataClient
{
public:
    MarketDataClient(std::string const& host, int port, MarketDataHandler& handler)
    : handler(handler)
    {
        auto const target = host + ":" + std::to_string(port);
        auto const channel = grpc::CreateChannel(target, grpc::InsecureChannelCredentials());
        stub = marketdata::Provider::NewStub(channel);
    }

    virtual ~MarketDataClient() = default;

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
            handler.get().onSnapshot(snapshot);
        }

        grpc::Status const status = reader->Finish();

        if (!status.ok())
        {
            LOG_ERROR("StreamMarketDataSnapshots RPC failed: " + status.error_message());
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
            handler.get().onUpdate(update);
        }

        grpc::Status const status = reader->Finish();

        if (!status.ok())
        {
            LOG_ERROR("StreamMarketDataUpdates RPC failed: " + status.error_message());
        }
    }

private:
    std::unique_ptr<marketdata::Provider::Stub> stub;
    std::reference_wrapper<MarketDataHandler> handler;
};
