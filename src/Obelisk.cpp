#include "Obelisk.hpp"

Obelisk::Obelisk(int site_id, int player_id) : ConstructionSite(site_id), player_id_(player_id), obelisk_height_(0) {}

Obelisk::~Obelisk() = default;

Obelisk::Obelisk(const Obelisk &copy_obelisk) = default;

void Obelisk::addStone(int stones_amount)
{
  obelisk_height_ += stones_amount;
}

int Obelisk::getStones() const
{
  return obelisk_height_;
}

int Obelisk::getPlayerId() const
{
  return player_id_;
}
