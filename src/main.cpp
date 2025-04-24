/**
 * main.cpp
 *
 * This file is the heart of the program.
 * It controls everything else to make the
 * game of Imhotep work.
 * Everything is called through here.
 *
 */

#include "network/Exceptions.hpp"
#include "network/Server.hpp"

#include "Game.hpp"
#include "checkValid.hpp"

#include <cctype>
#include <format>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

enum ERROR_HANDLING
{
  SUCCESS,
  WRONG_ARG,
  WRONG_VAL,
  NET_ERROR,
  RUNTIME_ERROR
};

/**
 * This is main function.
 * Everything is controlled through here.
 *
 * @throws error exceptions
 * @param argc number of arguments
 * @param argv string arguments
 * @return enum from above signaling success or other enum.
 */
int main(int argc, char **argv)
{
  try
  {
    if (argc != 3)
    {
      std::cout << "ERROR: Usage ./imhotep_server PLAYER_COUNT ROUND_COUNT" << std::endl;
      return WRONG_ARG;
    }

    int player_count = 0;
    int round_count = 0;

    try
    {
      player_count = std::stoi(argv[1]);
      round_count = std::stoi(argv[2]);
    }
    catch (const std::invalid_argument &)
    {
      std::cout << "ERROR: Usage ./imhotep_server PLAYER_COUNT ROUND_COUNT" << std::endl;
      return WRONG_VAL;
    }
    catch (const std::out_of_range &)
    {
      std::cout << "ERROR: Usage ./imhotep_server PLAYER_COUNT ROUND_COUNT" << std::endl;
      return WRONG_VAL;
    }

    if (!CheckValid::validPlayer(player_count) || !CheckValid::validRound(round_count))
    {
      std::cout << "ERROR: Usage ./imhotep_server PLAYER_COUNT ROUND_COUNT" << std::endl;
      return WRONG_VAL;
    }

    net::Server network(player_count);

    Game game(player_count, round_count);
    game.run(network);
  }
  catch (const net::NetworkException &e)
  {
    std::cerr << e.what();
    return NET_ERROR;
  }
  catch (const net::TimeoutException &e)
  {
    std::cerr << e.what();
    return NET_ERROR;
  }
  catch (const std::runtime_error &e)
  {
    std::cerr << e.what();
    return RUNTIME_ERROR;
  }
  return SUCCESS;
}

