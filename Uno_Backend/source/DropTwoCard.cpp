// DropTwoCard.cpp
#include "../header/DropTwoCard.h"
#include "../header/UnoGame.h"
#include "../header/Player.h"
#include <iostream>
#include <algorithm>

DropTwoCard::DropTwoCard(Color color)
    : ActionCard("DropTwo", color) {}

void DropTwoCard::play(UnoGame& game, Player* currentPlayer) {
    std::cout << "Drop Two Card played! " << currentPlayer->getName() << " can drop 2 cards of their choice." << std::endl;
    
    int handSize = currentPlayer->getHandSize();
    if (handSize <= 0) {
        std::cout << "No cards to drop!" << std::endl;
        return;
    }
    
    // Display the current player's hand for selection
    currentPlayer->displayHand();
    
    // Ask the player to select cards to drop (up to 2)
    int cardsToDrop = std::min(2, handSize);
    std::vector<int> selectedIndices;
    
    for (int i = 0; i < cardsToDrop; ++i) {
        int cardIndex;
        std::cout << "Select card " << (i + 1) << " to drop (0-" << (handSize - 1) << "), or -1 to stop dropping: ";
        std::cin >> cardIndex;
        
        // Allow early exit
        if (cardIndex < 0) {
            break;
        }
        
        // Validate selection
        if (cardIndex >= handSize) {
            std::cout << "Invalid card index! Skipping this drop." << std::endl;
            continue;
        }
        
        // Check if already selected
        bool alreadySelected = false;
        for (int idx : selectedIndices) {
            if (idx == cardIndex) {
                alreadySelected = true;
                break;
            }
        }
        
        if (alreadySelected) {
            std::cout << "Card already selected! Choose a different one." << std::endl;
            --i; // retry this selection
            continue;
        }
        
        selectedIndices.push_back(cardIndex);
        
        // Display the selected card
        std::cout << "Selected to drop: ";
        Card* selectedCard = currentPlayer->getCardAtIndex(cardIndex);
        selectedCard->print();
        std::cout << std::endl;
    }
    
    // Sort indices in descending order to avoid shifting issues when removing
    std::sort(selectedIndices.begin(), selectedIndices.end(), std::greater<int>());
    
    // Drop the selected cards
    for (int index : selectedIndices) {
        Card* cardToDrop = currentPlayer->getCardAtIndex(index);
        std::cout << "Dropping: ";
        cardToDrop->print();
        std::cout << std::endl;
        
        game.dropCardFromPlayer(currentPlayer, index);
    }
    
    std::cout << currentPlayer->getName() << " dropped " << selectedIndices.size() << " card(s)." << std::endl;
}

void DropTwoCard::print() const {
    std::cout << colorToString(color) << " Drop Two";
}

std::string DropTwoCard::getName() const {
    return "DropTwo";
}