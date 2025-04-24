#ifndef OBELISK_HPP
#define OBELISK_HPP

#include "ConstructionSite.hpp"

class Obelisk : public ConstructionSite
{
  private:
    int player_id_;
    int obelisk_height_;

  public:
    /**
     * Obelisk Constructor
     *
     * @param site_id initializes the side ID of the Obelisk.
     * @param player_id initializes the current player ID of the Obelisk.
     */
    Obelisk(int site_id, int player_id);

    /**
     * Obelisk Destruktor
     *
     * @param void
     */
    ~Obelisk() override;

    /**
     * Pyramid Copy Constructor
     *
     * @param obelisk that will be copied
     */
    Obelisk(const Obelisk &obelisk);

    /**
     * Adds a stone to the obelisk of each player.
     *
     * @param stone_amount that will added to the stone storage
     * @return void
     */
    void addStone(int stone_amount) override;

    /**
     * Gets the amount of stones from the obelisk.
     *
     * @param void
     * @return obelisk_height_ is the number oder stored/stacked stones in Obelisk
     */
    int getStones() const;

    /**
     * Gets the player id from the obelisk (which players obelisk it is).
     *
     * @param void
     * @return player_id_ is the player id oft the Obelisk
     */
    int getPlayerId() const;
};

#endif
