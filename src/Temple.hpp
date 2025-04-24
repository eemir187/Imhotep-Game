#ifndef TEMPLE_HPP
#define TEMPLE_HPP

#include "ConstructionSite.hpp"
#include "Player.hpp"
#include <vector>

class Temple : public ConstructionSite
{
  public:
    /**
     * This is the constructor for our temple.
     *
     * @param site_id the site id
     * @param player_count the player count
     */
    Temple(int site_id, int player_count);

    /**
     * This is the destructor for our temple.
     *
     */
    ~Temple() override;

    /**
     * This is the copy constructor for our temple.
     *
     * @param other the temple to copy
     */
    Temple(const Temple &other) = default;

    /**
     * This will initialize the temple.
     *
     * @return void
     */
    void initialize();

    /**
     * This will add a stone to the temple.
     *
     * @param player_id the player id
     * @return void
     */
    void addStone(int player_id);

    /**
     * This will score the temple at the end of the round.
     *
     * @param players the players
     * @return void
     */
    void scoreEndOfRound(std::vector<Player> &players);


    /**
     * This will return the temple levels.
     *
     * @return the temple levels
     */
    const std::vector<std::vector<int>> &getTempleLevels() const;

  private:
    int player_count_;
    int temple_width_;
    int iterator_;
    std::vector<std::vector<int>> temple_levels_;
};

#endif
