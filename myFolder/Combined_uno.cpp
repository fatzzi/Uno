// Keep necessary standard library includes
#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <algorithm>
#include <random>
#include <ctime>
#include <cstdlib>
#include "..\headers\Combined_uno.h" 
// ActionCard.cpp
ActionCard::ActionCard(const std::string& cardName, deckColor cardColor)
    : Card(cardName, cardColor) {}

ActionCard::~ActionCard() {}

// Card.cpp
Card::Card(const std::string& cardName, deckColor cardColor)
    : name(cardName), color(cardColor) {}

Card::~Card() {}

deckColor Card::getColor() const {
    return color;
}

void Card::setColor(deckColor newColor) {
    color = newColor;
}

bool Card::operator==(const Card& other) const {
     return name == other.name && color == other.color;
}


// Deck.cpp
Deck::Deck() {
   std::srand(std::time(0)); // Initialize random number generator for shuffling
}

Deck::~Deck() {
    // Cleanup: Delete all cards in the drawPile and discardPile
    while (!drawPile.empty()) {
        delete drawPile.top();
        drawPile.pop();
    }

    while (!discardPile.empty()) {
        delete discardPile.top();
        discardPile.pop();
    }
}

void Deck::initializeDeck() {
   addStandardUNODeck(); // Fill the deck with the standard UNO cards
}

void Deck::addStandardUNODeck() {
    // Standard UNO deck has 108 cards:
    // - 19 Blue, 19 Green, 19 Red, and 19 Yellow cards (76 total)
    // - 8 Skip, 8 Reverse, and 8 Draw Two cards (24 total)
    // - 4 Wild cards and 4 Wild Draw Four cards (8 total)
    // Added: 8 Draw Six and 8 Drop Two cards (16 total) - making 124 cards in total

   std::vector<deckColor> colors = {deckColor::red, deckColor::green, deckColor::blue, deckColor::yellow};

    for (const auto& color : colors) {
        // One 0 card per color
       drawPile.push(new NumberCard(0, color));

        // Two of each 1-9 cards per color
        for (int i = 1; i <= 9; ++i) {
           drawPile.push(new NumberCard(i, color));
           drawPile.push(new NumberCard(i, color));
        }

        // Two of each action card per color
        for (int i = 0; i < 2; ++i) {
           drawPile.push(new SkipCard(color));
           drawPile.push(new ReverseCard(color));
           drawPile.push(new DrawTwoCard(color));
           drawPile.push(new DrawSixCard(color)); // New card type
           drawPile.push(new DropTwoCard(color)); // New card type
        }
    }

    // Add wild cards
    for (int i = 0; i < 4; ++i) {
       drawPile.push(new WildCard());
       drawPile.push(new DrawFourCard());
    }
}

void Deck::shuffleDeck() {
   std::vector<Card*> allCards;

    // Transfer all cards from the drawPile to a temporary vector
    while (!drawPile.empty()) {
       allCards.push_back(drawPile.top());
       drawPile.pop();
    }

    // Shuffle using modern C++ random facilities
    std::random_device rd;
    std::mt19937 g(rd());
   std::shuffle(allCards.begin(), allCards.end(), g);

    // Push the shuffled cards back into the drawPile
    for (auto& card : allCards) {
       drawPile.push(card);
    }
}

Card* Deck::drawCard() {
    if (drawPile.empty()) {
        if (discardPile.empty()) {
            return nullptr; // No cards left in the game
        }
       reshuffleDiscardIntoDeck();
    }

    Card* top = drawPile.top();
    drawPile.pop();
    return top;
}

bool Deck::isEmpty() const {
    return drawPile.empty();
}

void Deck::placeInDiscard(Card* card) {
   discardPile.push(card);
}

void Deck::reshuffleDiscardIntoDeck() {
    // Save the top card of the discard pile
    Card* topCard = nullptr;
    if (!discardPile.empty()) {
       topCard = discardPile.top();
       discardPile.pop();
    }

    // Move all cards from discard pile to draw pile
    while (!discardPile.empty()) {
       drawPile.push(discardPile.top());
       discardPile.pop();
    }

    // Shuffle the draw pile
    shuffleDeck();

    // Put the top card back on the discard pile
    if (topCard) {
       discardPile.push(topCard);
    }
}

