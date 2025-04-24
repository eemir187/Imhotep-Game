#pragma once

#include <SFML/Network/TcpSocket.hpp>
#include <string>
#include <tuple>

namespace net
{
std::tuple<sf::TcpSocket::Status, std::string> retrieveMessage(sf::TcpSocket &socket);
}
