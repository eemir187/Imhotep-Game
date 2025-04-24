/**
 * checkValid.cpp
 *
 * Checks for the correct input args function logic
 *
 */

#include "checkValid.hpp"
#include <regex>

static const std::regex GET_NEW_STONES_REGEX("(^getnewstones$)", std::regex_constants::icase);
static const std::regex PLACE_STONE_ON_SHIP_REGEX("(^placestoneonship (-?\\d+) (-?\\d+)$)",
                                                  std::regex_constants::icase);
static const std::regex SAIL_SHIP_REGEX("(^sailship (-?\\d+) (-?\\d+)$)", std::regex_constants::icase);
static const std::regex TAKE_MARKET_CARD("(^takemarketcard (-?\\d+)$)", std::regex_constants::icase);
static const std::regex PLAY_BLUE_MARKET_CARD("(^PlayBlueMarketCard (-?\\d+)$)", std::regex_constants::icase);

bool CheckValid::isMalformedInput(const std::string &input)
{
  if (std::regex_match(input, GET_NEW_STONES_REGEX) || std::regex_match(input, PLACE_STONE_ON_SHIP_REGEX) ||
      std::regex_match(input, SAIL_SHIP_REGEX) || std::regex_match(input, TAKE_MARKET_CARD) ||
      std::regex_match(input, PLAY_BLUE_MARKET_CARD))
  {
    return true;
  }
  return false;
}

bool CheckValid::validPlayer(int player_count)
{
  return (player_count >= 2 && player_count <= 4);
}

bool CheckValid::validRound(int round_count)
{
  return (round_count >= 1 && round_count <= 6);
}
