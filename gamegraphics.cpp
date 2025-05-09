#include "../gamegraphics.hpp"

#include <iostream>

// --- Constructor / Destructor / Resources / Core Methods --- (Mostly Unchanged)
GameGraphics::GameGraphics(int width, int height, const char* title)
    : screenWidth(width), screenHeight(height) {
    SetConfigFlags(FLAG_FULLSCREEN_MODE | FLAG_VSYNC_HINT);
    InitWindow(screenWidth, screenHeight, title);
    screenWidth = GetScreenWidth();
    screenHeight = GetScreenHeight();
    SetTargetFPS(60);
    LoadResources();
    std::cout << "GameGraphics initialized (" << screenWidth << "x" << screenHeight << ")." << std::endl;
}

GameGraphics::~GameGraphics() {
    UnloadResources();
    CloseWindow();
    std::cout << "GameGraphics destroyed." << std::endl;
}

void GameGraphics::LoadResources() {
    defaultFont = GetFontDefault();
    std::cout << "Resources loaded." << std::endl;
}

void GameGraphics::UnloadResources() {
    std::cout << "Resources unloaded." << std::endl;
}

bool GameGraphics::WindowShouldCloseRequested() const {
    return WindowShouldClose();
}

void GameGraphics::BeginDrawingFrame(Color clearColor) {
    BeginDrawing();
    ClearBackground(clearColor);
}

void GameGraphics::EndDrawingFrame() {
    EndDrawing();
}

// --- Screen Drawing Methods --- (Welcome, PlayerSetup, Shuffling Unchanged)
void GameGraphics::DrawWelcomeScreen(Rectangle& nextButtonRect) {
    const char* titleText = "Welcome to Custom UNO!";
    int titleFontSize = 50;
    Vector2 titleSize = MeasureTextEx(defaultFont, titleText, titleFontSize, 1.0f);
    Vector2 titlePos = { (screenWidth - titleSize.x) / 2.0f, screenHeight * 0.1f };
    DrawTextEx(defaultFont, titleText, titlePos, titleFontSize, 1.0f, YELLOW);
    const char* instructions[] = {
        "Instructions:", "- Players: 2-4", "- Cards: 116 (Standard + 4 Draw Six + 4 Drop Two)",
        "- Custom Rules:", "  - Draw Six: Next player draws 6 cards.",
        "  - Drop Two: Choose a player to discard 2 cards (randomly).",
        "- Elimination: Player eliminated if hand reaches 25+ cards.",
        "- Goal: Be the last player remaining!", "- Type: Pass and Play"
    };
    int instructionFontSize = 25;
    float lineSpacing = instructionFontSize * 1.5f;
    Vector2 instructionStartPos = { screenWidth * 0.15f, screenHeight * 0.25f };
    for (int i = 0; i < sizeof(instructions) / sizeof(instructions[0]); ++i) {
        Vector2 pos = { instructionStartPos.x, instructionStartPos.y + i * lineSpacing };
        if (i > 0 && (instructions[i][0] == '-' || instructions[i][0] == ' ')) {
             pos.x += 30;
             DrawTextEx(defaultFont, instructions[i], pos, instructionFontSize * 0.9f, 1.0f, WHITE);
        } else {
             DrawTextEx(defaultFont, instructions[i], pos, instructionFontSize, 1.0f, LIGHTGRAY);
        }
    }
    float buttonWidth = 200; float buttonHeight = 60;
    Vector2 buttonPos = { (screenWidth - buttonWidth) / 2.0f, screenHeight * 0.85f };
    nextButtonRect = DrawButton(buttonPos, buttonWidth, buttonHeight, "Next", 30, BLUE, WHITE);
}

