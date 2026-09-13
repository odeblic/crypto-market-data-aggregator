#include "cfg/Loader.hpp"
#include "cfg/InternalExchangeConfiguration.hpp"
#include "core/Arguments.hpp"
#include "ws/WebsocketServer.hpp"

#include <nlohmann/json.hpp>

#include <iostream>

int main(int argc, char * argv[])
{
    auto arguments = Arguments{argc, argv};
    auto path = arguments.getConfigFilePath();
    auto config = loadConfigFromFile<InternalExchangeConfiguration>(path);
    auto server = WebsocketServer{config.service.host, config.service.port, config.service.cert, config.service.pkey};
    std::cout << "Listening on: " << config.service.host << ":" << config.service.port << "\n";
    server.run();
}
