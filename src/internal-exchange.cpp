#include "ws/WebsocketServer.hpp"

#include <nlohmann/json.hpp>

#include <cstdlib>
#include <iostream>

int main(int argc, char * argv[])
{
    auto server = WebsocketServer{"127.0.0.1", 8080, "certificate.pem", "private-key.pem"};
    std::cout << "Listening on: 127.0.0.1:8080\n";
    server.run();
    return EXIT_SUCCESS;
}
