/**
 * Game.cpp
 *
 * This is the Game class.
 * Is contains everything for game.
 *
 */

#include "Game.hpp"

Game::Game(int player_count, int round_count) : player_count_(player_count), round_count_(round_count) {}

Game::~Game()
{
  obelisks_.clear();
  ships_.clear();
  players_.clear();
}

Game::Game(const Game &copy_game) = default;

void Game::startGameMessage(net::Server &network)
{
  std::string start_message = "startgame ";

  start_message += std::to_string(player_count_) + " ";

  for (int player = 0; player < player_count_; player++)
  {
    start_message += std::to_string(players_[player].getStoneCount()) + " ";
  }

  start_message += std::to_string(player_count_);

  for (int player = 0; player < player_count_; player++)
  {
    start_message += " " + players_[player].getName();
  }

  network.sendToAllClients(start_message);
}


void Game::initializeGame(net::Server &network)
{

  for (int init_counter = 0; init_counter < player_count_; init_counter++)
  {
    std::string name = network.getPlayerName(init_counter);
    int stone_supply = 2 + init_counter;

    Player newPlayer(init_counter, name, stone_supply, 0, 0);

    players_.push_back(newPlayer);

    auto newObelisk = std::make_shared<Obelisk>(4, init_counter);
    obelisks_.push_back(newObelisk);
  }

  Pyramid newPyramid(0);
  pyramid_ = std::make_shared<Pyramid>(newPyramid);
  pyramid_->initialize();

  temple_ = std::make_shared<Temple>(2, player_count_);
  temple_->initialize();

  burial_chamber_ = std::make_shared<BurialChamber>(3);

  market_place = std::make_shared<MarketPlace>(0);

  startGameMessage(network);
}

bool Game::actionGetNewStones(int current_player)
{
  if (players_.at(current_player).getStoneCount() == 5)
  {
    return false;
  }

  players_.at(current_player).putStoneToSled();

  return true;
}

void Game::sendUpdateStones(net::Server &network, int stone_difference, int current_player, bool takeout_sled)
{
  if (takeout_sled == true)
  {
    players_.at(current_player).removeSingleStone();
  }

  int current_stone_amount = players_.at(current_player).getStoneCount();

  std::string update_stone_message = "updatestones ";
  update_stone_message += std::to_string(current_player) + " ";
  update_stone_message += std::to_string(stone_difference) + " ";
  update_stone_message += std::to_string(current_stone_amount);

  network.sendToAllClients(update_stone_message);
}

bool Game::actionPlaceStoneOnShip(int current_player, int target_ship_id, int position_for_ship) const
{
  bool ship_found = false;
  for (const auto &ship : ships_)
  {
    if (target_ship_id == ship->getShipID())
    {
      ship_found = true;
    }
  }

  if (ship_found == false)
  {
    return false;
  }

  if (position_for_ship >= ships_.at(target_ship_id)->getSize())
  {
    return false;
  }

  if (ships_.at(target_ship_id)->isPositionFree(position_for_ship) == false)
  {
    return false;
  }

  int free_ship_spaces = ships_.at(target_ship_id)->countEmptyFieldShip();
  int current_stone_amount = players_.at(current_player).getStoneCount();
  bool current_sail = ships_.at(target_ship_id)->getSailed();

  if (current_stone_amount == 0 || free_ship_spaces == 0 || current_sail == true)
  {
    return false;
  }

  ships_.at(target_ship_id)->placeStoneOnShip(current_player, position_for_ship);

  return true;
}

void Game::sendUpdateShip(net::Server &network, int target_ship_id) const
{
  std::string update_ship_message = "updateship ";
  update_ship_message += std::to_string(target_ship_id);
  update_ship_message += " ";
  update_ship_message += std::to_string(ships_[target_ship_id]->getSize());

  for (int count_through = 0; count_through < ships_[target_ship_id]->getSize(); count_through++)
  {
    update_ship_message += " " + std::to_string(ships_.at(target_ship_id)->getCargo().at(count_through));
  }

  network.sendToAllClients(update_ship_message);
}

