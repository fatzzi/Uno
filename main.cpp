#include "../gamegraphics.hpp"
#include <vector>
#include <string>
#include <iostream>
#include <numeric> // For std::iota
#include <random>  // For std::shuffle
#include <algorithm> // For std::shuffle, std::find_if
#include <chrono> // For seeding random generator

// --- Game State Structure ---
// Ideally, put this in GameState.h/.cpp
struct GameState {
    std::vector<Card> deck;
    std::vector<std::vector<Card>> playerHands;
    std::vector<Card> discardPile;
    int currentPlayerIndex = 0;
    int numPlayers = 0;
    std::vector<std::string> playerNames; // Optional names
    bool gameInitialized = false;
    int selectedCardIndex = -1; // Index of the card selected by the current player in their hand
    // Add other state: direction, mustDraw, chosenColor (for Wilds), etc.
};

// --- Helper Function Prototypes ---
void InitializeGameState(GameState& state);
void CreateDeck(std::vector<Card>& deck);
// Add functions for PlayCard, DrawCardFromDeck, NextTurn etc. later

// --- Main Function ---
int main() {
    // --- Initialization ---
    const int initialScreenWidth = 1024;
    const int initialScreenHeight = 768;
    GameGraphics graphics(initialScreenWidth, initialScreenHeight, "Custom UNO Game");

    // --- Game State Variables ---
    GameScreen currentScreen = GameScreen::WELCOME;
    GameState gameState; // Create the game state object

    // --- UI Element Rectangles ---
    Rectangle nextButtonRect = {0};
    Rectangle startButtonRect = {0};
    Rectangle drawPileRect = {0};
    Rectangle drawButtonRect = {0};
    Rectangle playButtonRect = {0};
    std::vector<Rectangle> currentHandCardRects; // Rects for cards in the current player's hand

    // --- Timer for Shuffling Screen ---
    float shufflingStartTime = 0.0f;
    const float SHUFFLING_DURATION = 3.0f; // Shortened shuffling time


    // --- Main Game Loop ---
    while (!graphics.WindowShouldCloseRequested()) {

        // --- Update (Input & State Logic) ---
        float currentTime = GetTime();
        Vector2 mousePos = GetMousePosition();

        switch (currentScreen) {
            case GameScreen::WELCOME:
                if (graphics.IsButtonClicked(nextButtonRect)) {
                    currentScreen = GameScreen::PLAYER_SETUP;
                }
                break;

            case GameScreen::PLAYER_SETUP:
                // Basic Input Simulation (Replace with real input)
                if (IsKeyPressed(KEY_TWO)) gameState.numPlayers = 2;
                if (IsKeyPressed(KEY_THREE)) gameState.numPlayers = 3;
                if (IsKeyPressed(KEY_FOUR)) gameState.numPlayers = 4;
                // TODO: Input player names into gameState.playerNames

                bool setupComplete = (gameState.numPlayers >= 2 && gameState.numPlayers <= 4 /* && gameState.playerNames.size() == gameState.numPlayers */);
                if (setupComplete && graphics.IsButtonClicked(startButtonRect)) {
                    currentScreen = GameScreen::SHUFFLING;
                    shufflingStartTime = currentTime;
                    // Assign default names if none entered
                    if (gameState.playerNames.empty()) {
                        for(int i = 0; i < gameState.numPlayers; ++i) {
                            gameState.playerNames.push_back("Player " + std::to_string(i+1));
                        }
                    }
                }
                break;

            case GameScreen::SHUFFLING:
                if (currentTime - shufflingStartTime >= SHUFFLING_DURATION) {
                    InitializeGameState(gameState); // Setup deck, hands, etc.
                    if (gameState.gameInitialized) {
                        currentScreen = GameScreen::GAMEPLAY;
                        std::cout << "Game started!" << std::endl;
                    } else {
                        // Handle error - e.g., couldn't find valid start card
                        std::cerr << "Error initializing game state!" << std::endl;
                        // Maybe go back to WELCOME or show an error screen
                        currentScreen = GameScreen::WELCOME;
                    }
                }
                break;

            case GameScreen::GAMEPLAY:
                if (!gameState.gameInitialized) break;

                // --- Gameplay Input ---
                int currentPlayer = gameState.currentPlayerIndex;
                bool cardClickedThisFrame = false;

                // 1. Check for clicks on player hand cards
                if (!currentHandCardRects.empty() && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    for (int i = 0; i < currentHandCardRects.size(); ++i) {
                        // Check collision with the *correct* rectangle for the i-th card
                        if (CheckCollisionPointRec(mousePos, currentHandCardRects[i])) {
                            // If clicking the already selected card, deselect it. Otherwise, select the new card.
                            gameState.selectedCardIndex = (gameState.selectedCardIndex == i) ? -1 : i;
                            cardClickedThisFrame = true;
                            std::cout << "Card " << i << (gameState.selectedCardIndex == i ? " selected." : " deselected.") << std::endl;
                            break; // Stop checking once a card is clicked
                        }
                    }
                }

                // 2. Check for Draw Card button click
                if (graphics.IsButtonClicked(drawButtonRect)) {
                     std::cout << "Draw Card button clicked!" << std::endl;
                     // --- TODO: Implement Draw Card Logic ---
                     // - Check if drawing is allowed (e.g., player hasn't played yet)
                     // - Get card from gameState.deck
                     // - Add card to gameState.playerHands[currentPlayer]
                     // - Remove card from gameState.deck
                     // - Potentially end turn or allow playing the drawn card (depending on rules)
                     // - Deselect any selected card: gameState.selectedCardIndex = -1;
                     // --- End Draw Card Logic ---
                }

                // 3. Check for Play Selected Card button click (only if a card is selected)
                if (gameState.selectedCardIndex != -1 && graphics.IsButtonClicked(playButtonRect)) {
                    std::cout << "Play Selected Card button clicked for card index: " << gameState.selectedCardIndex << std::endl;
                    // --- TODO: Implement Play Card Logic ---
                    // - Get the selected card: Card cardToPlay = gameState.playerHands[currentPlayer][gameState.selectedCardIndex];
                    // - Get the top discard card: Card topDiscard = gameState.discardPile.back();
                    // - Check if cardToPlay is valid to play on topDiscard (color, type, or wild)
                    // - If valid:
                    //   - Add cardToPlay to gameState.discardPile
                    //   - Remove cardToPlay from gameState.playerHands[currentPlayer]
                    //   - Handle card effects (Skip, Reverse, Draw+, Wild color choice)
                    //   - Check for win condition (hand empty)
                    //   - Advance to next turn (update gameState.currentPlayerIndex, handle direction)
                    // - If not valid:
                    //   - Maybe show an error message (e.g., using a temporary text display)
                    // - Deselect the card after attempting to play: gameState.selectedCardIndex = -1;
                    // --- End Play Card Logic ---
                }

                 // 4. Check for Draw Pile click (alternative way to draw)
                 if (!cardClickedThisFrame && graphics.IsButtonClicked(drawPileRect)) {
                     std::cout << "Draw Pile clicked!" << std::endl;
                     // --- TODO: Implement Draw Card Logic (same as button click) ---
                 }


                // --- Gameplay Logic Update ---
                // TODO: AI turns, checking elimination condition (card count >= 25), etc.

                break; // End GAMEPLAY case
        } // End switch(currentScreen)


        // --- Draw ---
        Color bgColor = DARKBLUE;
        if (currentScreen == GameScreen::GAMEPLAY) bgColor = DARKGREEN;
        else if (currentScreen == GameScreen::SHUFFLING) bgColor = BLACK;
        graphics.BeginDrawingFrame(bgColor);

        switch (currentScreen) {
            case GameScreen::WELCOME:
                graphics.DrawWelcomeScreen(nextButtonRect);
                break;
            case GameScreen::PLAYER_SETUP:
                // Pass player count by reference for drawing
                graphics.DrawPlayerSetupScreen(gameState.numPlayers, gameState.playerNames, startButtonRect);
                break;
            case GameScreen::SHUFFLING:
                graphics.DrawShufflingScreen();
                break;
            case GameScreen::GAMEPLAY:
                if (!gameState.gameInitialized) break;

                // --- Draw Gameplay Elements ---
                int screenW = graphics.GetScreenWidth();
                int screenH = graphics.GetScreenHeight();
                int currentPlayer = gameState.currentPlayerIndex;

                // Draw Player's Hand (bottom) - Get card rectangles back
                Vector2 playerHandPos = { screenW * 0.1f, screenH * 0.75f };
                float playerHandSpacing = graphics.cardWidth * 0.7f; // Overlap cards slightly
                 // Adjust spacing if hand is very large to prevent going off screen
                 if (!gameState.playerHands[currentPlayer].empty()) {
                     float requiredWidth = graphics.cardWidth + (gameState.playerHands[currentPlayer].size() - 1) * playerHandSpacing;
                     float availableWidth = screenW * 0.8f; // Use 80% of width
                     if (requiredWidth > availableWidth) {
                         playerHandSpacing = (availableWidth - graphics.cardWidth) / (gameState.playerHands[currentPlayer].size() - 1);
                     }
                     // Center the hand horizontally
                     playerHandPos.x = (screenW - (graphics.cardWidth + (gameState.playerHands[currentPlayer].size() - 1) * playerHandSpacing)) / 2.0f;

                 }

                currentHandCardRects = graphics.DrawPlayerHand(
                    gameState.playerHands[currentPlayer],
                    playerHandPos,
                    playerHandSpacing,
                    true, // Show face up
                    gameState.selectedCardIndex // Pass selected index for highlighting
                );

                // Draw Opponent Hands (top - simplified)
                Vector2 opponentHandPos = { screenW * 0.1f, screenH * 0.05f };
                float opponentHandSpacing = graphics.cardWidth * 0.2f; // Tighter spacing
                int opponentCount = gameState.numPlayers - 1;
                // Calculate average cards per opponent (approximate for display)
                 int totalOpponentCards = 0;
                 for(int i = 0; i < gameState.numPlayers; ++i) {
                     if (i != currentPlayer) totalOpponentCards += gameState.playerHands[i].size();
                 }
                 int avgCards = (opponentCount > 0) ? (totalOpponentCards / opponentCount) : 0;
                 // Draw placeholder card backs for opponents
                 graphics.DrawOpponentHands(opponentCount, avgCards, opponentHandPos, opponentHandSpacing);


                // Draw Piles
                Vector2 drawPilePos = { screenW * 0.35f, screenH * 0.35f }; // Adjusted positions
                Vector2 discardPilePos = { screenW * 0.55f, screenH * 0.35f }; // Adjusted positions

                graphics.DrawDrawPile(drawPilePos, gameState.deck.size(), drawPileRect); // Get draw pile rect

                if (!gameState.discardPile.empty()) {
                    graphics.DrawDiscardPile(gameState.discardPile.back(), discardPilePos);
                } else {
                    // Should not happen after init, but draw placeholder just in case
                    Rectangle pileRect = { discardPilePos.x, discardPilePos.y, graphics.cardWidth, graphics.cardHeight };
                    DrawRectangleLinesEx(pileRect, 2.0f, LIGHTGRAY);
                }

                // Draw UI (Turn indicator, buttons)
                graphics.DrawGameplayUI(
                    currentPlayer,
                    gameState.playerNames[currentPlayer], // Pass player name
                    gameState.selectedCardIndex != -1, // Pass card selected status
                    drawButtonRect, // Get button rects back
                    playButtonRect
                 );

                break; // End GAMEPLAY draw case
        } // End switch(currentScreen) draw

        graphics.EndDrawingFrame();
    } // End main game loop

    // --- Cleanup --- handled by graphics destructor
    return 0;
}