void Deck::returnCardToDeck(Card* card) {
   drawPile.push(card);
}

Card* Deck::getTopDiscard() const {
    if (!discardPile.empty()) {
        return discardPile.top();
    }
    return nullptr;
}

// DrawFourCard.cpp
DrawFourCard::DrawFourCard()
    : Card("DrawFour", deckColor::none) {}

void DrawFourCard::play(UnoGame& game, Player* currentPlayer) {
    std::cout << "Draw Four Card played! Choose a color (0-Red, 1-Yellow, 2-Green, 3-Blue): ";
    int choice;
    std::cin >> choice;

    switch(choice) {
        case 0: color = deckColor::red; break;
        case 1: color = deckColor::yellow; break;
        case 2: color = deckColor::green; break;
        case 3: color = deckColor::blue; break;
        default: color = deckColor::red; break; // Default to red if invalid input
    }

    std::cout << "Color changed to " << colorToString(color) << "!\n";
    game.makeNextPlayerDraw(4);
}

void DrawFourCard::print() const {
    std::cout << "Wild Draw Four";
    if (color != deckColor::none) {
        std::cout << " (" << colorToString(color) << ")";
    }
}

std::string DrawFourCard::getName() const {
    return "DrawFour";
}

// DrawSixCard.cpp
DrawSixCard::DrawSixCard(deckColor color)
    : ActionCard("DrawSix", color) {}

void DrawSixCard::play(UnoGame& game, Player* currentPlayer) {
    std::cout << "Draw Six Card played! Next player must draw 6 cards.\n";
    game.makeNextPlayerDraw(6);
}

void DrawSixCard::print() const {
    std::cout << colorToString(color) << " Draw Six";
}

std::string DrawSixCard::getName() const {
    return "DrawSix";
}

// DrawTwoCard.cpp
DrawTwoCard::DrawTwoCard(deckColor color)
    : ActionCard("DrawTwo", color) {}

void DrawTwoCard::play(UnoGame& game, Player* currentPlayer) {
    std::cout << "Draw Two Card played! Next player must draw 2 cards.\n";
    game.makeNextPlayerDraw(2);
}

void DrawTwoCard::print() const {
    std::cout << colorToString(color) << " Draw Two";
}

std::string DrawTwoCard::getName() const {
    return "DrawTwo";
}

// DropTwoCard.cpp
DropTwoCard::DropTwoCard(deckColor color)
    : ActionCard("DropTwo", color) {}

