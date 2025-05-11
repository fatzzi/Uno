#include "../header/UnoGame.h"
#include "../header/Player.h"
#include "../header/Card.h"
#include "../header/NumberCard.h"
#include "../header/ActionCard.h"
#include "../header/WildCard.h"
#include "../header/DrawFourCard.h"
#include <iostream>
#include <algorithm>

UnoGame::UnoGame()
    : topCard(nullptr), currentPlayerIndex(0), isReverse(false), cardsDrawn(0) {
}

void UnoGame::addPlayer(Player* player) {
    players.push_back(player);
}

void UnoGame::startGame() {
    deck.initializeDeck();
    deck.shuffleDeck();
    
    for (auto& player : players) {
        for (int i = 0; i < 7; ++i) {  // Each player draws 7 cards at the start
            player->drawCard(deck);
        }
    }
    
    // Draw the first card to start the game
    topCard = deck.drawCard();
    
    // If the first card is a special card, handle it
    if (dynamic_cast<WildCard*>(topCard) || dynamic_cast<DrawFourCard*>(topCard)) {
        // If it's a wild card, set a random color
        topCard->setColor(static_cast<Color>(rand() % 4));
    }
    
    std::cout << "Game Started! Top card is: ";
    topCard->print();
    std::cout << std::endl;
}

void UnoGame::nextTurn() {
    if (isReverse) {
        currentPlayerIndex--;
        if (currentPlayerIndex < 0) {
            currentPlayerIndex = players.size() - 1;
        }
    } else {
        currentPlayerIndex++;
        if (currentPlayerIndex >= players.size()) {
            currentPlayerIndex = 0;
        }
    }
}

int UnoGame::nextPlayerIndex() const {
    int nextIdx = currentPlayerIndex;
    if (isReverse) {
        nextIdx--;
        if (nextIdx < 0) nextIdx = players.size() - 1;
    } else {
        nextIdx++;
        if (nextIdx >= players.size()) nextIdx = 0;
    }
    return nextIdx;
}

void UnoGame::playTurn() {
    Player* currentPlayer = players[currentPlayerIndex];

    if (currentPlayer->hasWon()) return;

    std::cout << "\n-- " << currentPlayer->getName() << "'s turn --" << std::endl;
    std::cout << "Top card: ";
    topCard->print();
    std::cout << std::endl;

    currentPlayer->displayHand();

    int choice;
    bool validChoice = false; // Flag to check if a valid choice has been made

    while (!validChoice) { // Loop until a valid choice is made
        // Ask player to choose a card or draw
        std::cout << "Choose a card to play (0-" << (currentPlayer->getHandSize() - 1) 
                  << "), enter -1 to draw, or enter -2 to call UNO: ";
        std::cin >> choice;

        if (choice == -2) {
            currentPlayer->callUNO();
            std::cout << "Choose a card to play (0-" << (currentPlayer->getHandSize() - 1) 
                      << ") or enter -1 to draw: ";
            std::cin >> choice;
        }

        if (choice == -1) {
            std::cout << currentPlayer->getName() << " draws a card." << std::endl;
            drawCard(currentPlayer);
            validChoice = true; // End the loop after drawing a card
        } else if (choice >= 0 && choice < currentPlayer->getHandSize()) {
            Card* selectedCard = currentPlayer->getCardAtIndex(choice);

            if (isCardPlayable(selectedCard)) {
                currentPlayer->playCard(choice, *this);
                validChoice = true; // Valid card played, end the loop

                // Check if player should have called UNO
                enforceUNOCall(currentPlayer);

                // Check if player has won
                if (currentPlayer->hasWon()) {
                    std::cout << "\n*** " << currentPlayer->getName() << " wins! ***" << std::endl;
                    return;
                }
            } else {
                std::cout << "That card cannot be played on the current top card. Please select a valid card or draw." << std::endl;
                // Do not set validChoice to true, so the loop continues
            }
        } else {
            std::cout << "Invalid choice! Please select a valid card or draw." << std::endl;
            // Do not set validChoice to true, so the loop continues
        }
    }

    nextTurn();
}


void UnoGame::drawCard(Player* player) {
    player->drawCard(deck);
    std::cout << player->getName() << " drew a card." << std::endl;
}

void UnoGame::skipTurn() {
    std::cout << "Skipping " << players[nextPlayerIndex()]->getName() << "'s turn!" << std::endl;
    nextTurn();
}

void UnoGame::reverseDirection() {
    isReverse = !isReverse;
    std::cout << "Game direction reversed!" << std::endl;
}

