/**
 * checkValid.hpp
 *
 * Checks for the correct input args function classes and methods
 *
 */

#ifndef CHECKVALID_HPP
#define CHECKVALID_HPP

#include <regex>
#include <string>

class CheckValid
{
  public:
    /**
     * Validates the player count.
     *
     * @param player_count Number of players
     * @return True if valid, false otherwise
     */
    static bool validPlayer(int player_count);

    /**
     * Validates the round count.
     *
     * @param round_count Number of rounds
     * @return True if valid, false otherwise
     */
    static bool validRound(int round_count);

    /**
     * Validates player input using regex.
     *
     * @param input The player's input string
     * @return True if valid, false otherwise
     */
    static bool isMalformedInput(const std::string &input);
};

#endif
