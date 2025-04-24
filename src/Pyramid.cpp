#include "Pyramid.hpp"

Pyramid::Pyramid(int site_id) : ConstructionSite(site_id), third_level_(EMPTY), stones_placed_(0) {}

Pyramid::~Pyramid() = default;

Pyramid::Pyramid(const Pyramid &copy_pyramid) = default;

void Pyramid::initialize()
{
  first_level_ = std::vector<std::vector<int>>(3, std::vector<int>(3, EMPTY));
  second_level_ = std::vector<std::vector<int>>(2, std::vector<int>(2, EMPTY));
  third_level_ = EMPTY;
  bonus_level_.clear();
  stones_placed_ = 0;
}

void Pyramid::addStone(int new_stone)
{
  stones_placed_ = stones_placed_ + new_stone;
}

void Pyramid::addStoneWithPoints(int player_id, std::vector<Player> &players)
{
  int point_counter = 0;
  int first_level_points[9] = {2, 1, 3, 2, 4, 3, 2, 1, 3};
  int second_level_points[4] = {2, 3, 1, 3};
  int third_level_points = 4;

  if (stones_placed_ < FIRST_LEVEL)
  {
    for (int colm = 0; colm < 3; colm++)
    {
      for (int row = 0; row < 3; row++)
      {
        if (first_level_[row][colm] == EMPTY)
        {
          first_level_[row][colm] = player_id;
          players[player_id].addPoints(first_level_points[point_counter]);
          players[player_id].setRecentlyAddedPoints(players[player_id].getRecentlyAddedPoints() +
                                                    first_level_points[point_counter]);
          stones_placed_++;
          return;
        }
        point_counter++;
      }
    }
  }
  else if (stones_placed_ < SECOND_THIRD_LEVEL)
  {
    for (int colm = 0; colm < 2; colm++)
    {
      for (int row = 0; row < 2; row++)
      {
        if (second_level_[row][colm] == EMPTY)
        {
          second_level_[row][colm] = player_id;
          players[player_id].addPoints(second_level_points[point_counter]);
          players[player_id].setRecentlyAddedPoints(players[player_id].getRecentlyAddedPoints() +
                                                    second_level_points[point_counter]);
          stones_placed_++;
          return;
        }
        point_counter++;
      }
    }
  }
  else if (stones_placed_ == SECOND_THIRD_LEVEL)
  {
    third_level_ = player_id;
    players[player_id].addPoints(third_level_points);
    players[player_id].setRecentlyAddedPoints(players[player_id].getRecentlyAddedPoints() + third_level_points);
    stones_placed_++;
    return;
  }
  else
  {
    players[player_id].addPoints(1);
    players[player_id].setRecentlyAddedPoints(players[player_id].getRecentlyAddedPoints() + 1);
    bonus_level_.push_back(player_id);
    stones_placed_++;
  }
}

int Pyramid::getStonesPlaced() const
{
  return stones_placed_;
}

const std::vector<std::vector<int>> &Pyramid::getFirstLevel() const
{
  return first_level_;
}

const std::vector<std::vector<int>> &Pyramid::getSecondLevel() const
{
  return second_level_;
}

int Pyramid::getThirdLevel() const
{
  return third_level_;
}

const std::vector<int> &Pyramid::getBonusLevelStones() const
{
  return bonus_level_;
}
