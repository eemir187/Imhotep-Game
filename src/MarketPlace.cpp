/**
 * MarketPlace.cpp
 *
 * This is the MarketPlace class.
 * Is contains everything for MarketPlace.
 *
 */

#include "MarketPlace.hpp"

MarketPlace::MarketPlace(int ID) : ConstructionSite(ID) {}

MarketPlace::~MarketPlace() = default;

MarketPlace::MarketPlace(const MarketPlace &market_place) = default;

std::vector<MarketCard> &MarketPlace::getMarketCards()
{
  return market_cards_;
}

void MarketPlace::setMarketCards(MarketCard &market_card) // puts market cards in vector
{
  market_cards_.push_back(market_card);
}

void MarketPlace::addStone(int /*stone_amount*/)
{
  
}

void MarketPlace::RequestInputMarketPlace(net::Server &network, int player_id) const
{
  std::string message = "RequestInput " + std::to_string(player_id) + " 3 ";

  message += std::to_string(MarketCard::getNumberMarketCards());

  for (int market_card_counter = 0; market_card_counter < MarketCard::getNumberMarketCards(); market_card_counter++)
  {
    message += " " + std::to_string(market_cards_[market_card_counter].getMarketCardID());
  }

  network.sendToAllClients(message);
}

int MarketPlace::grantMarketCard(net::Server &network, std::vector<Player> &players, int player_id,
                                 MarketCard &market_card)
{
  if (market_card.getMarketCardColor() == RED)
  {
    network.sendToAllClients("playedmarketcard " + std::to_string(player_id) + " " +
                             std::to_string(market_card.getMarketCardID()));

    return market_card.getMarketCardID();
  }
  else
  {
    players.at(player_id).setMarketCard(market_card);

    std::string grant_message = "grantmarketcard " + std::to_string(player_id) + " " +
                                std::to_string(market_card.getMarketCardID()) + " " +
                                std::to_string(players.at(player_id).getMarketCards().size());
    for (const auto &card : players.at(player_id).getMarketCards())
    {
      grant_message += " " + std::to_string(card.getMarketCardID());
    }
    network.sendToAllClients(grant_message);

    return -1;
  }
}

int MarketPlace::takeMarketCard(net::Server &network, std::vector<Player> &players, int player_id)
{
  RequestInputMarketPlace(network, player_id);

  while (true)
  {
    net::Server::Message received_msg = network.awaitAnyClientMessage();

    if (received_msg.player_id == player_id)
    {

      if (!CheckValid::isMalformedInput(received_msg.content))
      {
        network.sendToAllClients("MalformedInput " + std::to_string(player_id));
        continue;
      }

      int card = std::stoi(received_msg.content.substr(14));

      if (card < 0 || card >= MarketCard::getNumberMarketCards() || market_cards_[card].getMarketCardID() == -1)
      {
        network.sendToAllClients("InexecutableInput " + std::to_string(player_id));
        continue;
      }

      MarketCard new_card = market_cards_[card];
      market_cards_[card].setMarketCardID(-1);

      std::string update_message = "updateconstructionsite 0 " + std::to_string(MarketCard::getNumberMarketCards());

      for (auto &market_card : market_cards_)
      {
        update_message += " " + std::to_string(market_card.getMarketCardID());
      }
      network.sendToAllClients(update_message);

      return grantMarketCard(network, players, player_id, new_card);

      break;
    }
    else
    {
      network.sendToAllClients("WrongPlayer " + std::to_string(received_msg.player_id));
    }
  }

  return -1;
}
