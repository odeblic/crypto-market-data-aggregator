#include "rpc/MarketDataClient.hpp"
#include "rpc/MarketDataHandlerBestBidOffer.hpp"

#include <nlohmann/json.hpp>

int main(int argc, char ** argv)
{
    auto handler = MarketDataHandlerBestBidOffer{};
    auto client = MarketDataClient{"localhost", 50051, handler};
    client.StreamMarketDataSnapshots("BTCUSD");
    return 0;
}