void UnoGame::makeNextPlayerDraw(int numCards) {
    int nextPlayerIdx = nextPlayerIndex();
    
    Player* nextPlayer = players[nextPlayerIdx];
    std::cout << nextPlayer->getName() << " must draw " << numCards << " cards!" << std::endl;
    
    for (int i = 0; i < numCards; ++i) {
        nextPlayer->drawCard(deck);
    }
}

void UnoGame::makeCurrentPlayerDrop(int numCards) {
    Player* currentPlayer = players[currentPlayerIndex];
    int cardsToDrop = std::min(numCards, currentPlayer->getHandSize());
    
    std::cout << currentPlayer->getName() << " must drop " << cardsToDrop << " cards!" << std::endl;
    
    for (int i = 0; i < cardsToDrop; ++i) {
        if (currentPlayer->getHandSize() > 0) {
            // For simplicity, always drop the first card
            Card* cardToDrop = currentPlayer->getCardAtIndex(0);
            std::cout << "Dropping: ";
            cardToDrop->print();
            std::cout << std::endl;
            
            currentPlayer->removeCardFromHand(0);
            deck.placeInDiscard(cardToDrop);
        }
    }
}

void UnoGame::dropCardFromPlayer(Player* player, int index) {
    if (index >= 0 && index < player->getHandSize()) {
        Card* cardToDrop = player->getCardAtIndex(index);
        player->removeCardFromHand(index);
        deck.placeInDiscard(cardToDrop);
    }
}

void UnoGame::eliminatePlayer(Player* player) {
    auto it = std::find(players.begin(), players.end(), player);
    if (it != players.end()) {
        std::cout << player->getName() << " has been eliminated from the game." << std::endl;
        players.erase(it);
        
        // Adjust currentPlayerIndex if needed
        if (currentPlayerIndex >= players.size()) {
            currentPlayerIndex = 0;
        }
    }
}

bool UnoGame::isGameOver() {
    for (auto& player : players) {
        if (player->hasWon()) {
            return true;
        }
    }
    return players.size() <= 1; // Game is also over if only one player remains
}

void UnoGame::enforceUNOCall(Player* player) {
    if (player->getHandSize() == 1 && !player->hasCalledUNOStatus()) {
        std::cout << player->getName() << " forgot to call UNO! Drawing 2 penalty cards." << std::endl;
        // Make the player draw two cards as penalty
        for (int i = 0; i < 2; ++i) {
            player->drawCard(deck);
        }
    }
}

bool UnoGame::isCardPlayable(Card* playedCard) {
    if (!topCard || !playedCard) return false;
    
    // Wild cards and Draw Four cards can be played on any card
    if (dynamic_cast<WildCard*>(playedCard) || dynamic_cast<DrawFourCard*>(playedCard)) {
        return true;
    }
    
    // Otherwise, cards must match in color or value/type
    if (playedCard->getColor() == topCard->getColor()) {
        return true;
    }
    
    // Check if they are number cards with the same number
    NumberCard* playedNumCard = dynamic_cast<NumberCard*>(playedCard);
    NumberCard* topNumCard = dynamic_cast<NumberCard*>(topCard);
    if (playedNumCard && topNumCard && playedNumCard->getNumber() == topNumCard->getNumber()) {
        return true;
    }
    
    // Check if they are the same type of action card
    if (playedCard->getName() == topCard->getName()) {
        return true;
    }
    
    return false;
}

void UnoGame::setTopCard(Card* card) {
    // Place the previous top card in the discard pile
    if (topCard) {
        deck.placeInDiscard(topCard);
    }
    
    // Set the new top card
    topCard = card;
}

Player* UnoGame::getCurrentPlayer() const {
    if (currentPlayerIndex >= 0 && currentPlayerIndex < players.size()) {
        return players[currentPlayerIndex];
    }
    return nullptr;
}

Card* UnoGame::getTopCard() const {
    return topCard;
}

void UnoGame::run() {
    std::cout << "=== UNO GAME STARTED ===" << std::endl;
    startGame();
    
    while (!isGameOver()) {
        playTurn();
    }
    
    // Find the winner
    Player* winner = nullptr;
    for (auto& player : players) {
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
}

int UnoGame::getPlayerCount() const {
    return players.size();
}

UnoGame::~UnoGame() {
    // Clean up the top card if it exists
    if (topCard) {
        delete topCard;
    }
    
    // Note: Players need to be deleted by whoever created them
}
