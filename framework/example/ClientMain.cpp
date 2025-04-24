#include "network/Client.hpp"
#include "network/Exceptions.hpp"

#include <iostream>
#include <string>
#include <thread>
#include <sstream>
#include <tuple>

// Assignment 0 test client

constexpr const char *DEFAULT_IP = "127.0.0.1";

int main(int argc, char **argv)
{
  if (argc != 2 && argc != 3)
  {
    std::cerr << "Usage: [numberOfPlayers] ([IPAddress])" << std::endl;
    return -1;
  }

  int number_of_players = std::stoi(argv[1]);

  std::vector<std::unique_ptr<net::Client>> clients;

  std::cout << "Connecting to server..." << std::endl;
  for (int i = 0; i < number_of_players; i++)
  {
    clients.emplace_back(
      std::make_unique<net::Client>(
        "ExampleClient_" + std::to_string(i),
        50500U,
        (argc == 3 ? std::string(argv[2]) : std::string(DEFAULT_IP)), 
        255U
      )
    );
  }

  std::cout << "All clients connected!" << std::endl;

  bool running = true;

  std::thread th([&](){
      while (running)
      {
        std::cout << "Enter client index and message to send: " << std::flush;
        std::string input_line;
        std::getline(std::cin, input_line);
        std::stringstream sstream(input_line);

        int client_index;
        std::string message;
        sstream >> client_index;
        // Skip any remaining whitespace after the client index
        sstream.ignore(std::numeric_limits<std::streamsize>::max(), ' ');
        std::getline(sstream, message);
        clients[client_index]->send(message + '\n');
      }
  });

  try
  {
    // Skip first messages (server setup)
    // for (auto& c : clients)
    // {
    //   std::ignore = c->awaitServerMessage();
    // }
    // Skip first input
    
    while (running)
    {
      std::string message;
      // Just overwrite since all clients receive the same message anyways
      for (auto& c : clients)
      {
        message = c->awaitServerMessage();
      }

      std::cout << "\nReceived message from server: " << message << std::endl;
      std::cout << "Enter client index and message to send: " << std::flush;

      if (message == "Quit\n")
      {
        std::cout << "Server closed connection!" << std::endl;
        running = false;
      }
    }
  }
  catch (net::NetworkException &e)
  {
    std::cerr << e.what() << std::endl;
    running = false;
  }

  std::cout << "Press Enter to close..." << std::flush;
  th.join();
}
