#ifndef COMBINED_UNO_H
#define COMBINED_UNO_H

#include <string>
#include <iostream>
#include <stack>
#include <vector>

// Forward declarations
class UnoGame;
class Player;
class Card;
class Deck;

enum class deckColor {
    red,
    blue,
    green,
    yellow,
    none    // For wild cards before color is chosen
};

// Helper function to convert Color to string
inline std::string colorToString(deckColor color) {
    switch (color) {
        case deckColor::red: return "Red";
        case deckColor::blue: return "Blue";
        case deckColor::green: return "Green";
        case deckColor::yellow: return "Yellow";
        case deckColor::none: return "None";
        default: return "Unknown";
    }
}

// Helper function to convert string to Color
inline deckColor stringToColor(const std::string& str) {
    if (str == "Red") return deckColor::red;
    if (str == "Blue") return deckColor::blue;
    if (str == "Green") return deckColor::green;
    if (str == "Yellow") return deckColor::yellow;
    return deckColor::none;
}

class Card {
protected:
    std::string name;
    deckColor color;

public:
    Card(const std::string& cardName, deckColor cardColor = deckColor::none);
    virtual void play(UnoGame& game, Player* currentPlayer) = 0;
    virtual void print() const = 0;
    virtual std::string getName() const = 0;
    virtual deckColor getColor() const;
    virtual void setColor(deckColor newColor);
    virtual bool operator==(const Card& other) const;
    virtual ~Card();
};

class ActionCard : public Card {
public:
    ActionCard(const std::string& cardName, deckColor cardColor = deckColor::none);
    virtual void play(UnoGame& game, Player* currentPlayer) = 0;
    virtual void print() const = 0;
    virtual std::string getName() const = 0;
    virtual ~ActionCard();
};


class Deck {
private:
    std::stack<Card*> drawPile;
    // Stack for drawing cards
    std::stack<Card*> discardPile;
    // Stack for discarded cards

    // Helper function to create and add the cards to the deck
    void addStandardUNODeck();

public:
    Deck();
    ~Deck();

    // Initialize the deck with 108 standard UNO cards
    void initializeDeck();

    // Shuffle the draw pile
    void shuffleDeck();

    // Draw a card from the deck (i.e., pop from drawPile)
    Card* drawCard();

    // Check if the draw pile is empty
    bool isEmpty() const;

    // Place a card in the discard pile
    void placeInDiscard(Card* card);

    // Move all cards from the discard pile back to the draw pile and shuffle
    void reshuffleDiscardIntoDeck();

    // Return a card to the draw pile
    void returnCardToDeck(Card* card);

    // Get the top card of the discard pile
    Card* getTopDiscard() const;
};

class DrawFourCard : public Card {
public:
    DrawFourCard();
    void play(UnoGame& game, Player* currentPlayer) override;
    void print() const override;
    std::string getName() const override;
};

class DrawSixCard : public ActionCard {
public:
    DrawSixCard(deckColor color);
    void play(UnoGame& game, Player* currentPlayer) override;
    void print() const override;
    std::string getName() const override;
};

class DrawTwoCard : public ActionCard {
public:
    DrawTwoCard(deckColor color);
    void play(UnoGame& game, Player* currentPlayer) override;
    void print() const override;
    std::string getName() const override;
};

class DropTwoCard : public ActionCard {
public:
    DropTwoCard(deckColor color);
    void play(UnoGame& game, Player* currentPlayer) override;
    void print() const override;
    std::string getName() const override;
};

class NumberCard : public Card {
private:
    int number;

public:
    NumberCard(int num, deckColor col);
    void play(UnoGame& game, Player* currentPlayer) override;
    void print() const override;
    std::string getName() const override;
    deckColor getColor() const override;
    int getNumber() const;
};

class Player {
private:
    std::string name;
    std::vector<Card*> hand;
    bool hasCalledUNO;

public:
    // Constructor that sets the player's name
    Player(const std::string& n);

    // Adds a card to the player's hand
    void drawCard(Deck& deck);

    // Plays a card from the player's hand
    void playCard(int index, UnoGame& game);

    // Checks if the player has no cards left
    bool hasWon() const;

    // Displays the player's current cards
    void displayHand() const;

    // Player declares they have one card left
    void callUNO();

    // Returns whether the player has called UNO
    bool hasCalledUNOStatus() const;

    // Resets the UNO call status
    void resetUNOCall();

    // Removes a card from hand at a given index
    void removeCardFromHand(int index);

    // Retrieves the card at a given index
    Card* getCardAtIndex(int index) const;

    // Returns the size of the hand
    int getHandSize() const;

    // Gets the player's name
    std::string getName() const;

    // Destructor to clean up cards in hand
    ~Player();
};

class ReverseCard : public ActionCard {
public:
    ReverseCard(deckColor color);
    void play(UnoGame& game, Player* currentPlayer) override;
    void print() const override;
    std::string getName() const override;
};

class SkipCard : public ActionCard {
public:
    SkipCard(deckColor color);
    void play(UnoGame& game, Player* currentPlayer) override;
    void print() const override;
    std::string getName() const override;
};


class UnoGame {
private:
    std::vector<Player*> players;
    Deck deck;
    Card* topCard;
    int currentPlayerIndex;
    bool isReverse;
    int cardsDrawn;
    bool gameEnded = false;

public:
    UnoGame();

    // Adds a player to the game
    void addPlayer(Player* player);

    // Starts the game, initializing and shuffling deck, etc.
    void startGame();

    // Advances the turn to the next player
    void nextTurn();

    // Executes the player's turn
    void playTurn();

    // Forces a player to draw a card
    void drawCard(Player* player);

    // Skip the next player's turn
    void skipTurn();

    // Reverse the direction of the turn order
    void reverseDirection();

    // Make the next player draw a specified number of cards
    void makeNextPlayerDraw(int numCards);

    // Make the current player drop a specified number of cards
    void makeCurrentPlayerDrop(int numCards);

    // Eliminate a player from the game
    void eliminatePlayer(Player* player);

    // Checks if the game is over
    bool isGameOver();

    // Enforce the rule of calling UNO when a player has one card left
    void enforceUNOCall(Player* player);

    // Check if a card can be played on the current top card
    bool isCardPlayable(Card* playedCard);

    // Set the top card of the game
    void setTopCard(Card* card);

    // Get the current player
    Player* getCurrentPlayer() const;

    // Get the top card
    Card* getTopCard() const;

    // Get the next player index
    int nextPlayerIndex() const;

    // Drop a card from a player's hand
    void dropCardFromPlayer(Player* player, int index);

    // Run the entire game
    void run();
    int getPlayerCount() const;
    bool getIntegerInput(int& output);
    // Destructor to clean up resources
    ~UnoGame();
};

class WildCard : public Card {
public:
    WildCard();
    void play(UnoGame& game, Player* currentPlayer) override;
    void print() const override;
    std::string getName() const override;
};

#endif // COMBINED_UNO_H