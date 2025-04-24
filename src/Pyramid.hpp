#ifndef PYRAMID_HPP
#define PYRAMID_HPP

#include "ConstructionSite.hpp"
#include <vector>
#include "Player.hpp"

class Pyramid : public ConstructionSite
{
  private:
    std::vector<std::vector<int>> first_level_;
    std::vector<std::vector<int>> second_level_;
    int third_level_;
    std::vector<int> bonus_level_;
    int stones_placed_;

  public:
    /**
     * Pyramid Constructor
     *
     * @param site_id is the ID for the given building
     */
    Pyramid(int site_id);

    /**
     * Pyramid Destructor
     *
     * @param void
     */
    ~Pyramid() override;

    /**
     * Pyramid Copy Constructor
     *
     * @param pyramid that will be copied
     */
    Pyramid(const Pyramid &pyramid);

    /**
     * Initializes the values of the Pyramid
     *
     * @param void
     * @return void
     */
    void initialize();

    /**
     * Adds a spacific number of stones to the.
     *
     * @param new_stone Number of stone to add up on the stones of the pyramids
     * @return void
     *
     */
    void addStone(int new_stone) override;

    /**
     * Adds a spacific number of stones to the.
     *
     * @param player_id Number of the current Player ID
     * @param players List off all Players in the Game
     */
    void addStoneWithPoints(int player_id, std::vector<Player> &players);

    /**
     * Gets all stones stored on the pyramid constructio site.
     *
     * @param void
     * @return stones_placed_ is the amount of current stored stones.
     *
     */
    int getStonesPlaced() const;

    /**
     * This function gets a List of all fields of the first level of the Pyramid
     * It shows multible fields and if they are use the player id is on it if they are empty it's -1.
     *
     * @param void
     * @return first_level_
     *
     */
    const std::vector<std::vector<int>> &getFirstLevel() const;

    /**
     * This function gets a List of all fields of the second level of the Pyramid
     * It shows multible fields and if they are use the player id is on it if they are empty it's -1.
     *
     * @param void
     * @return second_level_
     *
     */
    const std::vector<std::vector<int>> &getSecondLevel() const;

    /**
     * This function gets a single number of the third level of the Pyramid because there is only one more field
     * It shows a single field and if it is used the player id is on it if they are empty it's -1.
     *
     * @param void
     * @return third_level_
     *
     */
    int getThirdLevel() const;

    /**
     * This function gets a single number of the bonus level of the Pyramid because there is only one more field.
     * The bonus field is on the very top of the pyramid and is for points after give away.
     * It shows a single field and if it is used the player id is on it if they are empty it's -1.
     *
     * @param void
     * @return bonus_level_
     *
     */
    const std::vector<int> &getBonusLevelStones() const;
};

#endif