void GameGraphics::DrawPlayerSetupScreen(int& numPlayers, std::vector<std::string>& playerNames, Rectangle& startButtonRect) {
    const char* titleText = "Player Setup";
    int titleFontSize = 40;
    Vector2 titleSize = MeasureTextEx(defaultFont, titleText, titleFontSize, 1.0f);
    Vector2 titlePos = { (screenWidth - titleSize.x) / 2.0f, screenHeight * 0.1f };
    DrawTextEx(defaultFont, titleText, titlePos, titleFontSize, 1.0f, YELLOW);
    std::string numPlayersPrompt = "Enter Number of Players (2-4): " + (numPlayers > 0 ? std::to_string(numPlayers) : "_"); //add an exception for player number <2 or >4
    int promptFontSize = 30;
    Vector2 promptSize = MeasureTextEx(defaultFont, numPlayersPrompt.c_str(), promptFontSize, 1.0f);
    Vector2 promptPos = { (screenWidth - promptSize.x) / 2.0f, screenHeight * 0.3f };
    DrawTextEx(defaultFont, numPlayersPrompt.c_str(), promptPos, promptFontSize, 1.0f, WHITE);
    float namePromptY = screenHeight * 0.45f; float nameSpacing = 50.0f;
    for (int i = 0; i < numPlayers; ++i) {
        std::string namePrompt = "Player " + std::to_string(i + 1) + " Name: " + (i < playerNames.size() ? playerNames[i] : "_________");
        Vector2 namePromptSize = MeasureTextEx(defaultFont, namePrompt.c_str(), promptFontSize * 0.8f, 1.0f);
        Vector2 namePromptPos = { (screenWidth - namePromptSize.x) / 2.0f, namePromptY + i * nameSpacing };
        DrawTextEx(defaultFont, namePrompt.c_str(), namePromptPos, promptFontSize * 0.8f, 1.0f, WHITE);
    }
    float buttonWidth = 250; float buttonHeight = 60;
    Vector2 buttonPos = { (screenWidth - buttonWidth) / 2.0f, screenHeight * 0.85f };
    bool setupComplete = (numPlayers >= 2 && numPlayers <= 4 /* && playerNames.size() == numPlayers */);
    Color buttonColor = setupComplete ? GREEN : GRAY;
    const char* buttonText = setupComplete ? "Start Shuffling" : "Complete Setup";
    startButtonRect = DrawButton(buttonPos, buttonWidth, buttonHeight, buttonText, 30, buttonColor, WHITE, setupComplete); // Pass enabled state
}

void GameGraphics::DrawShufflingScreen() {
    const char* shuffleText = "Wait while cards are shuffled...";
    int fontSize = 40;
    Vector2 textSize = MeasureTextEx(defaultFont, shuffleText, fontSize, 1.0f);
    Vector2 textPos = { (screenWidth - textSize.x) / 2.0f, screenHeight * 0.3f };
    DrawTextEx(defaultFont, shuffleText, textPos, fontSize, 1.0f, WHITE);
    Vector2 logoCenter = { screenWidth / 2.0f, screenHeight / 2.0f };
    DrawUnoLogo(logoCenter, 4.0f);
}

// --- Gameplay Drawing Methods ---

// Updated DrawCard to include highlight
void GameGraphics::DrawCard(const Card& card, Vector2 position, float scale, bool highlighted) {
    float actualWidth = cardWidth * scale;
    float actualHeight = cardHeight * scale;
    float actualRadius = (actualWidth < actualHeight ? actualWidth : actualHeight) * cardCornerRadius;
    int actualFontSize = static_cast<int>(cardFontSize * scale);
    Rectangle cardRect = { position.x, position.y, actualWidth, actualHeight };
    Color baseColor = GetRaylibColor(card.color);

    // Draw main body
    DrawRectangleRounded(cardRect, actualRadius, 6, baseColor);

    // Draw inner white area
    if (card.color != Card::Color::BLACK) {
         Rectangle innerRect = {
             position.x + actualWidth * 0.1f, position.y + actualHeight * 0.075f,
             actualWidth * 0.8f, actualHeight * 0.85f
         };
         DrawRectangleRounded(innerRect, actualRadius * 0.8f, 6, WHITE);
    }

    // Draw text
    std::string cardText = GetCardText(card);
    Color textColor = (card.color == Card::Color::YELLOW) ? BLACK : WHITE;
    Color innerTextColor = (card.color == Card::Color::BLACK) ? WHITE : BLACK;
    Vector2 center = { position.x + actualWidth / 2, position.y + actualHeight / 2 };
    Vector2 textSize = MeasureTextEx(defaultFont, cardText.c_str(), actualFontSize, 1.0f);
    Vector2 textPosition = { center.x - textSize.x / 2, center.y - textSize.y / 2 };
    DrawTextEx(defaultFont, cardText.c_str(), textPosition, actualFontSize, 1.0f, innerTextColor);
    int cornerFontSize = actualFontSize * 0.7f;
    Vector2 topLeftPos = { position.x + actualWidth * 0.08f, position.y + actualHeight * 0.05f };
    DrawTextEx(defaultFont, cardText.c_str(), topLeftPos, cornerFontSize, 1.0f, textColor);

    // Draw border (thicker and yellow if highlighted)
    float borderThickness = highlighted ? 3.0f * scale : 1.5f * scale;
    Color borderColor = highlighted ? GOLD : LIGHTGRAY;
    DrawRectangleRoundedLines(cardRect, actualRadius, 6, borderThickness, borderColor);
}

