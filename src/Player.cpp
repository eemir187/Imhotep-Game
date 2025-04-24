/**
 * Player.cpp
 *
 * Player function logics
 *
 */

#include "Player.hpp"

Player::Player(int player_id, const std::string &player_name, int stone_supply, int player_points,
               int recently_added_points) :
    player_id_(player_id), player_name_(player_name), supply_sled_(stone_supply), points_(player_points),
    recently_added_points_(recently_added_points)
{
}

Player::~Player() = default;

Player::Player(const Player &player) = default;

const std::string &Player::getName() const
{
  return player_name_;
}

int Player::getStoneCount() const
{
  return supply_sled_;
}

void Player::removeSingleStone()
{
  supply_sled_ = supply_sled_ - 1;
}

void Player::putStoneToSled()
{
  if (supply_sled_ == MAX_NEW_STONE)
  {
    return;
  }

  supply_sled_ += NEW_STONE;
  if (supply_sled_ > MAX_NEW_STONE)
  {
    supply_sled_ = MAX_NEW_STONE;
  }
}

void Player::addPoints(int points)
{
  points_ += points;
}

int Player::getPoints() const
{
  return points_;
}

int Player::getId() const
{
  return player_id_;
}

void Player::setRecentlyAddedPoints(int points)
{
  recently_added_points_ = points;
}

int Player::getRecentlyAddedPoints() const
{
  return recently_added_points_;
}

void Player::setMarketCard(MarketCard &market_card)
{
  player_market_cards_.push_back(market_card);
}

int Player::giveCardIndexPosition(int card_id)
{
  int cards_amount = player_market_cards_.size();

  for (int count = 0; count < cards_amount; count++)
  {
    if (player_market_cards_.at(count).getMarketCardID() == card_id)
    {
      return count;
    }
  }

  return -1;
}

void Player::deleteCardByIndex(int index)
{
  player_market_cards_.erase(player_market_cards_.begin() + index);
}

std::vector<MarketCard> Player::getMarketCards() const
{
  return player_market_cards_;
}
