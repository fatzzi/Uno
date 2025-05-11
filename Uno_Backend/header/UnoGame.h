#ifndef UNOGAME_H
#define UNOGAME_H

#include <vector>
#include <stack>
#include "Deck.h"

// Forward declarations
class Player;
class Card;

class UnoGame {
private:
    std::vector<Player*> players;
    Deck deck;
    Card* topCard;
    int currentPlayerIndex;
    bool isReverse;
    int cardsDrawn;

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
    // Destructor to clean up resources
    ~UnoGame();
};

#endif // UNOGAME_H