void Game::UpdateConstructionSite(net::Server &network, int site_id) const
{

  std::string update_construction_message = "updateconstructionsite " + std::to_string(site_id);

  if (site_id == static_cast<int>(Sites::OBELISK))
  {
    update_construction_message =
        update_construction_message + " " + std::to_string(static_cast<int>(obelisks_.size()));
    for (int count_through = 0; count_through < static_cast<int>(obelisks_.size()); count_through++)
    {
      update_construction_message += " " + std::to_string(obelisks_.at(count_through)->getStones());
    }
  }
  else if (site_id == static_cast<int>(Sites::PYRAMID))
  {
    update_construction_message = update_construction_message + " " + std::to_string(pyramid_->getStonesPlaced());

    for (int colm = 0; colm < 3; colm++)
    {
      for (int row = 0; row < 3; row++)
      {
        if (pyramid_->getFirstLevel()[row][colm] != EMPTY)
        {
          update_construction_message += " " + std::to_string(pyramid_->getFirstLevel()[row][colm]);
        }
      }
    }

    for (int colm_second_level = 0; colm_second_level < 2; colm_second_level++)
    {
      for (int row_second_level = 0; row_second_level < 2; row_second_level++)
      {
        if (pyramid_->getSecondLevel()[row_second_level][colm_second_level] != EMPTY)
        {
          update_construction_message +=
              " " + std::to_string(pyramid_->getSecondLevel()[row_second_level][colm_second_level]);
        }
      }
    }

    if (pyramid_->getThirdLevel() != EMPTY)
    {
      update_construction_message += " " + std::to_string(pyramid_->getThirdLevel());
    }

    for (int stone : pyramid_->getBonusLevelStones())
    {
      update_construction_message += " " + std::to_string(stone);
    }
  }
  else if (site_id == static_cast<int>(Sites::TEMPLE))
  {
    const auto &levels = temple_->getTempleLevels();
    int total_stones = 0;
    for (const auto &row : levels)
    {
      for (int stone : row)
      {
        if (stone != -1)
        {
          total_stones++;
        }
      }
    }

    update_construction_message += " " + std::to_string(total_stones);
    for (const auto &row : levels)
    {
      for (int stone : row)
      {
        if (stone != -1)
        {
          update_construction_message += " " + std::to_string(stone);
        }
      }
    }
  }
  else if (site_id == static_cast<int>(Sites::BURRIALCHAMBER))
  {
    update_construction_message += " " + std::to_string(burial_chamber_->getStones());
    int number_of_columns = static_cast<int>(burial_chamber_->getBuriedMatrix().at(0).size());

    for (int col = 0; col < number_of_columns; col++)
    {
      for (int row = 0; row < 3; row++)
      {
        int stone = burial_chamber_->getBuriedMatrix().at(row).at(col);
        if (stone != -1)
        {
          update_construction_message += " " + std::to_string(stone);
        }
      }
    }
  }

  network.sendToAllClients(update_construction_message);
}

void Game::emptyAllShips()
{
  for (const auto &ship : ships_)
  {
    ship->emptyShip();
  }
}