void GameGraphics::DrawCardBack(Vector2 position, float scale) {
    float actualWidth = cardWidth * scale;
    float actualHeight = cardHeight * scale;
    float actualRadius = (actualWidth < actualHeight ? actualWidth : actualHeight) * cardCornerRadius;
    Rectangle cardRect = { position.x, position.y, actualWidth, actualHeight };
    DrawRectangleRounded(cardRect, actualRadius, 6, BLACK);
    Vector2 center = { position.x + actualWidth / 2.0f, position.y + actualHeight / 2.0f };
    DrawUnoLogo(center, scale);
    DrawRectangleRoundedLines(cardRect, actualRadius, 6, 1.5f * scale, DARKGRAY);
}

// Updated DrawPlayerHand to return rectangles and handle selection
std::vector<Rectangle> GameGraphics::DrawPlayerHand(const std::vector<Card>& hand, Vector2 startPosition, float spacing, bool showCards, int selectedIndex) {
    std::vector<Rectangle> cardRects;
    cardRects.reserve(hand.size()); // Optimize vector allocation
    Vector2 currentPosition = startPosition;

    for (int i = 0; i < hand.size(); ++i) {
        Rectangle cardRect = { currentPosition.x, currentPosition.y, cardWidth, cardHeight };
        cardRects.push_back(cardRect); // Store the rectangle for this card

        if (showCards) {
            DrawCard(hand[i], currentPosition, 1.0f, i == selectedIndex); // Pass highlight status
        } else {
            DrawCardBack(currentPosition, 1.0f);
        }
        currentPosition.x += spacing;
    }
    return cardRects; // Return the list of rectangles
}

// New: Draw opponent hands (simple version)
void GameGraphics::DrawOpponentHands(int numOpponents, int cardsPerOpponent, Vector2 startPosition, float spacing) {
     Vector2 currentPosition = startPosition;
     // This is highly simplified. A real game might show multiple rows or fan them out.
     // For now, just draw a block of card backs representing all opponent cards together.
     int totalOpponentCards = numOpponents * cardsPerOpponent;
     for (int i = 0; i < totalOpponentCards; ++i) {
         // Prevent drawing too many cards if space is limited
         if (currentPosition.x + cardWidth > screenWidth * 0.95f) break;
         DrawCardBack(currentPosition, 1.0f);
         currentPosition.x += spacing;
     }
     // Optionally draw text indicating number of opponents/cards
     std::string text = std::to_string(numOpponents) + " Opponent(s)";
     DrawText(text.c_str(), startPosition.x, startPosition.y - 20, 15, WHITE);

}


void GameGraphics::DrawDrawPile(Vector2 position, int cardCount, Rectangle& pileRect) {
     pileRect = { position.x, position.y, cardWidth, cardHeight }; // Output the rect
    if (cardCount > 0) {
        DrawCardBack(position, 1.0f);
        std::string countText = std::to_string(cardCount);
        int fontSize = 15;
        Vector2 textSize = MeasureTextEx(defaultFont, countText.c_str(), fontSize, 1.0f);
        Vector2 textPos = { position.x + (cardWidth - textSize.x) / 2, position.y + cardHeight + 5 };
        DrawTextEx(defaultFont, countText.c_str(), textPos, fontSize, 1.0f, WHITE);
    } else {
        DrawRectangleLinesEx(pileRect, 2.0f, LIGHTGRAY);
        DrawText("Empty", position.x + 5, position.y + cardHeight / 2 - 10, 20, LIGHTGRAY);
    }
}

void GameGraphics::DrawDiscardPile(const Card& topCard, Vector2 position) {
    // Assuming discard pile is never truly empty after start
    DrawCard(topCard, position);
}

