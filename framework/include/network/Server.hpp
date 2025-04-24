#pragma once

#include <SFML/Network/SocketSelector.hpp>
#include <SFML/Network/TcpListener.hpp>
#include <SFML/Network/TcpSocket.hpp>
#include "TestsystemCommunicationBridge.hpp"
#include <fstream>
#include <memory>
#include <optional>
#include <string>
#include <vector>

/**
 * DEAR STUDENTS,
 * 
 * THIS IS THE CORRECT HEADER FILE THAT YOU SHOULD INCLUDE FOR A0-A2 IN YOUR APPLICATION: #include "network/Server.hpp"
 * YOU CAN FIND A DESCRIPTION IN THE ASSIGNMENT SPECIFICATION <Framework.md>.
 * DON'T MOVE THE HEADER FILE OR COPY IT'S CONTENT.
 * 
 * SINCERELY,
 * YOUR TUTORS
 */

namespace net
{
class Server final
{
public:
  using ClientId = int;

  struct Message
  {
    /**
     * The network message sender's id.
     */
    ClientId player_id;

    /**
     * The content of the network message.
     */
    std::string content;
  };

public:
  /**
   * Constructor.
   * Initializes network communication and pauses execution until all clients are connected to the network. 
   * 
   * @throws net::NetworkException If an error occurred during the network communication setup.
   * @param number_of_players The amount of expected clients to conenct.
   */
  Server(unsigned number_of_players);

  /**
   * Destructor.
   * Disconnects all clients and clears sockets. 
   */
  ~Server();

  /**
   * Pauses execution until a message from a connected client was received. 
   * 
   * @throws net::NetworkException If a network-related error occurs. 
   * @return net::Server::Message The received client message.
   */
  [[nodiscard]] Message awaitAnyClientMessage();

  /**
   * Sends a message to all connected clients.
   *  
   * @throws net::NetworkException if sending was not successful due to internal network error.
   * @throws std::runtime_error if content is completely ill-formed that it might cause network issues. 
   * @param content The message's content about to send.   
   */
  void sendToAllClients(std::string content);

  /**
   * Getter for number_of_players. 
   * 
   * @return std::size_t The total amount of connected clients.
   */
  [[nodiscard]] std::size_t getNumberOfPlayers() const noexcept;

  /**
   * Fetches a player's name.
   * 
   * @throws std::out_of_range If provided player_index is not inside [0; getNumberOfPlayers()-1)]. 
   * @param player_index The target player's index.
   * @return std::string The target player's name.
   */
  [[nodiscard]] const std::string &getPlayerName(ClientId player_index) const;

  /* ALL OTHER METHODS AND PROPERTIES ARE NOT MEANT TO BE USED FROM STUDENTS */

#pragma region NOT_FOR_STUDENT_USAGE

  Server(const Server&) = delete;
  Server(Server&&) = delete;
  Server& operator=(const Server&) = delete;

private:
  void sendToSpectator(const std::string &content);

  void disconnectClient(ClientId id);
  void splitAndQueueMessages(ClientId client_id, std::string content);

  void handleNewConnection();
  void handleIncomingClientMessages();

  Message waitForMessage();


private:
  unsigned short port_;
  sf::SocketSelector selector_;
  sf::TcpListener listener_;

  struct Client
  {
    bool windows_line_endings{false};
    std::unique_ptr<sf::TcpSocket> socket_{};
    std::string name_{};
    std::ofstream logger_{};
  };

  std::vector<Client> clients_;
  std::vector<Client> spectators_;

  std::vector<Message> message_queue_;

  bool all_players_connected_{false};

  std::ofstream input_logger_{"server.in", std::ios::trunc};
  std::ofstream output_logger_{"server.out", std::ios::trunc};

  bool awaiting_message_longer_then_500ms_ {false};

  std::string line_endings_{'\n'};

private:
  std::unique_ptr<TestsystemCommunicationBridge> tcb_;

#pragma endregion
};
} // namespace net