// --- Helper Function Implementations ---

void CreateDeck(std::vector<Card>& deck) {
    deck.clear();
    deck.reserve(116); // Reserve space for efficiency

    const Card::Color colors[] = { Card::Color::RED, Card::Color::BLUE, Card::Color::GREEN, Card::Color::YELLOW };
    const Card::Type numberTypes[] = {
        Card::Type::ONE, Card::Type::TWO, Card::Type::THREE, Card::Type::FOUR, Card::Type::FIVE,
        Card::Type::SIX, Card::Type::SEVEN, Card::Type::EIGHT, Card::Type::NINE
    };
    const Card::Type actionTypes[] = { Card::Type::SKIP, Card::Type::REVERSE, Card::Type::DRAW_TWO };
    const Card::Type customTypes[] = { Card::Type::DRAW_SIX, Card::Type::DROP_TWO }; // Your custom cards

    for (Card::Color color : colors) {
        // One Zero card
        deck.push_back({color, Card::Type::ZERO});
        // Two of each number card (1-9)
        for (Card::Type type : numberTypes) {
            deck.push_back({color, type});
            deck.push_back({color, type});
        }
        // Two of each action card
        for (Card::Type type : actionTypes) {
            deck.push_back({color, type});
            deck.push_back({color, type});
        }
         // One of each custom card per color (assuming 4 total of each custom type)
        for (Card::Type type : customTypes) {
             deck.push_back({color, type});
        }
    }

    // Four Wild cards
    for (int i = 0; i < 4; ++i) {
        deck.push_back({Card::Color::BLACK, Card::Type::WILD});
    }
    // Four Wild Draw Four cards
    for (int i = 0; i < 4; ++i) {
        deck.push_back({Card::Color::BLACK, Card::Type::WILD_DRAW_FOUR});
    }

     std::cout << "Deck created with " << deck.size() << " cards." << std::endl;
     // Expected: 4*(1 Zero + 2*9 Numbers + 2*3 Actions + 1*2 Custom) + 4 Wild + 4 WildDrawFour
     // = 4*(1 + 18 + 6 + 2) + 4 + 4 = 4*(27) + 8 = 108 + 8 = 116. Correct.
}

