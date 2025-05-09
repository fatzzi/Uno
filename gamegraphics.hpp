#ifndef GAMEGRAPHICS_H
#define GAMEGRAPHICS_H

#include "raylib.h"
#include <vector>
#include <string>
#include <vector>

// --- Game State Enum ---
enum class GameScreen {
    WELCOME,
    PLAYER_SETUP,
    SHUFFLING,
    GAMEPLAY
};

// --- Forward Declarations ---
// Assume Card struct is defined (as before)
struct Card {
    enum class Color { RED, YELLOW, GREEN, BLUE, BLACK } color;
    enum class Type {
        ZERO, ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE,
        SKIP, REVERSE, DRAW_TWO,
        WILD, WILD_DRAW_FOUR,
        DRAW_SIX, DROP_TWO
    } type;
     // Helper to check if a card is an action card (for initial discard)
    bool IsActionCard() const {
        return type == Type::SKIP || type == Type::REVERSE || type == Type::DRAW_TWO ||
               type == Type::WILD || type == Type::WILD_DRAW_FOUR ||
               type == Type::DRAW_SIX || type == Type::DROP_TWO;
    }
};

// --- GameGraphics Class Definition ---
class GameGraphics {
public:
    // --- Constructor & Destructor ---
    GameGraphics(int screenWidth, int screenHeight, const char* title);
    ~GameGraphics();

    // --- Core Methods ---
    bool WindowShouldCloseRequested() const;
    void BeginDrawingFrame(Color clearColor = DARKGREEN);
    void EndDrawingFrame();

    // --- Screen Drawing Methods ---
    void DrawWelcomeScreen(Rectangle& nextButtonRect);
    void DrawPlayerSetupScreen(int& numPlayers, std::vector<std::string>& playerNames, Rectangle& startButtonRect);
    void DrawShufflingScreen();

    // --- Gameplay Drawing Methods ---
    void DrawCard(const Card& card, Vector2 position, float scale = 1.0f, bool highlighted = false); // Added highlight option
    // Draws a player's hand and returns rectangles for click detection. Highlights the selected card.
    std::vector<Rectangle> DrawPlayerHand(const std::vector<Card>& hand, Vector2 startPosition, float spacing, bool showCards, int selectedIndex = -1);
    void DrawDrawPile(Vector2 position, int cardCount, Rectangle& pileRect); // Output pile rect
    void DrawDiscardPile(const Card& topCard, Vector2 position);
    // Draws opponent hands (simplified, just card backs)
    void DrawOpponentHands(int numOpponents, int cardsPerOpponent, Vector2 startPosition, float spacing);
    // Draws the main gameplay UI elements (buttons, turn indicator)
    void DrawGameplayUI(int currentPlayer, const std::string& playerName, bool cardSelected, Rectangle& drawButtonRect, Rectangle& playButtonRect);


    // --- UI Element Methods ---
    Rectangle DrawButton(Vector2 position, float width, float height, const char* text, int fontSize, Color bgColor, Color textColor, bool enabled = true); // Added enabled state
    bool IsButtonClicked(Rectangle buttonRect) const;

    // --- Helper Methods ---
    Color GetRaylibColor(Card::Color unoColor) const;
    std::string GetCardText(const Card& card) const;

    // --- Public Accessors ---
    int GetScreenWidth() const { return screenWidth; }
    int GetScreenHeight() const { return screenHeight; }
    Font GetDefaultFont() const { return defaultFont; }

    // --- Card Visual Properties ---
    const float cardWidth = 70.0f;
    const float cardHeight = 100.0f;

private:
    // --- Member Variables ---
    int screenWidth;
    int screenHeight;
    Font defaultFont;

    // --- Card Visual Properties ---
    const float cardCornerRadius = 0.1f;
    const int cardFontSize = 20;

    // --- Private Helper Methods ---
    void LoadResources();
    void UnloadResources();
    void DrawCardBack(Vector2 position, float scale);
    void DrawUnoLogo(Vector2 center, float scale);
};

#endif /
