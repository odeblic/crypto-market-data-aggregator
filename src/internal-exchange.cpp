#include "cfg/Loader.hpp"
#include "cfg/InternalExchangeConfiguration.hpp"
#include "core/ArgumentParser.hpp"
#include "core/Display.hpp"
#include "ws/WebsocketServer.hpp"

#include <nlohmann/json.hpp>

#include <iostream>

int main(int argc, char * argv[])
{
    auto parser = ArgumentParser{argc, argv};
    parser.assertExchangeCount(0);
    auto arguments = parser.getArguments();
    auto const config = loadConfigFromFile<InternalExchangeConfiguration>(arguments.configPath);
    auto const verbose = config.verbose || arguments.verbose;
    Display::instantiate(verbose, true);
    LOG_INFO("starting the internal exchange");
    auto server = WebsocketServer{config.service.host, config.service.port, config.service.cert, config.service.pkey};
    LOG_DEBUG("Listening on: " + config.service.host + ":" + std::to_string(config.service.port));
    server.run();
}
