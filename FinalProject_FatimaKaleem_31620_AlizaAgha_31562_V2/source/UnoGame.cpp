#include <iostream>
#include <algorithm>
#include <limits>
#include <sstream>
#include <exception>
#include <stdexcept>
#include "../header/UnoGame.h"
#include "../header/Player.h"
#include "../header/Card.h"
#include "../header/NumberCard.h"
#include "../header/ActionCard.h"
#include "../header/WildCard.h"
#include "../header/DrawFourCard.h"
#include "../header/Debug.h"
#include "../header/Exceptions.h"

UnoGame::UnoGame()
    : topCard(nullptr), currentPlayerIndex(0), isReverse(false), cardsDrawn(0), gameEnded(false), 
    pendingSkipEffect(false) {
}
// No top card at start, Start with first player in the list, Game direction is normal, not reversed, No cards drawn yet, Game is not ended initially,No skip effect pending

void UnoGame::addPlayer(Player* player) {
    // Check for null player pointer before adding
    if (!player) {
        throw Uno::NullPointerException("player");
    }
    players.push_back(player); // Add player to game
}

void UnoGame::startGame() {
    // Ensure there are players before starting the game
    if (players.empty()) {
        throw Uno::GameStateException("Cannot start game with no players");
    }
    
    // Initialize and shuffle the deck
    try {
        deck.initializeDeck(); // Fill deck with cards
        deck.shuffleDeck(); // Shuffle the deck randomly
    } catch (const std::exception& e) {
        throw Uno::ResourceException(std::string("Failed to initialize or shuffle deck: ") + e.what());
    }
    
    // Each player draws 7 cards at the start
    for (auto& player : players) {
        // Check for null player pointer in the list
        if (!player) {
            throw Uno::NullPointerException("player in players list");
        }
        for (int i = 0; i < 7; ++i) {
            try {
                player->drawCard(deck);
            } catch (const std::exception& e) {
                throw Uno::PlayerException(std::string("Failed for player to draw initial cards: ") + e.what());
            }
        }
    }
    
    // Draw the first card to start the game
    try {
        topCard = deck.drawCard();
        if (!topCard) {
            throw Uno::CardException("Failed to draw initial top card from deck");
        }
    } catch (const std::exception& e) {
        throw Uno::CardException(std::string("Error drawing initial top card: ") + e.what());
    }
    
    // If the first card is a special card, handle it
    if (dynamic_cast<WildCard*>(topCard) || dynamic_cast<DrawFourCard*>(topCard)) {
        // If it's a wild card, set a random color
        try {
            topCard->setColor(static_cast<CardColor>(rand() % 4)); // Set random color from 4 colors
        } catch (const std::exception& e) {
            throw Uno::CardException(std::string("Failed to set color for initial wild card: ") + e.what());
        }
    }
    
    std::cout << "Game Started! Top card is: ";
    topCard->print();
    std::cout << std::endl;
}

// Move to the next player's turn according to play direction and player list size
void UnoGame::nextTurn() {
    // Ensure there are players to advance turns
    if (players.empty()) {
        throw Uno::GameStateException("Cannot advance turns with no players");
    }
    
    // Calculate the next player index based on game direction
    if (isReverse) { // If play direction is reversed
        currentPlayerIndex--; // Move backward
        if (currentPlayerIndex < 0) {
            currentPlayerIndex = players.size() - 1; // Wrap around to last player if index negativ
        }
    } else {
        currentPlayerIndex++; // Otherwise move forward
        if (currentPlayerIndex >= players.size()) { 
            currentPlayerIndex = 0; // Wrap around to first player if overflow
        }
    }
}

// Return the index of the next player without changing currentPlayerIndex
int UnoGame::nextPlayerIndex() const {
    // Ensure there are players to determine the next player
    if (players.empty()) {
        throw Uno::GameStateException("Cannot determine next player with no players");
    }
    
    // Calculate the next player index without changing the current player
    int nextIdx = currentPlayerIndex;
    if (isReverse) { // Calculate previous player index if reversed
        nextIdx--;
        if (nextIdx < 0) nextIdx = players.size() - 1;
    } else {
        nextIdx++; // Otherwise calculate next player index forward
        if (nextIdx >= players.size()) nextIdx = 0;
    }
    return nextIdx;
}

