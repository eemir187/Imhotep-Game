/**
 * Ship.cpp
 *
 * This is the Ship class.
 * Everything Ship related is here.
 * This contains the constructor telling how ship is built.
 * Also methods for game logic overall.
 *
 */

#include "Ship.hpp"

int Ship::number_ships_ = 4;

Ship::Ship(int ship_ID, int size) : shipID_(ship_ID), size_(size), cargo_(size_, -1), sailed_(false)
{

}

Ship::~Ship() = default;

Ship::Ship(const Ship &copy_ship) = default;

int Ship::getSize() const
{
  return size_;
}

int Ship::getNumberShips()
{
  return number_ships_;
}

bool Ship::getSailed() const
{
  return sailed_;
}

bool Ship::canSail()
{
  int minimum_load = 0;
  for (int load : cargo_)
  {
    if (load != -1)
    {
      minimum_load++;
    }
  }

  switch (size_)
  {
    case 1:
      return minimum_load >= MINIMUM_LOAD_SIZE_1;
    case 2:
      return minimum_load >= MINIMUM_LOAD_SIZE_2;
    case 3:
      return minimum_load >= MINIMUM_LOAD_SIZE_3;
    case 4:
      return minimum_load >= MINIMUM_LOAD_SIZE_4;
  }

  return false;
}

int Ship::countEmptyFieldShip()
{
  int empty_field = 0;
  for (int load : cargo_)
  {
    if (load == -1)
    {
      empty_field++;
    }
  }

  return empty_field;
}

int Ship::countNonEmptyField()
{
  int non_empty_field = 0;
  for (int load : cargo_)
  {
    if (load != -1)
    {
      non_empty_field++;
    }
  }

  return non_empty_field;
}

bool Ship::isPositionFree(int ship_position)
{
  if (cargo_.at(ship_position) != -1)
  {
    return false;
  }

  return true;
}

void Ship::placeStoneOnShip(int player_id, int ship_position)
{
  if (cargo_.at(ship_position) == -1)
  {
    cargo_.at(ship_position) = player_id;
    return;
  }
}

std::vector<int> Ship::getCargo() const
{
  return cargo_;
}

void Ship::unloadShip(int site_id, std::vector<std::shared_ptr<Obelisk>> &obelisks, Pyramid &pyramid, Temple &temple,
                      BurialChamber &burial_chamber, std::vector<Player> &players)
{
  if (site_id == 1)
  {
    for (int player_id : cargo_)
    {
      if (player_id != -1)
      {
        pyramid.addStoneWithPoints(player_id, players);
      }
    }
  }
  else if (site_id == 4)
  {
    for (int player_id : cargo_)
    {
      if (player_id != -1)
      {
        obelisks.at(player_id)->addStone(1);
      }
    }
  }
  else if (site_id == 2)
  {
    for (int player_id : cargo_)
    {
      if (player_id != -1)
      {
        temple.addStone(player_id);
      }
    }
  }
  else if (site_id == 3)
  {
    for (int player_id : cargo_)
    {
      if (player_id != -1)
      {
        burial_chamber.addStoneWithPlayer(player_id);
      }
    }
  }

  obelisks.at(0)->setBlocked(false);
}

void Ship::emptyShip()
{
  for (int count = 0; count < size_; count++)
  {
    cargo_[count] = -1;
  }
}

void Ship::sailShipBoolean()
{
  sailed_ = true;
}

void Ship::resailShipBoolean()
{
  sailed_ = false;
}

int Ship::getShipID() const
{
  return shipID_;
}

void Ship::unloadShipMarketPlace(net::Server &network, std::vector<Player> &players, MarketPlace &marketplace,
                                 int &card_id, int &player_id)
{
  card_id = -1;
  player_id = -1;

  for (size_t counter = 0; counter < cargo_.size(); counter++)
  {

    if (cargo_[counter] != -1)
    {

      player_id = cargo_[counter];
      card_id = marketplace.takeMarketCard(network, players, player_id);

      cargo_[counter] = -1;

      if (card_id != -1)
      {
        return;
      }
    }
  }
}

void Ship::setCargo(std::vector<int> new_cargo)
{
  cargo_ = new_cargo;
}
