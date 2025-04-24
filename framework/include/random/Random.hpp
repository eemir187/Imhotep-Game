//------------------------------------------------------------------------------
// Random.hpp
//
// Adaption of Random from SS_2020
//
//------------------------------------------------------------------------------

#ifndef RANDOM_HPP
#define RANDOM_HPP

#include <random>
#include <vector>

//----------------------------------------------------------------------------
// Random class
//

class Random final
{
private:
    std::mt19937 mersenne_twister_;
    std::vector<int> card_ids_;
    std::vector<int> ship_sizes_;

    //--------------------------------------------------------------------------
    // Constructor of Random Class is private, only getInstance possible
    //
    Random();

    //--------------------------------------------------------------------------
    // Initializer for the card_ids_ vector, fills the vector and shuffles it
    //
    void initializeCards();

    //--------------------------------------------------------------------------
    // Initializer for the ship_sizes_ vector, fills the vector and shuffles it
    //
    void initializeShipSizes();

public:

    //--------------------------------------------------------------------------
    // returns an integer suitable to seed the random number generator
    //
    static unsigned int getSeed();

    //--------------------------------------------------------------------------
    // singleton pattern returns Instance of Random class
    //
    static Random& getInstance();

    //--------------------------------------------------------------------------
    // deleted copy constructor
    //
    Random(const Random& other) = delete;

    //--------------------------------------------------------------------------
    // deleted assignment operator
    //
    Random& operator=(const Random& rhs) = delete;

    //--------------------------------------------------------------------------
    //  getRandomNumber function, value between min(incl.) and max (incl.)
    //
    long getRandomNumber(int min, int max);

    //--------------------------------------------------------------------------
    //  getNextMarketCard function, removes and returns the last element of the
    //  card_ids vector, if the vector is empty start initialisation;
    //
    //  @return id of next Market Card of shuffled stack
    int getNextMarketCard();

    //--------------------------------------------------------------------------
    //  getNextShipSize function, removes and returns the last element of the
    //  ship_sizes_ vector, if the vector is empty or under 4 start initialisation;
    //
    //  @return size of the next Ship
    int getNextShipSize();
};
#endif