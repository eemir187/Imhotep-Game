/**
 * Player.hpp
 *
 * Player class and methods
 *
 */

#ifndef PLAYER_HPP
#define PLAYER_HPP

#define MAX_NEW_STONE 5
#define NEW_STONE 3

#include "MarketCard.hpp"
#include <string>
#include <vector>

class MarketCard;

class Player
{
  private:
    int player_id_;
    std::string player_name_;
    int supply_sled_;
    int points_;
    int recently_added_points_;
    std::vector<MarketCard> player_market_cards_;

  public:
    /**
     * Player Constructor
     *
     * @param player_id Player ID
     * @param player_name Player name
     * @param stone_supply Player's supply
     * @param player_points Player points
     * @param recently_added_points Player's recently added points
     *
     */
    Player(int player_id, const std::string &player_name, int stone_supply, int player_points,
           int recently_added_points);

    /**
     * Player Destructor
     *
     */
    ~Player();

    /**
     * Player Copy Constructor
     *
     * @param player Player object
     *
     */
    Player(const Player &player);

    /**
     * Getter for id
     *
     * @param void
     * @return player_id_ Player id
     *
     */
    int getId() const;

    /**
     * Getter for name
     *
     * @param void
     * @return player_name_ Player name
     *
     */
    const std::string &getName() const;

    /**
     * Getter for Player supply sled
     *
     * @param void
     * @return supply_sled_ Player supply sled
     *
     */
    int getStoneCount() const;

    /**
     * Removes Single Stone
     *
     * @param void
     * @return void
     *
     */
    void removeSingleStone();

    /**
     * Puts stone to Sled
     *
     * @param void
     * @return void
     *
     */
    void putStoneToSled();

    int giveCardIndexPosition(int card_id);

    void deleteCardByIndex(int index);

    /**
     * Adds Points to player
     *
     * @param points Number of points
     * @return void
     *
     */
    void addPoints(int points);

    /**
     * Getter for Player points
     *
     * @param void
     * @return points_ PLayer points
     *
     */
    int getPoints() const;

    /**
     * Sets recently added points
     *
     * @param points Number of points
     * @return void
     *
     */
    void setRecentlyAddedPoints(int points);

    /**
     * Gets Recently added points
     *
     * @param void
     * @return recently_added_points_ Player's recently added points
     *
     */
    int getRecentlyAddedPoints() const;

    void setMarketCard(MarketCard &market_card);

    std::vector<MarketCard> getMarketCards() const;
};

#endif
