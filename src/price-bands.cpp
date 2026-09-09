#include "rpc/MarketDataClient.hpp"

#include <nlohmann/json.hpp>

int main(int argc, char ** argv)
{
    auto client = MarketDataClient{"localhost", 50051};
    client.StreamMarketDataSnapshots("BTCUSD");
    return 0;
}
