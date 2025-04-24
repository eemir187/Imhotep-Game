#include "RetrieveMessage.hpp"

#include <SFML/Network/TcpSocket.hpp>
#include <algorithm>
#include <array>
#include <iostream>
#include <tuple>

namespace net
{
std::tuple<sf::TcpSocket::Status, std::string> retrieveMessage(sf::TcpSocket &socket)
{
  // Efficiency is not important. Just a straight forward implementation
  std::array<char, 4096> buffer;
  std::fill(buffer.begin(), buffer.end(), '\0');

  std::string content;
  sf::TcpSocket::Status status;

  size_t received = 0;
  do
  {
    status = socket.receive(buffer.data(), buffer.size(), received);

    if (status == sf::TcpSocket::NotReady)
      break;

    content.insert(content.end(), buffer.begin(), buffer.begin() + received);
  } while (status == sf::TcpSocket::Status::Done && received > buffer.size());

  return {status, std::move(content)};
}
} // namespace net
