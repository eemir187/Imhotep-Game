#pragma once

#include <SFML/Network/SocketSelector.hpp>
#include <SFML/Network/TcpSocket.hpp>
#include <SFML/System/Time.hpp>
#include <string>

/**
 * DEAR STUDENTS,
 * 
 * THIS HEADER FILE IS ONLY MEANT TO BE INCLUDED IN A3.
 * IF YOU INCLUDE THIS HEADER FILE FOR A0 - A2, YOU'RE DOING SOMETHING VERY WRONG.
 * YOU SHOULD ONLY INCLUDE: { "network/Server.hpp", "network/Exceptions.hpp" } (and your own custom header files).
 * 
 * SINCERELY,
 * YOUR TUTORS
 */

namespace net
{
class Client final
{
public:
  /**
   * Constructor for student usage.
   * 
   * @throws net::NetworkException if internal network error occurred. 
   * @param name: The player's name.
   * @param port: NOT MEANT TO BE USED FOR STUDENTS.
   * @param ip: NOT MEANT TO BE USED FOR STUDENTS.
   * @param port_retries: NOT MEANT TO BE USED FOR STUDENTS.
   */
  Client(std::string name,
         unsigned short port = 50500,
         const std::string &ip = "127.0.0.1",
         unsigned port_retries = 255);
  
  /**
   * Receives message form server.
   * 
   * @throws net::NetworkException if internal network error occurred (i.e. server disconnected).
   * @throws net::TimeOutException of timer expired (if param `time` was specified).
   * @param time NOT MEANT TO BE USED FROM STUDENTS.
   */
  [[nodiscard]] std::string awaitServerMessage(sf::Time time = sf::Time::Zero);

  /**
   * Sends a message to the server.
   * 
   * @note Students can ignore the return value.
   * @param message The message content that should be send to the server.
   * @return bool The information whether some internal network error occurred when sending the message.
   */
  bool send(const std::string &message);
  
  /**
   * @return The client's assigned id (player_id).
   */
  [[nodiscard]] int getPlayerIndex() const { return id_; }

  /**
   * @return The total amount of players in the game. 
   */
  [[nodiscard]] int getNumberOfPlayers() const { return number_of_players_; }
  
  
  /* ALL OTHER METHODS AND PROPERTIES ARE NOT MEANT TO BE USED FROM STUDENTS */

#pragma region NOT_FOR_STUDENT_USAGE
  Client(std::string name, unsigned short port, const std::string &ip, unsigned port_retries, std::string join_message);
  
  bool send(const void *data, size_t bytes);

  void disconnect();

  bool hasMessageInQueue() const { return !message_queue_.empty(); }

  void clearMessageQueue() noexcept { message_queue_.clear(); }
  
  Client(const Client&) = delete;
  Client(Client&&) = delete;
  Client& operator=(const Client&) = delete;

private:
  void splitAndQueueMessages(std::string content);

private:
  std::string name_;
  int id_;
  int number_of_players_;

  sf::TcpSocket socket_;
  sf::SocketSelector selector_;

  std::vector<std::string> message_queue_;
#pragma endregion
};
} // namespace net