// New: Draw Gameplay UI (Buttons, Turn Indicator)
void GameGraphics::DrawGameplayUI(int currentPlayerIndex, const std::string& playerName, bool cardSelected, Rectangle& drawButtonRect, Rectangle& playButtonRect) {
    // --- Turn Indicator ---
    std::string turnText = "Player " + std::to_string(currentPlayerIndex + 1) + "'s Turn";
    if (!playerName.empty()) {
        turnText = playerName + "'s Turn"; // Use name if available
    }
    int turnFontSize = 30;
    Vector2 turnTextSize = MeasureTextEx(defaultFont, turnText.c_str(), turnFontSize, 1.0f);
    Vector2 turnTextPos = { (screenWidth - turnTextSize.x) / 2.0f, screenHeight * 0.68f }; // Position above player hand
    DrawTextEx(defaultFont, turnText.c_str(), turnTextPos, turnFontSize, 1.0f, YELLOW);

    // --- Action Buttons ---
    float buttonWidth = 200;
    float buttonHeight = 50;
    float buttonSpacing = 30;
    float totalButtonWidth = buttonWidth * 2 + buttonSpacing;
    float buttonStartY = screenHeight * 0.60f; // Position buttons vertically

    // Draw Card Button
    Vector2 drawButtonPos = { (screenWidth - totalButtonWidth) / 2.0f, buttonStartY };
    drawButtonRect = DrawButton(drawButtonPos, buttonWidth, buttonHeight, "Draw Card", 24, BLUE, WHITE);

    // Play Selected Card Button (Enabled only if a card is selected)
    Vector2 playButtonPos = { drawButtonPos.x + buttonWidth + buttonSpacing, buttonStartY };
    playButtonRect = DrawButton(playButtonPos, buttonWidth, buttonHeight, "Play Selected Card", 24, GREEN, WHITE, cardSelected); // Pass enabled state
}


// --- UI Element Methods ---
// Updated DrawButton to handle enabled state
Rectangle GameGraphics::DrawButton(Vector2 position, float width, float height, const char* text, int fontSize, Color bgColor, Color textColor, bool enabled) {
    Rectangle buttonRect = { position.x, position.y, width, height };
    float cornerRadius = 0.2f;
    int segments = 6;

    Color currentBgColor = enabled ? bgColor : GRAY; // Dim background if disabled
    Color currentTextColor = enabled ? textColor : LIGHTGRAY; // Dim text if disabled

    DrawRectangleRounded(buttonRect, cornerRadius, segments, currentBgColor);
    DrawRectangleRoundedLines(buttonRect, cornerRadius, segments, 2.0f, Fade(BLACK, 0.3f));

    Vector2 textSize = MeasureTextEx(defaultFont, text, fontSize, 1.0f);
    Vector2 textPos = { position.x + (width - textSize.x) / 2.0f, position.y + (height - textSize.y) / 2.0f };
    DrawTextEx(defaultFont, text, textPos, fontSize, 1.0f, currentTextColor);

    return buttonRect;
}

// Updated IsButtonClicked to check if the button was enabled when drawn
bool GameGraphics::IsButtonClicked(Rectangle buttonRect) const {
     // Basic check - doesn't know if button was logically enabled,
     // relies on caller to check game state *before* acting on click.
    return CheckCollisionPointRec(GetMousePosition(), buttonRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

// --- Helper Methods --- (GetRaylibColor, GetCardText, DrawUnoLogo Unchanged)
Color GameGraphics::GetRaylibColor(Card::Color unoColor) const { /* ... unchanged ... */
    switch (unoColor) {
        case Card::Color::RED:    return RED;
        case Card::Color::YELLOW: return YELLOW;
        case Card::Color::GREEN:  return LIME;
        case Card::Color::BLUE:   return BLUE;
        case Card::Color::BLACK:  return BLACK;
        default:                  return GRAY;
    }
 }
std::string GameGraphics::GetCardText(const Card& card) const { /* ... unchanged ... */
    switch (card.type) {
        case Card::Type::ZERO: return "0"; case Card::Type::ONE: return "1";
        case Card::Type::TWO: return "2"; case Card::Type::THREE: return "3";
        case Card::Type::FOUR: return "4"; case Card::Type::FIVE: return "5";
        case Card::Type::SIX: return "6"; case Card::Type::SEVEN: return "7";
        case Card::Type::EIGHT: return "8"; case Card::Type::NINE: return "9";
        case Card::Type::SKIP: return "Skip"; case Card::Type::REVERSE: return "Rev";
        case Card::Type::DRAW_TWO: return "+2"; case Card::Type::WILD: return "Wild";
        case Card::Type::WILD_DRAW_FOUR: return "+4"; case Card::Type::DRAW_SIX: return "+6";
        case Card::Type::DROP_TWO: return "Drop2"; default: return "?";
    }
 }
void GameGraphics::DrawUnoLogo(Vector2 center, float scale) { /* ... unchanged ... */
    const char* logoText = "UNO";
    Color logoColor = RED;
    int fontSize = static_cast<int>(cardFontSize * 1.5f * scale);
    Vector2 textSize = MeasureTextEx(defaultFont, logoText, fontSize, 1.0f);
    Vector2 textPos = { center.x - textSize.x / 2.0f, center.y - textSize.y / 2.0f };
    DrawTextEx(defaultFont, logoText, textPos, fontSize, 1.0f, logoColor);
 }

