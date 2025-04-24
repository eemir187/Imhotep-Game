/**
 * Ship.hpp
 *
 * This is the Ship class.
 * Everything Ship related is here.
 * This contains the constructor telling how ship is built.
 * Also methods for game logic overall.
 *
 * 
 */

#ifndef SHIP_HPP
#define SHIP_HPP

constexpr int MINIMUM_LOAD_SIZE_1 = 1;
constexpr int MINIMUM_LOAD_SIZE_2 = 1;
constexpr int MINIMUM_LOAD_SIZE_3 = 2;
constexpr int MINIMUM_LOAD_SIZE_4 = 3;

#include "network/Server.hpp"

#include "ConstructionSite.hpp"
#include "Pyramid.hpp"
#include "Obelisk.hpp"
#include "BurialChamber.hpp"
#include "MarketPlace.hpp"
#include "Temple.hpp"

#include <array>
#include <string>
#include <vector>
class Obelisk;
class Pyramid;
class MarketPlace;
class Ship
{
  private:
    static int number_ships_;
    int shipID_;
    int size_;
    std::vector<int> cargo_;
    bool sailed_;

  public:
    /**
     * This is the constructor for our ship.
     *
     * @param shipID this will be the id of out ship.
     */
    Ship(int shipID, int size);

    /**
     * This is the destructor for our ship.
     *
     */
    ~Ship();

    /**
     * This is the copy constructor for our ship.
     *
     * @param copy_ship ship which will get copied.
     */
    Ship(const Ship &copy_ship);

    /**
     * This will give us the size of ship.
     *
     * @return size of ship
     */
    int getSize() const;

    /**
     * This will return amount of ships.
     *
     * @return the number of ships
     */
    static int getNumberShips();

    /**
     * Tells if ship sailed or no.
     *
     * @return return boolean whether ship sailed or no.
     */
    bool getSailed() const;

    /**
     * Tell whether ship is acceptable concerning requirement
     *
     * @return requirement accomplished or no.
     */
    bool canSail();

    /**
     * Count our empty fields inside ship.
     *
     * @return amount of empty field in ship
     */
    int countEmptyFieldShip();

    /**
     * Tell us whether position in ship is empty or no.
     *
     * @return boolean whether position in ship is empty or no.
     */
    bool isPositionFree(int ship_position);

    /**
     * Places the Stone on ship to desired position.
     *
     * @param player_id players id.
     * @param ship_position place of ship where stone to be placed.
     * @return void
     */
    void placeStoneOnShip(int player_id, int ship_position);

    /**
     * Gives cargo back.
     *
     * @return ships cargo.
     */
    std::vector<int> getCargo() const;

    /**
     * This places stone from ship to construction site.
     *
     * @param site_id location for stone to be placed.
     * @param obelisks players obelisks
     * @param pyramid our single pyramid
     * @param players our players
     * @return void
     */
    void unloadShip(int site_id, std::vector<std::shared_ptr<Obelisk>> &obelisks, Pyramid &pyramid, Temple &temple,
                    BurialChamber &burial_chamber, std::vector<Player> &players);

    /**
     * empty our ship.
     *
     * @return void
     */
    void emptyShip();

    /**
     * This sets ship to sail.
     *
     * @return void
     */
    void sailShipBoolean();

    /**
     * This sets ship to not sail.
     *
     * @return void
     */
    void resailShipBoolean();

    /**
     * Gives us the ships id
     *
     * @return the ship id
     */
    int getShipID() const;

    /**
     * Gives us number of non empty fields
     * 
     * @param void
     * @return non_empty_field Number of non empty fields
     */
    int countNonEmptyField();

    /**
     * Unload ship MarketPlace
     * 
     * @param network Network
     * @param players Players
     * @param marketplace MarketPlace
     * @param card_id Card ID
     * @param player_id Player ID
     * @return void
     */
    void unloadShipMarketPlace(net::Server &network, std::vector<Player> &players, MarketPlace &marketplace,
                               int &card_id, int &player_id);

    /**
     * Setter for cargo
     *  
     * @param new_cargo New cargo
     * @return void
     */
    void setCargo(std::vector<int> new_cargo);
};

#endif
