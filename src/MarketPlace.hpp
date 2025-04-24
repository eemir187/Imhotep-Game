/**
 * MarketPlace.hpp
 
 * This is the MarketPlace class.
 * Is contains everything for MarketPlace.
 *
 * 
 */

#ifndef MARKETPLACE_HPP
#define MARKETPLACE_HPP

#include "ConstructionSite.hpp"
#include "MarketCard.hpp"
#include "network/Server.hpp"
#include "checkValid.hpp"

#include <vector>

class MarketCard;
class Player;

class MarketPlace : public ConstructionSite
{
  private:
    std::vector<MarketCard> market_cards_;

  public:
    /**
     * This is the constructor for our MarketPlace
     *
     * @param ID MarketPlace ID
     * @return void
     */
    MarketPlace(int ID);

    /**
     * This is the destructor for our MarketPlace
     *
     * @param void
     * @return void
     */
    ~MarketPlace();

    /**
     * This is the copy constructor for our MarketPlace
     *
     * @param market_place MarketPlace
     * @return void
     */
    MarketPlace(const MarketPlace &market_place);

    /**
     * Getter for Market Cards
     *
     * @param void
     * @return market_cards_ Market Cards
     */
    std::vector<MarketCard> &getMarketCards();

    /**
     * Setter for Market Cards
     *
     * @param market_card Market Card
     * @return void
     */
    void setMarketCards(MarketCard &market_card);


    /**
     * Add stone
     *
     * @param stone_amount Stone amount
     * @return void
     */
    void addStone(int stone_amount) override;


    /**
     * Request Input from player
     *
     * @param network Network
     * @param player_id Player id
     * @return void
     */
    void RequestInputMarketPlace(net::Server &network, int player_id) const;

    /**
     * Funtion to grant Market Card to player
     *
     * @param network Network
     * @param players Players
     * @param player_id Player id
     * @param market_card Market Card
     */
    int grantMarketCard(net::Server &network, std::vector<Player> &players, int player_id, MarketCard &market_card);

    /**
     * Funtion to take Market Card from MarketPlace
     *
     * @param network Network
     * @param players Players
     * @param player_id Player id
     */
    int takeMarketCard(net::Server &network, std::vector<Player> &players, int player_id);
};

#endif
