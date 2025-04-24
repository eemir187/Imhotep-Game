/**
 * ConstructionSite.hpp
 *
 * Includes all of the used construction classes.
 *
 */

#ifndef CONSTRUCTIONSITE_HPP
#define CONSTRUCTIONSITE_HPP

#include <algorithm>
#include <vector>
#include "network/Server.hpp"
#include "Player.hpp"

constexpr int EMPTY = -1;

enum LEVEL
{
  FIRST_LEVEL = 9,
  SECOND_THIRD_LEVEL = 13,
};

class ConstructionSite
{
  private:
    int site_id_;
    bool blocked_;

  public:
    /**
     * ConstructionSite Constructor
     *
     * @param site_id is the site id of the construction site
     *
     */
    ConstructionSite(int site_id);

    /**
     * ConstructionSite Destructor
     *
     * @param void
     *
     */
    virtual ~ConstructionSite();

    /**
     * ConstructionSite Copy-Constructor
     *
     * @param void
     *
     */
    ConstructionSite(const ConstructionSite &constructiosite);

    /**
     * Adds stones to the construction site
     *
     * @param stone_amount
     * @return void
     *
     */
    virtual void addStone(int stone_amount) = 0;

    /**
     * Gets stones from the construction site
     *
     * @param void
     * @return obelisk_height_ or stones_placed_ it returns each stone storage of the building.
     *
     */
    int getStones() const;

    /**
     * Get's information if the site is blocked at the moment or not
     *
     * @param void
     * @return blocked_ is if the site is blocked or not
     *
     */
    bool isBlocked() const;

    /**
     * Set's information if the site is blocked at the moment or not
     *
     * @param state sets if the site is blocked or not
     * @return void
     *
     */
    void setBlocked(bool state);
};

#endif