bool Game::actionSailShip(int target_ship_id, int site_id, net::Server &network)
{
  if (site_id != static_cast<int>(Sites::OBELISK) && site_id != static_cast<int>(Sites::PYRAMID) &&
      site_id != static_cast<int>(Sites::TEMPLE) && site_id != static_cast<int>(Sites::BURRIALCHAMBER) &&
      site_id != static_cast<int>(Sites::MARKETPLACE))
  {
    return false;
  }

  bool ship_found = false;
  for (const auto &ship : ships_)
  {
    if (target_ship_id == ship->getShipID())
    {
      ship_found = true;
    }
  }

  if (ship_found == false)
  {
    return false;
  }

  if (ships_.at(target_ship_id)->canSail() == false)
  {
    return false;
  }

  bool sailed_boolean = ships_.at(target_ship_id)->getSailed();
  std::shared_ptr<ConstructionSite> our_site = nullptr;

  if (site_id == static_cast<int>(Sites::OBELISK))
  {
    our_site = std::static_pointer_cast<ConstructionSite>(obelisks_.at(0));
  }
  else if (site_id == static_cast<int>(Sites::PYRAMID))
  {
    our_site = std::static_pointer_cast<ConstructionSite>(pyramid_);
  }
  else if (site_id == static_cast<int>(Sites::TEMPLE))
  {
    our_site = std::static_pointer_cast<ConstructionSite>(temple_);
  }
  else if (site_id == static_cast<int>(Sites::BURRIALCHAMBER))
  {
    our_site = std::static_pointer_cast<ConstructionSite>(burial_chamber_);
  }

  if (site_id == static_cast<int>(Sites::MARKETPLACE))
  {
    int card_id = -1;
    int player_id = -1;

    if (sailed_boolean == true)
    {
      return false;
    }

    network.sendToAllClients("sailedship " + std::to_string(target_ship_id) + " " + std::to_string(site_id));
    ships_.at(target_ship_id)->sailShipBoolean();

    while (true)
    {
      ships_.at(target_ship_id)->unloadShipMarketPlace(network, players_, *market_place, card_id, player_id);

      if (card_id != -1 && player_id != -1)
      {
        handleRedCardAction(card_id, player_id, network);
      }

      bool stones_remaining = false;

      if (ships_.at(target_ship_id)->countNonEmptyField() >= 1)
      {
        stones_remaining = true;
      }

      if (!stones_remaining)
      {
        break;
      }
    }
  }

  if (site_id != static_cast<int>(Sites::MARKETPLACE))
  {
    if (sailed_boolean == true || our_site->isBlocked() == true)
    {
      return false;
    }
    ships_.at(target_ship_id)->sailShipBoolean();
    ships_.at(target_ship_id)->unloadShip(site_id, obelisks_, *pyramid_, *temple_, *burial_chamber_, players_);
  }

  if (site_id != static_cast<int>(Sites::MARKETPLACE))
  {
    network.sendToAllClients("sailedship " + std::to_string(target_ship_id) + " " + std::to_string(site_id));
    UpdateConstructionSite(network, site_id);
  }

  if (site_id == static_cast<int>(Sites::PYRAMID))
  {
    for (int player_id = 0; player_id < player_count_; player_id++)
    {
      if (players_.at(player_id).getRecentlyAddedPoints() == 0)
      {
        continue;
      }

      network.sendToAllClients("grantpoints " + std::to_string(player_id) + " " +
                               std::to_string(players_.at(player_id).getRecentlyAddedPoints()) + " " +
                               std::to_string(players_.at(player_id).getPoints()));
      players_.at(player_id).setRecentlyAddedPoints(0);
    }
    for (const auto &ship : ships_)
    {
      if (ship->getSailed() == false)
      {
        return true;
      }
    }
  }

  return true;
}

std::string Game::convertInputToLowerCase(const std::string &user_input)
{
  std::string result = user_input;
  std::transform(result.begin(),
                 result.end(),
                 result.begin(),
                 [](unsigned char current_character) { return std::tolower(current_character); });
  return result;
}

void Game::handleBlueCardChisel(net::Server &network, int player_id)
{
  network.sendToAllClients("playedmarketcard " + std::to_string(player_id) + " 11");

  for (int counter = 0; counter < 2; counter++)
  {
    network.sendToAllClients("RequestInput " + std::to_string(player_id) + " 2");
    net::Server::Message received_msg = network.awaitAnyClientMessage();
    processPlayerCommandAction(received_msg.content, player_id, network);
  }
}

void Game::handleBlueCardLever(net::Server &network, int player_id)
{
  network.sendToAllClients("playedmarketcard " + std::to_string(player_id) + " 8");
  network.sendToAllClients("RequestInput " + std::to_string(player_id) + " 4");
  net::Server::Message received_msg = network.awaitAnyClientMessage();
  std::string input = received_msg.content;
  int ship_id = std::stoi(input.substr(18, 1));
  int site_id = std::stoi(input.substr(20, 1));
  std::vector<int> new_cargo;
  int temp = 0;

  for (int counter = 0; counter < ships_.at(ship_id)->getSize(); counter++)
  {
    temp = std::stoi(input.substr(22 + (counter * 2), 1));

    if (temp < ships_.at(ship_id)->getSize())
    {
      new_cargo.push_back(ships_.at(ship_id)->getCargo().at(temp));
    }
  }

  ships_.at(ship_id)->setCargo(new_cargo);
  actionSailShip(ship_id, site_id, network);
}

void Game::handleBlueCardHammer(net::Server &network, int player_id)
{
  network.sendToAllClients("playedmarketcard " + std::to_string(player_id) + " 9");

  int current_stone_amount = players_.at(player_id).getStoneCount();
  actionGetNewStones(player_id);
  int after_stone_amount = players_.at(player_id).getStoneCount();
  int stone_difference = after_stone_amount - current_stone_amount;
  sendUpdateStones(network, stone_difference, player_id, false);

  network.sendToAllClients("RequestInput " + std::to_string(player_id) + " 2");

  net::Server::Message received_msg = network.awaitAnyClientMessage();
  processPlayerCommandAction(received_msg.content, player_id, network);
}

