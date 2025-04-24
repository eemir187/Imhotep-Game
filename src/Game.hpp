/**
 * Game.hpp
 *
 * This is the Game class.
 * Is contains everything for game.
 *
 */

#ifndef GAME_HPP
#define GAME_HPP

#include "ConstructionSite.hpp"
#include "Pyramid.hpp"
#include "Obelisk.hpp"
#include "Temple.hpp"
#include "BurialChamber.hpp"
#include "Player.hpp"
#include "Ship.hpp"
#include "checkValid.hpp"
#include "network/Server.hpp"

#include "random/Random.hpp"
#include "MarketPlace.hpp"
#include "MarketCard.hpp"

#include <array>
#include <cmath>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

enum class Sites
{
  OBELISK = 4,
  PYRAMID = 1,
  MARKETPLACE = 0,
  BURRIALCHAMBER = 3,
  TEMPLE = 2
};

class Game
{
  private:
    int player_count_;
    int round_count_;
    std::vector<Player> players_;
    std::vector<std::shared_ptr<Obelisk>> obelisks_;
    std::vector<std::shared_ptr<Ship>> ships_;
    std::shared_ptr<Pyramid> pyramid_;
    std::shared_ptr<MarketPlace> market_place;
    // std::vector<MarketCard> market_cards_;
    std::shared_ptr<BurialChamber> burial_chamber_;
    std::shared_ptr<Temple> temple_;

  public:
    /**
     * This is the constructor for our game.
     *
     * @param player_count amount of player.
     */
    Game(int player_count, int round_count);

    /**
     * This is the destructor for our game.
     *
     */
    ~Game();

    /**
     * This is the copy constructor for our game.
     *
     * @param copy_ship game which will get copied.
     */
    Game(const Game &copy_game);

    /**
     * Start Game Message
     *
     * @param network Server network
     * @return void
     */
    void startGameMessage(net::Server &network);

    /**
     * Start Game Message
     *
     * @param network Server network
     * @return void
     */
    void initializeGame(net::Server &network);

    /**
     * action for getting new stones
     *
     * @param current_player Current player
     * @return logic value If the player can take new stones or not
     */
    bool actionGetNewStones(int current_player);

    /**
     * sents update stones message
     *
     * @param network Server network
     * @param stone_difference Stone difference
     * @param current player Current player
     * @param takeout_sled bool which checks if get newstones or placestoneonship
     * @return void
     */
    void sendUpdateStones(net::Server &network, int stone_difference, int current_player, bool takeout_sled);

    /**
     * action for placing stone on ship
     *
     * @param current_player Current player
     * @param target_ship_id Target ship id
     * @param position_for_ship Position on the ship
     * @return void
     */
    bool actionPlaceStoneOnShip(int current_player, int target_ship_id, int position_for_ship) const;

    /**
     * sends update ship message
     *
     * @param network Server network
     * @param target_ship_id Target ship id
     * @return void
     */
    void sendUpdateShip(net::Server &network, int target_ship_id) const;

    /**
     * sends update constructuon site message
     *
     * @param network Server network
     * @param site_id Site Id
     * @return void
     */
    void UpdateConstructionSite(net::Server &network, int site_id) const;

    /**
     * sempties all ship cargo
     *
     * @param void Site Id
     * @return void
     */
    void emptyAllShips();

    /**
     * Action to sail ship
     *
     * @param target_ship_id Numerator of the division
     * @param site_id Site id
     * @param network our network instance
     * @return bool whether inexectuable input or not.
     */
    bool actionSailShip(int target_ship_id, int site_id, net::Server &network);

    /**
     * This will convert our input to lower case.
     *
     * @param user_input our user input
     * @return to lower case converted input
     */
    static std::string convertInputToLowerCase(const std::string &user_input);

    /**
     * This currently does everything action related.
     * Getnewstones, PlaceStoneOnShip und SailShip.
     * It also calls methods for the message that have to be sent after
     * every action.
     *
     * @param input our received input
     * @param network our network instance
     * @param current_player the current player
     * @return bool whether inexecutable input or no.
     */
    bool processPlayerCommandAction(const std::string &input_raw, int current_player, net::Server &network);

    /**
     * This handles and checks the input
     * for the player.
     *
     * @param network our network instance
     * @param current_player the current player
     * @return void
     */
    void handlePlayerInput(net::Server &network, int current_player);

    /**
     * This constructs and send the round
     * message.
     *
     * @param network our network instance
     * @return void
     */
    void playRoundMessage(net::Server &network);

    /**
     * This will set all ships to
     * they are not sailed.
     *
     * @return void
     */
    void resailAllShips();

    /**
     * This is the play logic.
     * It controls round and turns.
     *
     * @param network our network instance
     * @return void
     */
    void playRounds(net::Server &network);

    /**
     * This calculates the points for the obelisks.
     *
     * @return void
     */
    void calculateObeliskScores();

    /**
     * This calculates the points for the MarketCards.
     *
     * @param network our network instance
     * @return void
     */
    void calculateMarketCardsPoints(std::vector<Player> &players);

    /**
     * This calculates the points for the purple cards.
     *
     * @param player the player
     * @param purple_card_counter the purple card counter
     * @return void
     */
    void calculatePurpleCardPoints(Player &player, int purple_card_counter);

    /**
     * This calculates the points for the green cards.
     *
     * @param player the player
     * @param card Card
     * @return void
     */
    void calculateGreenCardPoints(Player &player, MarketCard card);

    /**
     * This handles the red card
     *
     * @param card_id Card ID
     * @param player_id Player ID
     * @param network network
     * @return void
     */
    void handleRedCardAction(int card_id, int player_id, net::Server &network);

    /**
     * This handles the blue card chisel
     *
     * @param network network
     * @param player_id player ID
     * @return void
     */
    void handleBlueCardChisel(net::Server &network, int player_id);

    /**
     * This handles the blue card lever
     *
     * @param network network
     * @param player_id player ID
     * @return void
     */
    void handleBlueCardLever(net::Server &network, int player_id);

    /**
     * This handles the blue card hammer
     *
     * @param network network
     * @param player_id player ID
     * @return void
     */
    void handleBlueCardHammer(net::Server &network, int player_id);

    /**
     * This handles the blue card sail
     *
     * @param network network
     * @param player_id player ID
     * @return void
     */
    void handleBlueCardSail(net::Server &network, int player_id);

    /**
     * This controls everything from the game.
     *
     * @param network our network instance
     * @return void
     */
    void run(net::Server &network);
};

#endif
