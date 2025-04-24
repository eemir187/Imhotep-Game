#ifndef BURIALCHAMBER_HPP
#define BURIALCHAMBER_HPP

#include "ConstructionSite.hpp"

class BurialChamber : public ConstructionSite
{
  private:
    std::vector<std::vector<int>> buried_matrix_;
    int buried_stone_amount_;

  public:
    /**
     * BurialChamber Constructor
     *
     * @param site_id the site id for the burial chamber
     */
    BurialChamber(int site_id);

    /**
     * BurialChamber Copy Constructor
     *
     * @param burial_chamber the burial chamber to copy
     */
    BurialChamber(const BurialChamber &burial_chamber);

    /**
     * BurialChamber Destructor
     *
     */
    ~BurialChamber() override;

    /**
     * Just updates the stone amount.
     *
     */
    void addStone(int stone_amount) override;

    /**
     * This is the getter for the Matrix
     * for the BurialChamber
     *
     * @param void
     * @return buried_matrix_ which is the matrix
     */
    std::vector<std::vector<int>> getBuriedMatrix() const;

    /**
     * This calculates the future x position.
     *
     * @param void
     * @return new_x_axis_amount which is new position in x axis
     */
    int calcNewXAmount();

    /**
     * This calculates the future y position.
     *
     * @param void
     * @return future_place_y_axis which is new position in y axis
     */
    int calcFutureYAmount();

    /**
     * Counts existing field horizontally.
     *
     * @param void
     * @return x_axis_amount amount of existing field
     */
    int xAxisSize();

    /**
     * Create a new vertical line.
     * Inside Matrix.
     *
     */
    void attachNewX();

    /**
     * Counts connected stones.
     *
     * @param row row position
     * @param col col position
     * @param player_id player id to look for
     * @param visited visit history
     * @return amount of connected stones.
     */
    int countConnectedStones(int row, int col, int player_id, std::vector<std::vector<bool>> &visited);

    /**
     * Place stone from player to next possible position.
     *
     * @param player_id ID of the player placing the stone
     */
    void addStoneWithPlayer(int player_id);

    /**
     * Gets the total number of stones.
     *
     * @return number of stones placed
     */
    int getStones() const;

    /**
     * Calculates the points for the players
     *
     * @param players vector of the players
     */
    void calcPoints(std::vector<Player> &players);
};

#endif