// Handle a single player's turn: allow card play, drawing, calling UNO, quitting/ terminal version 
void UnoGame::playTurn() {
    Player* currentPlayer = getCurrentPlayer();
    // Ensure the current player is valid
    if (!currentPlayer) {
        throw Uno::PlayerException("Current player is null");
    }

    if (currentPlayer->hasWon()) return; // If player has already won, skip turn, this acts as a safegaurd as run checks if the player has won and terminates the game

    std::cout << "\n-- " << currentPlayer->getName() << "'s turn --" << std::endl;
    std::cout << "Top card: ";
    // Ensure the top card is valid
    if (!topCard) {
        throw Uno::CardException("Top card is null during play turn");
    }
    topCard->print();
    std::cout << std::endl;

    std::cout << "Has the device been passed to the next player? Press any key to continue..." << std::endl;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear input buffer

    currentPlayer->displayHand(); // Display current player's hand

    int choice;
    bool validChoice = false;

    while (!validChoice) {
        std::cout << "Choose a card to play (0-" << (currentPlayer->getHandSize() - 1) 
                  << "), enter -1 to draw, -2 to call UNO, or -9 to quit: ";

        if (!getIntegerInput(choice)) {
            std::cout << "Invalid input! Please enter a number." << std::endl;
            continue;
        }

        if (choice == -9) {
            std::cout << currentPlayer->getName() << " has ended the game." << std::endl;
            gameEnded = true;
            return;
        }

        if (choice == -2) {
            currentPlayer->callUNO(); // Player calls UNO
            continue; // Let them choose again
        }

        if (choice == -1) {
            std::cout << currentPlayer->getName() << " draws a card." << std::endl;
            try {
                drawCard(currentPlayer); // Player draws a card
                validChoice = true;
            } catch (const Uno::CardException& e) {
                std::cout << "Error drawing card: " << e.what() << std::endl;
                std::cout << "Trying to continue the game..." << std::endl;
            } catch (const Uno::GameStateException& e) {
                std::cout << "Game state error during draw: " << e.what() << std::endl;
                std::cout << "Trying to continue the game..." << std::endl;
            }
        } else if (choice >= 0 && choice < currentPlayer->getHandSize()) {
            try {
                Card* selectedCard = currentPlayer->getCardAtIndex(choice);
                // Ensure the selected card is valid
                if (!selectedCard) {
                    throw Uno::CardException("Selected card is null");
                }

                if (isCardPlayable(selectedCard)) {
                    currentPlayer->playCard(choice, *this); // Player plays the selected card
                    validChoice = true;
                    enforceUNOCall(currentPlayer); // Enforce UNO call rules

                    if (currentPlayer->hasWon()) {
                        std::cout << "\n*** " << currentPlayer->getName() << " wins! ***" << std::endl;
                        gameEnded = true;
                        return;
                    }
                } else {
                    std::cout << "That card cannot be played on the current top card. Please select a valid card or draw." << std::endl;
                }
            } catch (const Uno::CardException& e) {
                std::cout << "Card Error: " << e.what() << std::endl;
            } catch (const Uno::PlayerException& e) {
                std::cout << "Player Error: " << e.what() << std::endl;
            } catch (const Uno::GameStateException& e) {
                std::cout << "Game State Error: " << e.what() << std::endl;
            } catch (const Uno::InvalidInputException& e) {
                std::cout << "Input Error: " << e.what() << std::endl;
            }
        } else {
            std::cout << "Invalid choice! Please select a valid card or draw." << std::endl;
        }
    }

    nextTurn(); // Advance to the next turn
}

void UnoGame::drawCard(Player* player) {
    // Ensure player is not null
    if (!player) {
        throw Uno::NullPointerException("player");
    }
    
    try {
        player->drawCard(deck); // Player draws a card from the deck
        std::cout << player->getName() << " drew a card." << std::endl;
        
        // Check if player now has 21 or more cards after drawing
        if (player->getHandSize() >= 21) {
            std::cout << player->getName() << " has 21 or more cards and is eliminated!" << std::endl;
            eliminatePlayer(player); // Eliminate player if they have too many cards
        }
    } catch (const std::exception& e) {
        throw Uno::CardException(std::string("Failed to draw card for player: ") + e.what());
    }
}

void UnoGame::skipTurn() {
    try {
        int nextIdx = nextPlayerIndex(); // Get the index of the next player
        // Validate the next player index
        if (nextIdx < 0 || nextIdx >= players.size()) {
            throw Uno::GameStateException("Next player index out of bounds during skip turn");
        }
        
        std::cout << "Skipping " << players[nextIdx]->getName() << "'s turn!" << std::endl;
        nextTurn(); // Advance turn to skip the player
    } catch (const std::exception& e) {
        throw Uno::GameStateException(std::string("Failed to skip turn: ") + e.what());
    }
}