void Game::handleBlueCardSail(net::Server &network, int player_id)
{
  network.sendToAllClients("playedmarketcard " + std::to_string(player_id) + " 10");

  network.sendToAllClients("RequestInput " + std::to_string(player_id) + " 2");
  net::Server::Message received_msg = network.awaitAnyClientMessage();
  std::string input = received_msg.content;
  processPlayerCommandAction(input, player_id, network);

  network.sendToAllClients("RequestInput " + std::to_string(player_id) + " 1");
  received_msg = network.awaitAnyClientMessage();
  input = received_msg.content;
  processPlayerCommandAction(input, player_id, network);
}

bool Game::processPlayerCommandAction(const std::string &input_raw, int current_player, net::Server &network)
{
  std::string input = convertInputToLowerCase(input_raw);
  if (input.find("getnewstones") == 0)
  {
    int current_stone_amount = players_.at(current_player).getStoneCount();

    if (actionGetNewStones(current_player) == false)
    {
      return false;
    }

    int after_stone_amount = players_.at(current_player).getStoneCount();
    int stone_difference = after_stone_amount - current_stone_amount;

    sendUpdateStones(network, stone_difference, current_player, false);
  }

  else if (input.find("placestoneonship") == 0)
  {
    int target_ship_id = std::stoi(input.substr(17, 1));
    int position_for_ship = std::stoi(input.substr(19, 1));

    if (actionPlaceStoneOnShip(current_player, target_ship_id, position_for_ship) == false)
    {
      return false;
    }

    sendUpdateStones(network, -1, current_player, true);

    sendUpdateShip(network, target_ship_id);
  }

  else if (input.find("sailship") == 0)
  {
    std::regex sail_ship_regex("(^sailship (-?\\d+) (-?\\d+)$)", std::regex_constants::icase);
    std::smatch args_match;
    std::regex_match(input, args_match, sail_ship_regex);

    int target_ship_id = std::stoi(input.substr(9, 1));
    int site_id = std::stoi(input.substr(11, 1));

    if (actionSailShip(target_ship_id, site_id, network) == false)
    {
      return false;
    }
  }

  else if (input.find("playbluemarketcard") == 0)
  {
    int card_id = std::stoi(input.substr(18));
    int card_position_index = players_.at(current_player).giveCardIndexPosition(card_id);
    if (card_position_index == -1)
    {
      return false;
    }

    if (card_id == 11)
    {
      handleBlueCardChisel(network, current_player);
    }
    else if (card_id == 8)
    {
      handleBlueCardLever(network, current_player);
    }
    else if (card_id == 9)
    {
      handleBlueCardHammer(network, current_player);
    }
    else if (card_id == 10)
    {
      handleBlueCardSail(network, current_player);
    }
    players_.at(current_player).deleteCardByIndex(card_position_index);
  }

  return true;
}

void Game::handlePlayerInput(net::Server &network, int current_player)
{
  while (true)
  {
    net::Server::Message received_msg = network.awaitAnyClientMessage();

    if (received_msg.player_id == current_player)
    {
      if (!CheckValid::isMalformedInput(received_msg.content))
      {
        network.sendToAllClients("MalformedInput " + std::to_string(current_player));
        network.sendToAllClients("RequestInput " + std::to_string(current_player) + " 0");
      }
      else
      {
        bool inexecutable = processPlayerCommandAction(received_msg.content, current_player, network);
        if (inexecutable == false)
        {
          network.sendToAllClients("InexecutableInput " + std::to_string(current_player));
          network.sendToAllClients("RequestInput " + std::to_string(current_player) + " 0");
        }
        else
        {
          break;
        }
      }
    }
    else
    {
      network.sendToAllClients("WrongPlayer " + std::to_string(received_msg.player_id));
    }
  }
}

void Game::playRoundMessage(net::Server &network)
{

  std::string message = "StartRound " + std::to_string(Ship::getNumberShips()) + " ";
  for (int ship_counter = 0; ship_counter < Ship::getNumberShips(); ship_counter++)
  {
    message += std::to_string(ships_[ship_counter]->getSize()) + " ";
  }

  message += std::to_string(MarketCard::getNumberMarketCards());

  std::vector<MarketCard> market_cards = market_place->getMarketCards();

  for (int market_card_counter = 0; market_card_counter < MarketCard::getNumberMarketCards(); market_card_counter++)
  {
    message += " " + std::to_string(market_cards[market_card_counter].getMarketCardID());
  }

  network.sendToAllClients(message);
}

void Game::resailAllShips()
{
  for (const auto &ship : ships_)
  {
    ship->resailShipBoolean();
  }
}

