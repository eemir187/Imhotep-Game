#include "Temple.hpp"
#include <algorithm>

Temple::Temple(int site_id, int player_count) : ConstructionSite(site_id), player_count_(player_count), iterator_(0)
{
  temple_width_ = (player_count_ >= 3) ? 5 : 4;
}

Temple::~Temple() = default;

void Temple::initialize()
{
  temple_levels_.clear();
  iterator_ = 0;
}

void Temple::addStone(int player_id)
{
  if (temple_levels_.empty() || iterator_ == temple_width_)
  {
    std::vector<int> new_row(temple_width_, -1);
    temple_levels_.push_back(new_row);
    iterator_ = 0;
  }

  temple_levels_.back()[iterator_] = player_id;
  iterator_++;
}


void Temple::scoreEndOfRound(std::vector<Player> &players)
{

  for (int col = 0; col < temple_width_; col++)
  {
    for (int row = static_cast<int>(temple_levels_.size()) - 1; row >= 0; row--)
    {
      int player_id = temple_levels_[row][col];
      if (player_id != -1)
      {
        players[player_id].addPoints(1);
        players[player_id].setRecentlyAddedPoints(players[player_id].getRecentlyAddedPoints() + 1);
        break;
      }
    }
  }
}

const std::vector<std::vector<int>> &Temple::getTempleLevels() const
{
  return temple_levels_;
}