// Reverse the play direction (clockwise to counterclockwise or vice versa)
void UnoGame::reverseDirection() {
    isReverse = !isReverse; // Toggle the game direction
    std::cout << "Game direction reversed!" << std::endl;
}
// Force next player to draw specific number of cards as penalty
void UnoGame::makeNextPlayerDraw(int numCards) {
    // Validate number of cards to draw
    if (numCards <= 0) {
        throw Uno::InvalidInputException("Number of cards to draw must be positive");
    }
    
    try {
        int nextPlayerIdx = nextPlayerIndex(); // Get the index of the next player
        
        // Validate the next player index
        if (nextPlayerIdx < 0 || nextPlayerIdx >= players.size()) {
            throw Uno::GameStateException("Next player index out of bounds when making player draw");
        }
        
        Player* nextPlayer = players[nextPlayerIdx];
        // Ensure the next player is not null
        if (!nextPlayer) {
            throw Uno::NullPointerException("next player");
        }
        
        std::cout << nextPlayer->getName() << " must draw " << numCards << " cards!" << std::endl;
        
        for (int i = 0; i < numCards; ++i) {
            nextPlayer->drawCard(deck); // Next player draws cards
        }
        
        // Check if player now has 21 or more cards after drawing multiple cards
        if (nextPlayer->getHandSize() >= 21) {
            std::cout << nextPlayer->getName() << " has 21 or more cards and is eliminated!" << std::endl;
            eliminatePlayer(nextPlayer); // Eliminate player if they have too many cards
        }
    } catch (const std::exception& e) {
        throw Uno::GameStateException(std::string("Failed to make next player draw cards: ") + e.what());
    }
}
// Remove a specific card from a player's hand and put it in the discard pile
void UnoGame::dropCardFromPlayer(Player* player, int index) {
    // Ensure player is not null
    if (!player) {
        throw Uno::NullPointerException("player");
    }
    
    // Validate card index
    if (index < 0 || index >= player->getHandSize()) {
        throw Uno::InvalidInputException("Card index out of bounds for dropping card");
    }
    
    try {
        Card* cardToDrop = player->getCardAtIndex(index);
        // Ensure the card to drop is valid
        if (!cardToDrop) {
            throw Uno::CardException("Card to drop is null");
        }
        
        player->removeCardFromHand(index); // Remove card from player's hand
        deck.placeInDiscard(cardToDrop); // Place card in discard pile
    } catch (const std::exception& e) {
        throw Uno::CardException(std::string("Failed to drop card from player: ") + e.what());
    }
}
// Remove a player from the game (eliminate) and adjust current player index
void UnoGame::eliminatePlayer(Player* player) {
    // Ensure player is not null
    if (!player) {
        throw Uno::NullPointerException("player");
    }
    
    auto it = std::find(players.begin(), players.end(), player);
    if (it != players.end()) {
        std::cout << player->getName() << " has been eliminated from the game." << std::endl;
        players.erase(it); // Remove player from the game
        
        // Adjust currentPlayerIndex if needed after elimination
        if (currentPlayerIndex >= players.size()) {
            currentPlayerIndex = 0; // reset to 0 which will be ur first player
        }
    } else {
        throw Uno::PlayerException("Player not found in game for elimination");
    }
}

// Check if game is over either because game was ended, someone won, or only one player remains
bool UnoGame::isGameOver() {
    if (gameEnded) return true; // Game is over if explicitly ended/ flag set
    for (auto& player : players) {
        // Check for null player pointer during game over check
        if (!player) {
            throw Uno::NullPointerException("player in players list during game over check");
        }
        
        if (player->hasWon()) {
            return true; // Game is over if any player has won
        }
    }
    return players.size() <= 1; // Game is also over if only one player remains
}

// Enforce penalty if player forgets to call UNO when down to one card
void UnoGame::enforceUNOCall(Player* player) {
    // Ensure player is not null
    if (!player) {
        throw Uno::NullPointerException("player");
    }
    
    if (player->getHandSize() == 1 && !player->hasCalledUNOStatus()) {
        std::cout << player->getName() << " forgot to call UNO! Drawing 2 penalty cards." << std::endl;
        // Make the player draw two cards as penalty
        try {
            for (int i = 0; i < 2; ++i) {
                player->drawCard(deck);
            }
        } catch (const std::exception& e) {
            throw Uno::GameStateException(std::string("Failed to enforce UNO call penalty: ") + e.what());
        }
    }
}

bool UnoGame::isCardPlayable(Card* playedCard) {
    // Ensure playedCard is not null
    if (!playedCard) {
        throw Uno::CardException("Cannot check if null card is playable");
    }
    
    // Ensure topCard is not null
    if (!topCard) {
        throw Uno::CardException("Top card is null when checking card playability");
    }
    //if (!playedCard || !topCard) return false; this is redundant as the exception is handling this condition
    return *playedCard == *topCard;
    //return areCardsPlayable(playedCard, topCard); // Use utility function to check playability

    // We considered a templated comparison utility but determined that simple polymorphic comparison suffices here.
/
}