void Game::playRounds(net::Server &network)
{
  int start_player_ID = 0;

  int current_player = start_player_ID;

  for (int round = 0; round < round_count_; round++)
  {
    ships_.clear();

    for (int ship = 0; ship < Ship::getNumberShips(); ship++)
    {
      int ship_size = Random::getInstance().getNextShipSize();
      std::shared_ptr<Ship> newShip = std::make_shared<Ship>(ship, ship_size);
      ships_.push_back(newShip);
    }

    market_place->getMarketCards().clear();

    for (int market = 0; market < MarketCard::getNumberMarketCards(); market++)
    {
      int market_card_id = Random::getInstance().getNextMarketCard();
      MarketCard market_card(market_card_id);
      market_place->setMarketCards(market_card);
    }
    int ships_sailed = 0;
    std::vector<int> sailed_ships;

    playRoundMessage(network);

    while (ships_sailed != Ship::getNumberShips())
    {
      network.sendToAllClients("StartTurn " + std::to_string(current_player));
      network.sendToAllClients("RequestInput " + std::to_string(current_player) + " 0");
      handlePlayerInput(network, current_player);
      network.sendToAllClients("EndTurn " + std::to_string(current_player));
      for (const auto &ship : ships_)
      {
        if (ship->getSailed() &&
            std::find(sailed_ships.begin(), sailed_ships.end(), ship->getShipID()) == sailed_ships.end())
        {
          ships_sailed++;
          sailed_ships.push_back(ship->getShipID());
        }
      }

      if (current_player == player_count_ - 1)
      {
        current_player = 0;
      }
      else
      {
        current_player++;
      }
    }

    temple_->scoreEndOfRound(players_);
    for (int player_id = 0; player_id < player_count_; player_id++)
    {
      if (players_.at(player_id).getRecentlyAddedPoints() == 0)
      {
        continue;
      }

      network.sendToAllClients("grantpoints " + std::to_string(player_id) + " " +
                               std::to_string(players_.at(player_id).getRecentlyAddedPoints()) + " " +
                               std::to_string(players_.at(player_id).getPoints()));
      players_.at(player_id).setRecentlyAddedPoints(0);
    }

    network.sendToAllClients("EndRound");

    resailAllShips();

    start_player_ID = (start_player_ID + 1) % player_count_;
  }
}

void Game::calculateObeliskScores()
{
  int player_counter = static_cast<int>(players_.size());
  std::vector<std::pair<int, int>> player_heights;

  for (int count = 0; count < player_counter; count++)
  {
    int height = obelisks_[count]->getStones();
    int p_id = obelisks_[count]->getPlayerId();
    player_heights.push_back({p_id, height});
  }

  std::sort(player_heights.begin(), player_heights.end(), [](auto &a, auto &b) { return a.second > b.second; });

  std::vector<int> rank_points;
  switch (player_counter)
  {
    case 2:
      rank_points = {10, 1};
      break;
    case 3:
      rank_points = {12, 6, 1};
      break;
    case 4:
      rank_points = {15, 10, 5, 1};
      break;
    default:
      break;
  }

  int index = 0;
  while (index < player_counter)
  {
    int current_height = player_heights[index].second;
    if (current_height == 0)
    {
      break;
    }

    int tie_count = 1;
    int sum_points = rank_points[index];
    while (index + tie_count < player_counter && player_heights[index + tie_count].second == current_height)
    {
      sum_points += rank_points[index + tie_count];
      tie_count++;
    }

    int awarded_points = sum_points / tie_count;

    for (int counter = 0; counter < tie_count; counter++)
    {
      int p_id = player_heights[index + counter].first;
      players_[p_id].setRecentlyAddedPoints(awarded_points);
      players_[p_id].addPoints(awarded_points);
    }

    index += tie_count;
  }
}

void Game::calculatePurpleCardPoints(Player &player, int purple_card_counter)
{
  if (purple_card_counter == 0)
  {
    return;
  }

  int points = 0;
  switch (purple_card_counter)
  {
    case 1:
      points = 1;
      break;
    case 2:
      points = 3;
      break;
    case 3:
      points = 6;
      break;
    case 4:
      points = 10;
      break;
    case 5:
      points = 15;
      break;
  }

  if (purple_card_counter > 5)
  {
    points = 15;
    for (int count = 5; count < purple_card_counter; count++)
    {
      points += 2;
    }
  }

  player.addPoints(points);
  player.setRecentlyAddedPoints(points + player.getRecentlyAddedPoints());
}

