/**
 * MarketCard.cpp
 *
 * This is the MarektCard class.
 * Is contains everything for card.
 *
 */

#include "MarketCard.hpp"

int MarketCard::number_market_cards_ = 4;

MarketCard::MarketCard(int card_id) : card_id_(card_id)
{
  if (card_id >= 0 && card_id < 3)
  {
    color_ = RED;
  }
  else if (card_id >= 3 && card_id <= 6)
  {
    color_ = GREEN;
  }
  if (card_id == 7)
  {
    color_ = PURPLE;
  }
  if (card_id > 7 && card_id <= 11)
  {
    color_ = BLUE;
  }
}

MarketCard::~MarketCard() = default;

MarketCard::MarketCard(const MarketCard &market_card) = default;

int MarketCard::getNumberMarketCards()
{
  return number_market_cards_;
}

int MarketCard::getMarketCardID() const
{
  return card_id_;
}

void MarketCard::setMarketCardID(int value)
{
  card_id_ = value;
}

COLOR MarketCard::getMarketCardColor() const
{
  return color_;
}