void DropTwoCard::play(UnoGame& game, Player* currentPlayer) {
    std::cout << "Drop Two Card played! " << currentPlayer->getName()
              << " can drop 2 cards of their choice." << std::endl;

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
        std::cout << "Select card " << (i + 1) << " to drop (0-" << (handSize - 1)
                  << "), or -1 to stop dropping: ";

        // Use getIntegerInput for safer integer input
        if (!game.getIntegerInput(cardIndex)) {
             std::cout << "Invalid input. Skipping this drop selection." << std::endl;
             // Clear the error state of cin and ignore the rest of the line
             std::cin.clear();
             std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
             --i; // Retry this selection attempt
             continue;
        }

        // Allow early exit
        if (cardIndex < 0) {
            break;
        }

        // Validate selection
        if (cardIndex >= handSize) {
            std::cout << "Invalid card index! Skipping this drop." << std::endl;
            --i; // retry this selection
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


// Numbercard.cpp
NumberCard::NumberCard(int num, deckColor col)
    : Card("NumberCard", col), number(num) {
    name = colorToString(col) + " " + std::to_string(num);
}

void NumberCard::play(UnoGame& game, Player* currentPlayer) {
    // Logic for number cards - no special action, just played as is
    std::cout << "Played " << colorToString(color) << " "
              << number << " card." << std::endl;
}

void NumberCard::print() const {
    std::cout << colorToString(color) << " " << number;
}

int NumberCard::getNumber() const {
    return number;
}

deckColor NumberCard::getColor() const {
    return color;
}

std::string NumberCard::getName() const {
    return name;
}

// Player.cpp
Player::Player(const std::string& n)
    : name(n), hasCalledUNO(false) {
}

void Player::drawCard(Deck& deck) {
    Card* drawnCard = deck.drawCard();
    if (drawnCard) {
        hand.push_back(drawnCard); // Add the drawn card to the player's hand
        // Reset UNO call if player has more than one card now
        if (hand.size() > 1) {
           resetUNOCall();
        }
    }
}

void Player::playCard(int index, UnoGame& game) {
    if (index >= 0 && index < hand.size()) {
        Card* cardToPlay = hand[index];

        // Remove the card from the player's hand
        hand.erase(hand.begin() + index);

        // Then play the card which applies its effects
        cardToPlay->play(game, this);

        // Update the top card in the game
        game.setTopCard(cardToPlay);
    }
}

bool Player::hasWon() const {
    return hand.empty(); // A player wins when they have no cards left
}

void Player::displayHand() const {
    std::cout << name << "'s hand:" << std::endl;
    for (int i = 0; i < hand.size(); ++i) {
        std::cout << i << ": ";
        hand[i]->print();
        std::cout << std::endl;
    }
}

void Player::callUNO() {
    if (hand.size() == 2) {
        hasCalledUNO = true;
        std::cout << name << " has called UNO!" << std::endl;
    } else {
        std::cout << "You must have 2 cards to call UNO!" << std::endl;
    }
}

bool Player::hasCalledUNOStatus() const {
    return hasCalledUNO;
}

void Player::resetUNOCall() {
    hasCalledUNO = false;
}

void Player::removeCardFromHand(int index) {
    if (index >= 0 && index < hand.size()) {
        // Don't delete the card as it will be used elsewhere (transferred ownership)
        hand.erase(hand.begin() + index);
    }
}

Card* Player::getCardAtIndex(int index) const {
    if (index >= 0 && index < hand.size()) {
        return hand[index];
    }
    return nullptr; // Return nullptr if the index is invalid
}

int Player::getHandSize() const {
    return hand.size();
}

std::string Player::getName() const {
    return name;
}

Player::~Player() {
    for (Card* card : hand) {
        delete card; // Free the memory allocated for each card
    }
}

// Reversecard.cpp
ReverseCard::ReverseCard(deckColor color)
    : ActionCard("Reverse", color) {}

void ReverseCard::play(UnoGame& game, Player* currentPlayer) {
    // Check the number of players in the game
    if (game.getPlayerCount() == 2) {
        // If there are 2 players, skip the next player's turn
        std::cout << "Reversing the direction of play! (acting as a skip)" << std::endl;
        game.skipTurn(); // Call the skip function
    } else {
        // Normal reverse functionality
        std::cout << "Reversing the direction of play!" << std::endl;
        game.reverseDirection();
    }
}
void ReverseCard::print() const {
    std::cout << colorToString(color) << " Reverse";
}

std::string ReverseCard::getName() const {
    return "Reverse";
}

// Skipcard.cpp
SkipCard::SkipCard(deckColor color)
    : ActionCard("Skip", color) {}

void SkipCard::play(UnoGame& game, Player* currentPlayer) {
    std::cout << "Skipping the next player's turn." << std::endl;
    game.skipTurn(); // Skip next player's turn in the game logic
}

void SkipCard::print() const {
    std::cout << colorToString(color) << " Skip";
}

std::string SkipCard::getName() const {
    return "Skip";
}

// Wildcard.cpp
WildCard::WildCard()
    : Card("Wild", deckColor::none) {}

void WildCard::play(UnoGame& game, Player* currentPlayer) {
    std::cout << "Wild Card played! Choose a color (0-Red, 1-Yellow, 2-Green, 3-Blue): ";
    int choice;
    if (!game.getIntegerInput(choice)) {
         std::cout << "Invalid input. Defaulting to Red." << std::endl;
         // Clear the error state of cin and ignore the rest of the line
         std::cin.clear();
         std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
         choice = 0; // Default to Red
    }

    switch(choice) {
        case 0: color = deckColor::red; break;
        case 1: color = deckColor::yellow; break;
        case 2: color = deckColor::green; break;
        case 3: color = deckColor::blue; break;
        default: color = deckColor::red; break; // Default to red if invalid input
    }

    std::cout << "Color changed to " << colorToString(color) << "!\n";
}

void WildCard::print() const {
    std::cout << "Wild Card";
    if (color != deckColor::none) {
        std::cout << " (" << colorToString(color) << ")";
    }
}

std::string WildCard::getName() const {
    return "Wild";
}

// Unogame.cpp
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
        for (int i = 0; i < 7; ++i) { // Each player draws 7 cards at the start
            player->drawCard(deck);
        }
    }

    // Draw the first card to start the game
    topCard = deck.drawCard();

    // If the first card is a special card, handle it
    if (dynamic_cast<WildCard*>(topCard) || dynamic_cast<DrawFourCard*>(topCard)) {
        // If it's a wild card, set a random color
       topCard->setColor(static_cast<deckColor>(rand() % 4));
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

        if (!getIntegerInput(choice)) {
            std::cout << "Invalid input. Please enter a number." << std::endl;
            continue; // Continue the loop to get valid input
        }


        if (choice == -2) {
            currentPlayer->callUNO();
             std::cout << "Choose a card to play (0-" << (currentPlayer->getHandSize() - 1)
                       << ") or enter -1 to draw: ";
             if (!getIntegerInput(choice)) {
                 std::cout << "Invalid input. Please enter a number." << std::endl;
                 continue; // Continue the loop to get valid input
             }
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
                   gameEnded = true; // Set game ended flag
                   return; // Exit the playTurn function
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

    if (!gameEnded) { // Only proceed to the next turn if the game hasn't ended
        nextTurn();
    }
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
        // Delete the player object and remove from the vector
        delete *it;
        players.erase(it);

        // Adjust currentPlayerIndex if needed
        if (currentPlayerIndex >= players.size()) {
            currentPlayerIndex = 0;
        }
    }
}

bool UnoGame::isGameOver() {
    if (gameEnded) return true; // Check the game ended flag
    for (auto& player : players) {
        if (player->hasWon()) {
            gameEnded = true; // Set flag if a player wins
            return true;
        }
    }
    if (players.size() <= 1) {
         gameEnded = true; // Set flag if only one player remains
         return true;
    }
    return false;
}

void UnoGame::enforceUNOCall(Player* player) {
    if (player->getHandSize() == 1 && !player->hasCalledUNOStatus()) {
        std::cout << player->getName() << " forgot to call UNO! Drawing 2 penalty cards." << std::endl;
        // Make the player draw two cards as penalty
        for (int i = 0; i < 2; ++i) {
            player->drawCard(deck);
        }
    }
    // Reset UNO status regardless if they had one card or not after playing
    player->resetUNOCall();
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

    // Check if they are the same type of action card (Skip, Reverse, DrawTwo, DrawSix, DropTwo)
    // Need to check exact type match if not color or number match
    // This is a simplified check; a more robust solution might involve RTTI or virtual match methods
    if (playedCard->getName() == topCard->getName() && playedCard->getColor() == topCard->getColor()) {
         return true; // Allow playing same action card on same color action card
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

    // Get the number of players
    int numPlayers;
    std::cout << "Enter the number of players (2-10): ";
    while (true) {
        if (getIntegerInput(numPlayers) && numPlayers >= 2 && numPlayers <= 10) {
            break;
        }
        std::cout << "Invalid number of players. Please enter a number between 2 and 10: ";
    }

    // Get player names
    for (int i = 0; i < numPlayers; ++i) {
        std::string name;
        std::cout << "Enter name for Player " << (i + 1) << ": ";
        std::cin >> name;
        addPlayer(new Player(name));
    }

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
        std::cout << "\n*** Game over with no clear winner ***" << std::endl;
    }
}

int UnoGame::getPlayerCount() const {
    return players.size();
}

// Helper function for safe integer input
bool UnoGame::getIntegerInput(int& output) {
    std::cin >> output;
    if (std::cin.fail()) {
        std::cin.clear(); // Clear the error flags
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Discard invalid input
        return false;
    }
    // Consume the rest of the line to prevent issues with subsequent inputs
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return true;
}


UnoGame::~UnoGame() {
    // Clean up the top card if it exists
    if (topCard) {
        delete topCard;
        topCard = nullptr;
    }

    // Clean up players (who own their hands)
    for (Player* player : players) {
        delete player;
    }
    players.clear();

    // Deck destructor handles its cards
}