// Replace current top card with a new card and discard old top card
void UnoGame::setTopCard(Card* card) {
    // Ensure the card to set as top card is not null
    if (!card) {
        throw Uno::CardException("Cannot set null card as top card");
    }
    
    // Place the previous top card in the discard pile
    if (topCard) {
        try {
            deck.placeInDiscard(topCard);
        } catch (const std::exception& e) {
            throw Uno::CardException(std::string("Failed to place old top card in discard: ") + e.what());
        }
    }
    
    // Set the new top card
    topCard = card;
}

// Return pointer to the current player, throwing if invalid
Player* UnoGame::getCurrentPlayer() const {
    // Ensure there are players to get the current player
    if (players.empty()) {
        throw Uno::GameStateException("Cannot get current player with no players");
    }
    
    // Validate current player index
    if (currentPlayerIndex < 0 || currentPlayerIndex >= players.size()) {
        throw Uno::GameStateException("Current player index out of bounds");
    }
    
    Player* player = players[currentPlayerIndex];
    // Ensure the retrieved player is not null
    if (!player) {
        throw Uno::PlayerException("Current player is null");
    }
    
    return player;
}

Card* UnoGame::getTopCard() const {
    // Ensure top card is not null before returning
    if (!topCard) {
        throw Uno::CardException("Top card is null");
    }
    return topCard;
}

void UnoGame::run() {
    try {
        std::cout << "=== UNO GAME STARTED ===" << std::endl;
        startGame(); // Start the game
        
        while (!isGameOver()) {
            try {
                playTurn(); // Play a turn
            } catch (const Uno::GameStateException& e) {
                std::cout << "Game state error during turn: " << e.what() << std::endl;
                std::cout << "Attempting to continue with next player..." << std::endl;
                nextTurn(); // Advance to next player on game state error
            } catch (const Uno::PlayerException& e) {
                std::cout << "Player error during turn: " << e.what() << std::endl;
                std::cout << "Attempting to continue with next player..." << std::endl;
                nextTurn(); // Advance to next player on player error
            } catch (const Uno::CardException& e) {
                std::cout << "Card error during turn: " << e.what() << std::endl;
                std::cout << "Attempting to continue with next player..." << std::endl;
                nextTurn(); // Advance to next player on card error
            } catch (const Uno::InvalidInputException& e) {
                std::cout << "Invalid input error during turn: " << e.what() << std::endl;
                std::cout << "Attempting to continue with next player..." << std::endl;
                nextTurn(); // Advance to next player on invalid input error
            } catch (const Uno::ResourceException& e) {
                std::cout << "Resource error during turn: " << e.what() << std::endl;
                std::cout << "Attempting to continue with next player..." << std::endl;
                nextTurn(); // Advance to next player on resource error
            } catch (const std::exception& e) {
                std::cout << "Unexpected error during turn: " << e.what() << std::endl;
                std::cout << "Attempting to continue with next player..." << std::endl;
                nextTurn(); // Advance to next player on any other unexpected error
            }
        }
        
        // Find the winner after the game ends
        Player* winner = nullptr;
        for (auto& player : players) {
            if (!player) continue; // Skip null players
            if (player->hasWon()) {
                winner = player;
                break;
            }
        }
        
        if (winner) {
            std::cout << "\n*** " << winner->getName() << " wins the game! ***" << std::endl;
        } else if (players.size() == 1) {
            std::cout << "\n*** " << players[0]->getName() << " is the last player standing and wins! ***" << std::endl;
        } else {
            std::cout << "\n*** Game over with no winner ***" << std::endl;
        }
    } catch (const Uno::UnoException& e) {
        std::cout << "Fatal game error: " << e.what() << std::endl;
        std::cout << "Game had to be terminated." << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Unexpected fatal error: " << e.what() << std::endl;
        std::cout << "Game had to be terminated." << std::endl;
    }
}

int UnoGame::getPlayerCount() const {
    return players.size(); // Return current number of players
}

// Helper function to read an integer from std::cin safely
bool UnoGame::getIntegerInput(int& output) {
    std::string line;
    std::getline(std::cin, line); // Read a line of input
    std::stringstream ss(line);
    // Attempt to parse integer and ensure no extra characters
    return (ss >> output) && ss.eof();
}

void UnoGame::applyPendingEffects() {
    
    // apply skip effect if pending
    if (pendingSkipEffect) {
        try {
            skipTurn(); // This will call nextTurn() internally
            pendingSkipEffect = false;
        } catch (const std::exception& e) {
            throw Uno::GameStateException(std::string("Failed to apply skip effect: ") + e.what());
        }
    }
}


UnoGame::~UnoGame() {
    // Clean up the top card if it exists to prevent memory leaks
    if (topCard) {
        delete topCard;
        topCard = nullptr; // Set to nullptr after deletion
    }
    
    // Clean up all players to prevent memory leaks
    for (Player* p : players) {
        delete p;
    }
    players.clear(); // Clear the player vector
}

