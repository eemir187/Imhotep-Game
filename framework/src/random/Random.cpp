//------------------------------------------------------------------------------
// Random.cpp
//
// Definition of methods for "Random"-class
//
//------------------------------------------------------------------------------

#include "random/Random.hpp"

#include <iostream>
#include <cstdlib>
#include <sstream>
#include <algorithm>
#include <cstring>

//------------------------------------------------------------------------------
unsigned int Random::getSeed()
{
    bool seed_valid = false;
    char *rand_seed_from_env = getenv("RAND_SEED");
    unsigned int seed = 0;

    if (rand_seed_from_env)
    {
      if (!strcmp(rand_seed_from_env, "RANDOM")){
        std::random_device rand_dev;
        seed = rand_dev();
        seed_valid = true;
      }else{
        std::stringstream rand_seed_sstream(rand_seed_from_env);
        rand_seed_sstream >> seed;
        if (!rand_seed_sstream.eof() || rand_seed_sstream.bad())
        {
            seed_valid = false;
            std::cerr << "[WARN] could not parse seed for RNG from environment\n";
        }
        else
        {
            seed_valid = true;
        }
      }
    }

    if (!seed_valid)
    {
      seed = 12312;
    }
    return seed;
}

//------------------------------------------------------------------------------
Random& Random::getInstance()
{
    static Random instance;
    return instance;
}

//------------------------------------------------------------------------------
Random::Random() : mersenne_twister_(getSeed())
{
}

//------------------------------------------------------------------------------
long Random::getRandomNumber(int min, int max)
{
    return (int) (mersenne_twister_.operator()() % (max - min + 1) + min);
}

//------------------------------------------------------------------------------
void Random::initializeCards()
{
    card_ids_= {0,0,1,1,2,2,3,3,4,4,5,5,
                6,6,7,7,7,7,7,7,7,7,7,7,
                8,8,9,9,10,10,10,11,11,11};
    int temp = 0;
    for (int i = static_cast<int>(card_ids_.size()) - 1; i > 0; i--)
    {
        temp = getRandomNumber(0, i);
        std::swap(card_ids_[temp], card_ids_[i]);
    }
}

//------------------------------------------------------------------------------
void Random::initializeShipSizes()
{
    ship_sizes_ = {4,4,3,3,3,2,2,1};
    int temp = 0;
    for (int j = static_cast<int>(ship_sizes_.size()) - 1; j > 0; j--)
    {
        temp = getRandomNumber(0, j);
        std::swap(ship_sizes_[temp], ship_sizes_[j]);
    }
}

//------------------------------------------------------------------------------
int Random::getNextMarketCard()
{
    if (card_ids_.empty() || ship_sizes_.empty())
    {
        initializeCards();
        initializeShipSizes();
    }
    int id = card_ids_.back();
    card_ids_.pop_back();
    return id;
}

//------------------------------------------------------------------------------
int Random::getNextShipSize()
{
    if (card_ids_.empty() || ship_sizes_.empty())
    {
        initializeCards();
        initializeShipSizes();
    }
    if (ship_sizes_.size() <= 4)
    {
        initializeShipSizes();
    }
    int size = ship_sizes_.back();
    ship_sizes_.pop_back();
    return size;
}