void InitializeGameState(GameState& state) {
    std::cout << "Initializing game state..." << std::endl;
    state.gameInitialized = false; // Start as not initialized
    state.deck.clear();
    state.playerHands.clear();
    state.discardPile.clear();
    state.currentPlayerIndex = 0; // Player 1 starts
    state.selectedCardIndex = -1;

    // 1. Create the standard deck
    CreateDeck(state.deck);

    // 2. Shuffle the deck
    // Use a time-based seed for better randomness
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::shuffle(state.deck.begin(), state.deck.end(), std::default_random_engine(seed));
    std::cout << "Deck shuffled." << std::endl;


    // 3. Deal cards to players
    state.playerHands.resize(state.numPlayers);
    for (int i = 0; i < 7; ++i) { // Deal 7 cards each
        for (int p = 0; p < state.numPlayers; ++p) {
            if (state.deck.empty()) {
                std::cerr << "Error: Deck ran out during initial deal!" << std::endl;
                return; // Cannot initialize game
            }
            state.playerHands[p].push_back(state.deck.back());
            state.deck.pop_back();
        }
    }
     for (int p = 0; p < state.numPlayers; ++p) {
         std::cout << "Player " << (p+1) << " dealt " << state.playerHands[p].size() << " cards." << std::endl;
     }


    // 4. Initialize discard pile
    // Flip cards from deck until a non-action/non-wild card is found
    while (!state.deck.empty()) {
        Card topCard = state.deck.back();
        state.deck.pop_back();
        if (!topCard.IsActionCard()) { // Check if it's a number card
            state.discardPile.push_back(topCard);
             std::cout << "Discard pile started with: " << GameGraphics::GetCardText(topCard) << " (" << static_cast<int>(topCard.color) << ")" << std::endl;
            break; // Found a valid starting card
        } else {
            // If it's an action/wild card, put it back in the middle of the deck and reshuffle slightly
            // (Simpler: just put back at bottom for now, will get shuffled later if needed)
             std::cout << "Skipping Action/Wild card for initial discard: " << GameGraphics::GetCardText(topCard) << std::endl;
            state.deck.insert(state.deck.begin() + (state.deck.size() / 2), topCard); // Put back somewhere in deck
        }
    }

    // Check if we successfully started the discard pile
    if (state.discardPile.empty()) {
        std::cerr << "Error: Could not find a valid starting card in the deck!" << std::endl;
         // This might happen if the deck is built incorrectly or has too many action cards.
         // Consider reshuffling the entire deck and trying again, or handle error.
        return; // Cannot initialize game
    }


    state.gameInitialized = true;
    std::cout << "Game state initialized successfully. Deck size: " << state.deck.size() << std::endl;
}
