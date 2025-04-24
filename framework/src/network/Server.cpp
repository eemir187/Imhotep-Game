#include "network/Server.hpp"
#include "network/TestsystemCommunicationBridge.hpp"

#include "RetrieveMessage.hpp"
#include "SFML/Network/IpAddress.hpp"
#include "SFML/System/Err.hpp"
#include "network/Exceptions.hpp"

#include <algorithm>
#include <array>
#include <cassert>
#include <cctype>
#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <limits>
#include <memory>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <thread>
#include <filesystem>

void replaceWindowsLineEndings(std::string &string)
{
  std::size_t pos = string.find("\r\n");
  while (pos != std::string::npos)
  {
    std::cout << string << std::endl;
    string.erase(pos, 1);
    std::cout << "After: " << std::endl;
    std::cout << string << std::endl;
    pos = string.find("\r\n");
  }
}

namespace net
{
Server::Server(unsigned number_of_players) : clients_(number_of_players)
{
  unsigned short port = 50500;
  constexpr unsigned short tries = 2048;
  bool listening = false;

  static std::ostringstream local;
  sf::err().rdbuf(local.rdbuf());

  sf::IpAddress ip = "127.0.0.1";
  if (char *remote_env = std::getenv("OOP1_REMOTE_CLIENTS"); remote_env != nullptr && std::stoi(remote_env) != 0)
  {
    ip = sf::IpAddress::Any;
  }

  for (unsigned short port_offset = 0; port_offset < tries; port_offset++)
  {
    if (port > std::numeric_limits<unsigned short>::max() - port_offset)
      break;

    if (listener_.listen(port + port_offset, ip) != sf::Socket::Status::Done)
    {
      continue;
    }
    listening = true;
    port += port_offset;
    // std::cout << "Listening on port " << port << std::endl;
    break;
  }

  if (!listening)
    throw net::NetworkException{"Failed to bind socket!"};

  {
    std::ofstream port_file("server.port", std::ios::out | std::ios::trunc);
    if (port_file.is_open())
      port_file << std::to_string(port) << std::endl;
    port_file.close();
  }

  port_ = port;

  selector_.add(listener_);

  // Communicate port file readyness to testsystem

  if (std::getenv("TESTING") != nullptr && std::string(std::getenv("TESTING")) == "1")
  {
    tcb_ = std::make_unique<TestsystemCommunicationBridge>("/imhotep", false);

    tcb_->signalNetworkReady();
    tcb_->updateState(TestsystemCommunicationBridge::InternalState::MESSAGE_COMMUNICATION);
  }

  // Wait for all players
  Message message;
  while (!all_players_connected_)
  {
    message = waitForMessage();
  }
}

Server::~Server()
{
  // Send quit message to all clients
  
  output_logger_.close();
  std::string quit_message{"Quit\n"};
  sendToAllClients(quit_message);

  // Remove server wait information if necessary
  
  // if (this->awaiting_message_longer_then_500ms_)
  // {
  //   std::filesystem::remove("server.waiting");
  //   this->awaiting_message_longer_then_500ms_ = false;
  // }

  // Give clients time to disconnect

  std::this_thread::sleep_for(std::chrono::seconds{1});

  // Read all remaining messages and wait for disconnect
  
  auto cleanUpSockets = [](std::vector<Client> &clients, auto buffer)
  {
    for (auto &client : clients)
    {
      if (!client.socket_)
      {
        continue;
      }
      client.socket_->setBlocking(false);
      sf::TcpSocket::Status status;
      do
      {
        std::size_t received;
        status = client.socket_->receive(buffer.data(), buffer.size(), received);
      } while (status == sf::TcpSocket::Done || status == sf::TcpSocket::Partial);

      // std::cout << "Client " << client.name_ << " closed: " << static_cast<int>(status) << std::endl;

      client.socket_->disconnect();
    }
  };

  std::array<unsigned char, 1024> buffer{};
  cleanUpSockets(clients_, buffer);
  cleanUpSockets(spectators_, buffer);
}

Server::Message Server::awaitAnyClientMessage()
{
  // This function should ONLY be called by the user explicitly, not internally.

  if (std::getenv("TESTING") != nullptr && std::string(std::getenv("TESTING")) == "1")
  {
    assert (tcb_ != nullptr); 
    tcb_->signalServerAwaitsInput();
  }
  
  auto tmp = waitForMessage();

  if (std::getenv("TESTING") != nullptr && std::string(std::getenv("TESTING")) == "1")
  {
    std::cout << "RESULT: " << tmp.content << std::endl;
  }

  return tmp;
}

Server::Message Server::waitForMessage()
{
  while (true)
  {
    if (!message_queue_.empty())
    {
      auto message = message_queue_.front();
      message_queue_.erase(message_queue_.begin());
      return message;
    }

    if (selector_.wait())
    {
      handleNewConnection();
      handleIncomingClientMessages();
    }
    else
    {
      throw net::NetworkException{"No socket available to handle connections / client messages!"};
    }
  }
}

void Server::sendToAllClients(std::string content)
{
  if (content.size() == 0) [[unlikely]]
    throw std::runtime_error("Try to send empty message!");
  
  if (content.back() != '\n') [[likely]] /* default for students */
    content.push_back('\n');

  output_logger_ << content << std::flush;

  for (auto &client : clients_)
  {
    if (client.socket_)
    {
      sf::Socket::Status send_status = client.socket_->send(content.data(), content.size());

      if (send_status != sf::Socket::Status::Done) 
        throw NetworkException("Failed to send message - socket::send returned " + std::to_string(send_status));

      client.logger_ << content << std::flush;
    }
  }
  sendToSpectator(content);
}

std::size_t Server::getNumberOfPlayers() const noexcept
{
  return clients_.size();
}

const std::string &Server::getPlayerName(ClientId index) const
{
  return clients_.at(index).name_; // might throw std::out_of_range 
}

void Server::sendToSpectator(const std::string &content)
{
  for (auto &spectator : spectators_)
  {
    if (spectator.socket_)
    {
      spectator.socket_->send(content.data(), content.size());
      spectator.socket_->send("\n", 1);
    }
  }
}

void Server::disconnectClient(ClientId id)
{
  if (id < static_cast<int>(clients_.size()) && clients_[id].socket_)
  {
    selector_.remove(*clients_[id].socket_);
    // Disconnect should not be needed, but sometimes the client does not get
    // notified when destroying the socket (maybe a bug in sfml)
    std::string_view quit_message{"Quit\n"};
    clients_[id].socket_->send(quit_message.data(), quit_message.size());
    clients_[id].socket_->disconnect();
    clients_[id].socket_.reset();
  }
}

void Server::handleNewConnection()
{
  if (selector_.isReady(listener_))
  {
    // Create new client in empty spot
    auto empty_slot = std::find_if(clients_.begin(),
                                   clients_.end(),
                                   [](const Client &c)
                                   {
                                     return c.socket_ == nullptr;
                                   });
    if (empty_slot == clients_.end())
    {
      throw net::NetworkException{"Already " + std::to_string(clients_.size()) + " clients connected!"};
    }

    auto client_index = static_cast<ClientId>(std::distance(clients_.begin(), empty_slot));

    auto new_socket = std::make_unique<sf::TcpSocket>();

    if (listener_.accept(*new_socket) != sf::TcpSocket::Done)
    {
      empty_slot->socket_.reset();
      throw net::NetworkException{"Network failure"};
    }

    auto [status, message] = net::retrieveMessage(*new_socket);
    bool windows_file_endings = message.find_first_of("\r\n") != std::string::npos;
    replaceWindowsLineEndings(message);

    if (message == "JoinAsSpectator\n")
    {
      spectators_.push_back({
        .windows_line_endings = windows_file_endings, 
        .socket_ = std::move(new_socket), 
        .name_ = "spectator", 
        .logger_ = {}
      });
      std::string answer = std::string("JoinedAsSpectator") + " " + std::to_string(clients_.size()) + "\n";
      spectators_.back().socket_->send(answer.data(), answer.size());
      return;
    }

    empty_slot->windows_line_endings = windows_file_endings;
    empty_slot->socket_ = std::move(new_socket);
    auto &client_socket = *(empty_slot->socket_);
    // New client connected!
    selector_.add(client_socket);

    if (status != sf::TcpSocket::Done)
    {
      // New client failed to send JoinGame
      disconnectClient(client_index);
      return;
    }
    std::string prefix = "joingame ";

    if (message.size() < prefix.size())
    {
      // Message too short
      disconnectClient(client_index);
      return;
    }

    std::transform(message.begin(),
                   std::next(message.begin(), prefix.size()),
                   message.begin(),
                   [](unsigned char c)
                   {
                     return std::tolower(c);
                   });
    if (std::mismatch(prefix.begin(), prefix.end(), message.begin()).first != prefix.end())
    {
      // New client did not open with JoinGame
      disconnectClient(client_index);
      return;
    }

    if (message.find_first_of(" ", prefix.size()) != std::string::npos)
    {
      // Multiple spaces in message
      std::cout << "Player name was invalid (contained spaces)!" << std::endl;
      disconnectClient(client_index);
      return;
    }

    empty_slot->name_ = message.substr(prefix.size());
    empty_slot->name_.pop_back(); // Remove \n

    // Open logger
    empty_slot->logger_ = std::ofstream(empty_slot->name_ + ".out", std::ios::trunc);

    //
    // Tell the client his player_id
    std::string answer =
        std::string("JoinedGame ") + std::to_string(client_index) + " " + std::to_string(clients_.size()) + "\n";
    client_socket.send(answer.data(), answer.size());
    empty_slot->logger_ << answer << std::flush;

    /*
    std::cout << "Player " << empty_slot->name_ << " with id " << std::to_string(client_index) << " connected!"
              << std::endl;
    */
    auto count = std::count_if(clients_.begin(),
                               clients_.end(),
                               [](Client &c)
                               {
                                 return c.socket_ == nullptr;
                               });
    if (count > 0)
    {
      return;
    }
    else
    {
      // Build name list
      message_queue_.clear();
      all_players_connected_ = true;
      splitAndQueueMessages(ClientId{-1}, message);

      selector_.remove(listener_);
      listener_.close();
    }
  }
}

void Server::handleIncomingClientMessages()
{
  // Check for data from clients
  
  [[maybe_unused]] bool found = false;
  
  // Create server.waiting file to indicate server waits for input but didn't after 0.5s
  
  if (!this->awaiting_message_longer_then_500ms_ && selector_.wait(sf::milliseconds(500)))
  {
    this->awaiting_message_longer_then_500ms_ = true;
    // std::ofstream file("server.waiting");
    // if (file.is_open()) 
    //   file.close();
    
    // Simply recall method
    handleIncomingClientMessages();
    return;
  }
  
  // --------------------
  
  for (std::size_t i = 0; i < clients_.size(); i++)
  {
    auto &client_socket = clients_[i].socket_;

    if (!client_socket || !selector_.isReady(*client_socket))
    {
      continue; // Do no break because the active sockets could be 1,4 for example
    }
    found = true;

    auto [status, content] = net::retrieveMessage(*client_socket); // blocks until received
    
    // If server waited a very long time => remove file to indicate end of "waiting" state.
    // if (!this->awaiting_message_longer_then_500ms_)
    //   std::filesystem::remove("server.waiting");
  
    replaceWindowsLineEndings(content);

    if (status == sf::TcpSocket::Disconnected)
    {
      disconnectClient(i);
      sendToSpectator("LostConnection " + std::to_string(i));
      throw net::NetworkException{"Lost connection to client " + std::to_string(i)};
    }

    splitAndQueueMessages(static_cast<ClientId>(i), content);
  }
}

void Server::splitAndQueueMessages(ClientId client_id, std::string content)
{
  std::size_t cur = 0;
  while (cur < content.size())
  {
    auto pos = content.find_first_of('\n', cur);
    if (pos == std::string::npos)
    {
      message_queue_.push_back({client_id, content.substr(cur)});
      break;
    }
    else
    {
      message_queue_.push_back({client_id, content.substr(cur, pos - cur)});
      cur = pos + 1;
    }
    input_logger_ << std::to_string(message_queue_.back().player_id) << ":" << message_queue_.back().content
                  << std::endl
                  << std::flush;
  }
}

} // namespace net
