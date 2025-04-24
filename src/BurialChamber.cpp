/**
 * BurialChamber.cpp
 *
 * Implementation of the BurialChamber class.
 *
 */

#include "BurialChamber.hpp"
BurialChamber::BurialChamber(int site_id) : ConstructionSite(site_id), buried_stone_amount_(0)
{
  buried_matrix_.resize(3);
}

BurialChamber::BurialChamber(const BurialChamber &burial_chamber) = default;

BurialChamber::~BurialChamber() = default;

void BurialChamber::addStone(int stone_amount)
{
  buried_stone_amount_ += stone_amount;
}

int BurialChamber::calcNewXAmount()
{
  int new_x_axis_amount = buried_stone_amount_ / 3;

  return new_x_axis_amount;
}

int BurialChamber::calcFutureYAmount()
{
  int future_place_y_axis = buried_stone_amount_ % 3;

  return future_place_y_axis;
}

int BurialChamber::xAxisSize()
{
  int x_axis_amount = static_cast<int>(buried_matrix_.at(0).size());

  return x_axis_amount;
}

void BurialChamber::attachNewX()
{
  for (int current_y = 0; current_y < 3; current_y++)
  {
    buried_matrix_.at(current_y).push_back(-1);
  }
}

std::vector<std::vector<int>> BurialChamber::getBuriedMatrix() const
{
  return buried_matrix_;
}

void BurialChamber::addStoneWithPlayer(int player_id)
{
  int new_x_amount = calcNewXAmount();
  int future_y = calcFutureYAmount();
  int current_x_amount = xAxisSize();

  if (new_x_amount >= current_x_amount)
  {
    attachNewX();
  }

  buried_matrix_.at(future_y).at(new_x_amount) = player_id;

  addStone(1);
}

int BurialChamber::getStones() const
{
  return buried_stone_amount_;
}

void BurialChamber::calcPoints(std::vector<Player> &players)
{
  if (buried_stone_amount_ == 0)
  {
    return;
  }

  int rows = 3;
  int cols = xAxisSize();
  if (cols == 0)
  {
    return;
  }

  std::vector<std::vector<bool>> visited(rows, std::vector<bool>(cols, false));
  for (int row_counter = 0; row_counter < rows; row_counter++)
  {
    for (int col_counter = 0; col_counter < cols; col_counter++)
    {
      int current_player_id = buried_matrix_[row_counter][col_counter];
      if (current_player_id == -1 || visited[row_counter][col_counter])
      {
        continue;
      }

      int group_size = countConnectedStones(row_counter, col_counter, current_player_id, visited);

      int points = 0;
      switch (group_size)
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
        default:
          points = 15 + (group_size - 5) * 2;
      }

      players[current_player_id].addPoints(points);
      players[current_player_id].setRecentlyAddedPoints(
      points + players[current_player_id].getRecentlyAddedPoints());
    }
  }
}

int BurialChamber::countConnectedStones(int row, int col, int player_id, std::vector<std::vector<bool>> &visited)
{
  if (row < 0 || row >= 3 || col < 0 || col >= xAxisSize())
    return 0;

  if (buried_matrix_[row][col] != player_id || visited[row][col])
    return 0;

  visited[row][col] = true;

  int count = 1;
  count += countConnectedStones(row - 1, col, player_id, visited);
  count += countConnectedStones(row + 1, col, player_id, visited);
  count += countConnectedStones(row, col - 1, player_id, visited);
  count += countConnectedStones(row, col + 1, player_id, visited);

  return count;
}
