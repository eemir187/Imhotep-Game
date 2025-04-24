/**
 * ConstructionSite.cpp
 *
 * Includes all of the useonstruction classes and methods logic.
 *
 */

#include "ConstructionSite.hpp"

ConstructionSite::ConstructionSite(int site_id) : site_id_(site_id), blocked_(false){};

ConstructionSite::~ConstructionSite() = default;

ConstructionSite::ConstructionSite(const ConstructionSite &constructiosite) = default;

bool ConstructionSite::isBlocked() const
{
  return blocked_;
}

void ConstructionSite::setBlocked(bool new_state)
{
  blocked_ = new_state;
}
