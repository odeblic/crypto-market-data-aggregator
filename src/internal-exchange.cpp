#include "cfg/Loader.hpp"
#include "cfg/InternalExchangeConfiguration.hpp"
#include "core/Arguments.hpp"
#include "core/Display.hpp"
#include "ws/WebsocketServer.hpp"

#include <nlohmann/json.hpp>

#include <iostream>

int main(int argc, char * argv[])
{
    auto arguments = Arguments{argc, argv};
    auto path = arguments.getConfigFilePath();
    auto config = loadConfigFromFile<InternalExchangeConfiguration>(path);
    Display::instantiate(config.verbose, true);
    LOG_INFO("starting the internal exchange");
    auto server = WebsocketServer{config.service.host, config.service.port, config.service.cert, config.service.pkey};
    LOG_DEBUG("Listening on: " + config.service.host + ":" + std::to_string(config.service.port));
    server.run();
}
