#include "network/Client.hpp"

#include "RetrieveMessage.hpp"
#include "network/Exceptions.hpp"

#include <SFML/Network/IpAddress.hpp>
#include <SFML/System/Time.hpp>
#include <iostream>
#include <limits>
#include <sstream>

namespace net
{
Client::Client(std::string name, unsigned short port, const std::string &ip, unsigned port_retries) :
    Client(name, port, ip, port_retries, "JoinGame " + name + "\n")
{
}

Client::Client(
    std::string name, unsigned short port, const std::string &ip, unsigned port_retries, std::string join_message) :
    name_{std::move(name)}
{
  bool connected = false;
  for (unsigned short port_offset = 0; port_offset < port_retries; port_offset++)
  {
    if (port > std::numeric_limits<unsigned short>::max() - port_offset)
      break;
    if (socket_.connect(ip, port + port_offset) != sf::TcpSocket::Done)
    {
      continue;
    }
    selector_.add(socket_);

    send(join_message);

    // Parse Joined Game message
    std::stringstream msg(awaitServerMessage());

    std::string token;
    msg >> token >> id_ >> number_of_players_;
    // std::cout << "Received message: "<< token << std::endl;
    if (!msg || token != "JoinedGame")
    {
      throw NetworkException("Invalid start message!");
    }

    connected = true;
    break;
  }

  if (!connected)
    throw net::NetworkException("Could not connect to server!");

}

std::string Client::awaitServerMessage(sf::Time timeout)
{
  if (!message_queue_.empty())
  {
    auto message = message_queue_.front();
    message_queue_.erase(message_queue_.begin());
    if (message.empty() || message == "\n" || message == "\r\n")
    {
      return awaitServerMessage();
    }
    return message;
  }

  if (!selector_.wait(timeout))
  {
    throw net::TimeoutException{};
  }

  auto [status, content] = net::retrieveMessage(socket_);

  if (status == sf::TcpSocket::Status::Disconnected)
  {
    throw net::NetworkException("Lost connection to server");
  }
  else if (status == sf::TcpSocket::Status::Done)
  {
    splitAndQueueMessages(content);
  }

  while (!message_queue_.empty())
  {
    auto message = message_queue_.front();
    message_queue_.erase(message_queue_.begin());
    if (message.empty() || message == "\n" || message == "\r\n")
    {
      return awaitServerMessage();
    }
    return message;
  }
  throw net::NetworkException{"Socket failed to retrieve data"};
}

bool Client::send(const std::string &message)
{
  if (message.back() == '\n')
  {
    return send(message.data(), message.size());
  }
  else
  {
    std::string messageWithNewline = message + '\n';
    return send(messageWithNewline.data(), messageWithNewline.size());
  }
}

bool Client::send(const void *data, size_t bytes)
{
  return socket_.send(data, bytes) == sf::TcpSocket::Done;
}

void Client::disconnect()
{
  socket_.disconnect();
}

void Client::splitAndQueueMessages(std::string content)
{
  std::size_t cur = 0;
  while (cur < content.size())
  {
    auto pos = content.find_first_of('\n', cur);
    if (pos == std::string::npos)
    {
      message_queue_.push_back(content.substr(cur));
      break;
    }
    else
    {
      message_queue_.push_back(content.substr(cur, pos - cur + 1));
      cur = pos + 1;
    }
  }
}

} // namespace net
