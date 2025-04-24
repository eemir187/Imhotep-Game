/**
 * MarketCard.hpp
 *
 * This is the MarektCard class.
 * Is contains everything for card.
 *
 */

#ifndef MARKETCARD_HPP
#define MARKETCARD_HPP

enum COLOR
{
  RED = 0,
  GREEN,
  PURPLE,
  BLUE
};

class MarketCard
{
  private:
    int card_id_;
    COLOR color_;
    static int number_market_cards_;

  public:
    /**
     * This is the constructor for our MarketCard
     *
     * @param ID card id.
     * @return void
     */
    MarketCard(int ID);

    /**
     * This is the destructor for our MarketCard
     *
     * @param void
     * @return void
     */
    ~MarketCard();

    /**
     * This is the copy constructor for our MarketCard
     *
     * @param market_card MarketCard
     * @return void
     */
    MarketCard(const MarketCard &market_card);

    /**
     * Getter for number of cards
     *
     * @param void
     * @return number_market_cards_ Number of market cards
     */
    static int getNumberMarketCards();

    /**
     * Getter for Market Card ID.
     *
     * @param void
     * @param card_id_ Market Card ID
     */
    int getMarketCardID() const;

    /**
     * Getter for Market Card ID.
     *
     * @param value New value
     * @return void
     */
    void setMarketCardID(int value);

    /**
     * Getter for Market Card ID.
     *
     * @param void
     * @return color_ Market Card color
     */
    COLOR getMarketCardColor() const;
};

#endif