void Game::calculateGreenCardPoints(Player &player, MarketCard card)
{
  int card_id = card.getMarketCardID();
  int new_points = 0;
  int obelisk_stones_amount = 0;
  int total_temple_stones = 0;

  if (card_id == 3)
  {
    new_points = (pyramid_->getStonesPlaced()) / 3;
  }

  else if (card_id == 4)
  {
    const auto &levels = temple_->getTempleLevels();
    for (const auto &row : levels)
    {
      for (int stone : row)
      {
        if (stone != EMPTY)
        {
          total_temple_stones++;
        }
      }
    }

    new_points = total_temple_stones / 3;
  }

  else if (card_id == 5)
  {
    new_points = (burial_chamber_->getStones()) / 3;
  }

  else if (card_id == 6)
  {
    for (auto obelisk : obelisks_)
    {
      obelisk_stones_amount += obelisk->getStones();
    }
    new_points = obelisk_stones_amount / 3;
  }

  player.addPoints(new_points);
  player.setRecentlyAddedPoints(new_points + player.getRecentlyAddedPoints());
}

void Game::calculateMarketCardsPoints(std::vector<Player> &players)
{
  int purple_card_counter = 0;
  int blue_card_counter = 0;

  for (auto &player : players)
  {
    for (const auto &card : player.getMarketCards())
    {
      if (card.getMarketCardColor() == PURPLE)
      {
        purple_card_counter++;
      }
      else if (card.getMarketCardColor() == GREEN)
      {
        calculateGreenCardPoints(player, card);
      }
      else if (card.getMarketCardColor() == BLUE)
      {
        blue_card_counter++;
      }
    }

    calculatePurpleCardPoints(player, purple_card_counter);

    player.addPoints(blue_card_counter);
    player.setRecentlyAddedPoints(player.getRecentlyAddedPoints() + blue_card_counter);
    purple_card_counter = 0;
    blue_card_counter = 0;
  }
}

void Game::handleRedCardAction(int card_id, int player_id, net::Server &network)
{
  if (card_id == 0)
  {
    pyramid_->addStoneWithPoints(player_id, players_);
    UpdateConstructionSite(network, 1);

    for (int id = 0; id < player_count_; id++)
    {
      if (players_.at(id).getRecentlyAddedPoints() == 0)
      {
        continue;
      }

      network.sendToAllClients("grantpoints " + std::to_string(id) + " " +
                               std::to_string(players_.at(id).getRecentlyAddedPoints()) + " " +
                               std::to_string(players_.at(id).getPoints()));
      players_.at(id).setRecentlyAddedPoints(0);
    }
  }

  else if (card_id == 1)
  {
    burial_chamber_->addStoneWithPlayer(player_id);
    UpdateConstructionSite(network, 3);
  }
  else if (card_id == 2)
  {
    obelisks_.at(player_id)->addStone(1);
    UpdateConstructionSite(network, 4);
  }
}

void Game::run(net::Server &network)
{
  initializeGame(network);
  playRounds(network);

  calculateObeliskScores();
  burial_chamber_->calcPoints(players_);

  calculateMarketCardsPoints(players_);

  for (int player_id = 0; player_id < player_count_; player_id++)
  {
    int recent = players_[player_id].getRecentlyAddedPoints();

    if (recent > 0)
    {
      network.sendToAllClients("grantpoints " + std::to_string(player_id) + " " + std::to_string(recent) + " " +
                               std::to_string(players_[player_id].getPoints()));
      players_[player_id].setRecentlyAddedPoints(0);
    }
  }

  for (int player_id = 0; player_id < player_count_; player_id++)
  {
    int recent = players_[player_id].getRecentlyAddedPoints();
    if (recent > 0)
    {
      network.sendToAllClients("grantpoints " + std::to_string(player_id) + " " + std::to_string(recent) + " " +
                               std::to_string(players_[player_id].getPoints()));
      players_[player_id].setRecentlyAddedPoints(0);
    }
  }

  std::sort(players_.begin(),
            players_.end(),
            [](Player &a, Player &b)
            {
              if (a.getPoints() == b.getPoints())
              {
                if (a.getStoneCount() == b.getStoneCount())
                {
                  return a.getId() < b.getId();
                }
                return a.getStoneCount() > b.getStoneCount();
              }
              return a.getPoints() > b.getPoints();
            });

  std::string endgame_message = "endgame " + std::to_string(players_.size());
  for (auto &player : players_)
  {
    endgame_message += " " + std::to_string(player.getId());
  }

  network.sendToAllClients(endgame_message);
}
