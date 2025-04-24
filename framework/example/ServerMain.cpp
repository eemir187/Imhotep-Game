#include "network/Server.hpp"
#include "network/Exceptions.hpp"

#include <iostream>
#include <string>
#include <thread>
#include <sstream>

int main()
{
    net::Server myServer(2);

    while (true)
    {
        auto msg = myServer.awaitAnyClientMessage();
        std::cout << "RECEIVED: (" << msg.player_id << ": " << msg.content << ")" << std::endl;
    }

    return 0;
}