#include "..\headers\Game.h" 
#include <iostream> 
#include <algorithm>
#include <sstream> 
// Combined_uno.h is included via Game.h, so direct include here might be redundant
// depending on your include guards and project structure.
// If Game.h is in a different directory, adjust the #include "Game.h" path.

// Button implementation
Button::Button(float x, float y, float width, float height, const std::string& buttonText, 
               int textSize, Color buttonColor, Color buttonHoverColor, Color buttonTextColor)
    : bounds({x, y, width, height}), text(buttonText), fontSize(textSize),
      color(buttonColor), hoverColor(buttonHoverColor), textColor(buttonTextColor), enabled(true) {}

bool Button::isClicked() const {
    if (!enabled) return false;
    return IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && 
           CheckCollisionPointRec(GetMousePosition(), bounds);
}

bool Button::isHovered() const {
    if (!enabled) return false;
    return CheckCollisionPointRec(GetMousePosition(), bounds);
}

void Button::draw() const {
    Color currentDrawColor = isHovered() && enabled ? hoverColor : color;
    
    if (!enabled) {
        currentDrawColor.a = 120; 
    }
    
    DrawRectangleRec(bounds, currentDrawColor);
    DrawRectangleLinesEx(bounds, 2, enabled ? ::BLACK : ::DARKGRAY);
    
    float textWidth = MeasureText(text.c_str(), fontSize);
    float textX = bounds.x + (bounds.width - textWidth) / 2;
    float textY = bounds.y + (bounds.height - fontSize) / 2;
    
    DrawText(text.c_str(), (int)textX, (int)textY, fontSize, enabled ? textColor : ::GRAY);
}

void Button::setEnabled(bool isEnabled) {
    enabled = isEnabled;
}

bool Button::isEnabled() const {
    return enabled;
}

void Button::setText(const std::string& newText) {
    text = newText;
}

// TextBox implementation
TextBox::TextBox(float x, float y, float width, float height, int textSize, 
                Color boxBgColor, Color boxTextColor, size_t maxTextLength)
    : bounds({x, y, width, height}), text(""), fontSize(textSize),
      bgColor(boxBgColor), textColor(boxTextColor), isActive(false), maxLength(maxTextLength) {}

void TextBox::update() {
    if (!isActive) return;
    
    int key = GetCharPressed();
    while (key > 0) { 
        if ((key >= 32) && (key <= 125) && (text.length() < maxLength)) {
            text += (char)key;
        }
        key = GetCharPressed(); 
    }
    
    if (IsKeyPressed(KEY_BACKSPACE) && !text.empty()) {
        text.pop_back();
    }
}

void TextBox::draw() const {
    DrawRectangleRec(bounds, bgColor);
    DrawRectangleLinesEx(bounds, 1, isActive ? ::RED : ::DARKGRAY); 
    
    float textX = bounds.x + 5; 
    float textY = bounds.y + (bounds.height - fontSize) / 2;
    
    DrawText(text.c_str(), (int)textX, (int)textY, fontSize, textColor);
    
    if (isActive && ( (int)(GetTime() * 2.0f) % 2 == 0 ) ) { 
        float cursorX = textX + MeasureText(text.c_str(), fontSize);
        // Ensure cursor does not go outside the textbox bounds too much
        if (cursorX < bounds.x + bounds.width - 5) { 
            DrawRectangle((int)cursorX, (int)(bounds.y + 4), 2, (int)(bounds.height - 8), textColor);
        }
    }
}

std::string TextBox::getText() const {
    return text;
}

void TextBox::setText(const std::string& newText) {
    if (newText.length() <= maxLength) {
        text = newText;
    } else {
        text = newText.substr(0, maxLength);
    }
}

void TextBox::setActive(bool active) {
    isActive = active;
}

bool TextBox::isActiveBox() const {
    return isActive;
}

// CardRenderer implementation
Color CardRenderer::getRaylibColorForGameEnum(enum deckColor gameCardColor) { 
    switch (gameCardColor) {
        case deckColor::red:    return ::RED;    
        case deckColor::green:  return ::GREEN;  
        case deckColor::blue:   return ::BLUE;   
        case deckColor::yellow: return ::YELLOW; 
        case deckColor::none:   return ::LIGHTGRAY; 
        default:            return ::BLACK;  
    }
}

void CardRenderer::drawCard(const Card* card, float x, float y, float width, float height, bool selected) {
    Color cardBgColor = ::WHITE; 
    Color cardTxtColor = ::BLACK; 
    std::string cardTextStr;
    
    if (card) {
        cardBgColor = getRaylibColorForGameEnum(card->getColor());
        
        if (card->getColor() != deckColor::none) { 
             cardBgColor.r = (unsigned char)std::min(255, cardBgColor.r + 60);
             cardBgColor.g = (unsigned char)std::min(255, cardBgColor.g + 60);
             cardBgColor.b = (unsigned char)std::min(255, cardBgColor.b + 60);
        }

        const NumberCard* numCard = dynamic_cast<const NumberCard*>(card);
        const WildCard* wildCard = dynamic_cast<const WildCard*>(card);
        const DrawFourCard* draw4Card = dynamic_cast<const DrawFourCard*>(card);
        const DrawTwoCard* draw2Card = dynamic_cast<const DrawTwoCard*>(card);
        const DrawSixCard* draw6Card = dynamic_cast<const DrawSixCard*>(card); 
        const SkipCard* skipCard = dynamic_cast<const SkipCard*>(card);
        const ReverseCard* revCard = dynamic_cast<const ReverseCard*>(card);
        const DropTwoCard* drop2Card = dynamic_cast<const DropTwoCard*>(card); 
        
        if (numCard) {
            cardTextStr = std::to_string(numCard->getNumber());
        } else if (wildCard) {
            cardTextStr = "Wild";
            if (card->getColor() != deckColor::none) { 
                cardTextStr += "\n(" + colorToString(card->getColor()) + ")"; 
            }
        } else if (draw4Card) {
            cardTextStr = "Wild\nDraw 4";
            if (card->getColor() != deckColor::none) { 
                cardTextStr += "\n(" + colorToString(card->getColor()) + ")";
            }
        } else if (draw2Card) {
            cardTextStr = "Draw 2";
        } else if (draw6Card) {
            cardTextStr = "Draw 6";
        } else if (skipCard) {
            cardTextStr = "Skip";
        } else if (revCard) {
            cardTextStr = "Reverse";
        } else if (drop2Card) {
            cardTextStr = "Drop 2";
        } else if (card->getName() != "NumberCard") { // Avoid "NumberCard" if not caught by specific types
             cardTextStr = card->getName();
        } else {
            cardTextStr = ""; // Default for unknown number cards if any
        }
    } else { 
        cardBgColor = ::DARKGRAY;
        cardTextStr = "UNO";
    }
    
    if (selected) {
        DrawRectangle((int)(x - 3), (int)(y - 3), (int)(width + 6), (int)(height + 6), ::GOLD); 
    }
    
    DrawRectangleRounded({x, y, width, height}, 0.1f, 8, cardBgColor);
DrawRectangleRoundedLines({x, y, width, height}, 0.1f, 8, ::BLACK);
    
    int fontSize = (cardTextStr.find('\n') != std::string::npos || cardTextStr.length() > 7) ? 16 : 22;
    if (cardTextStr.length() > 10 && cardTextStr.find('\n') == std::string::npos) fontSize = 14;


    if (cardTextStr.find('\n') != std::string::npos) {
        std::string line1 = cardTextStr.substr(0, cardTextStr.find('\n'));
        std::string line2 = cardTextStr.substr(cardTextStr.find('\n') + 1);
        float text1Width = MeasureText(line1.c_str(), fontSize);
        float text2Width = MeasureText(line2.c_str(), fontSize);

        float text1X = x + (width - text1Width) / 2;
        float text2X = x + (width - text2Width) / 2;
        // Adjust Y position for two lines to be more centered
        float line1Y = y + (height / 2) - fontSize; 
        float line2Y = y + (height / 2) + 2;    

        DrawText(line1.c_str(), (int)text1X, (int)line1Y, fontSize, cardTxtColor);
        DrawText(line2.c_str(), (int)text2X, (int)line2Y, fontSize, cardTxtColor);
    } else if (!cardTextStr.empty()){
        float textWidth = MeasureText(cardTextStr.c_str(), fontSize);
        float textX = x + (width - textWidth) / 2;
        float textY = y + (height - fontSize) / 2;
        DrawText(cardTextStr.c_str(), (int)textX, (int)textY, fontSize, cardTxtColor);
    }
}


// Patch to fix UnoGUI constructor in Game.cpp
// Find the UnoGUI constructor in Game.cpp and replace it with this version

UnoGUI::UnoGUI() 
    : currentScreen(GameScreen::TITLE),
      numPlayers(0),
      numPlayerInput(GetScreenWidth() / 2.0f - 125, 300, 250, 40, 24, ::WHITE, ::BLACK, 2), 
      startGameButton(GetScreenWidth() / 2.0f - 100, 360, 200, 50, "Start Setup", 24, ::LIGHTGRAY, ::GRAY, ::BLACK),
      unoButton(50, GetScreenHeight() - 70.0f, 120, 50, "Call UNO!", 20, ::RED, ::RED, ::WHITE),
      throwCardButton(180, GetScreenHeight() - 70.0f, 150, 50, "Throw Card", 20, ::GREEN, ::DARKGREEN, ::WHITE),
      drawCardButton(340, GetScreenHeight() - 70.0f, 150, 50, "Draw Card", 20, ::BLUE, ::DARKBLUE, ::WHITE),
      continueButton(GetScreenWidth() / 2.0f - 75, GetScreenHeight() / 2.0f + 50, 150, 50, "Continue", 24, ::PURPLE),
      redColorButton(0, 0, 100, 50, "RED", 20, ::RED), 
      greenColorButton(0, 0, 100, 50, "GREEN", 20, ::GREEN),
      blueColorButton(0, 0, 100, 50, "BLUE", 20, ::BLUE),   
      yellowColorButton(0, 0, 100, 50, "YELLOW", 20, ::YELLOW), 
      cardPendingColorChoice(nullptr),
      indexOfCardPendingColorChoice(-1),
      waitingForNextPlayer(false),
      selectedCardIndex(-1),
      gameOverState(false)
{
    // Make sure the colors are fully visible
    unoButton = Button(50, GetScreenHeight() - 70.0f, 120, 50, "Call UNO!", 20, ::RED, ::RED, ::WHITE);
    throwCardButton = Button(180, GetScreenHeight() - 70.0f, 150, 50, "Throw Card", 20, ::GREEN, ::DARKGREEN, ::WHITE);
    drawCardButton = Button(340, GetScreenHeight() - 70.0f, 150, 50, "Draw Card", 20, ::BLUE, ::DARKBLUE, ::WHITE);
    
    game = std::make_unique<UnoGame>();
    playerNameInputs.resize(10); 
    for(size_t i = 0; i < playerNameInputs.size(); ++i) { 
        playerNameInputs[i] = std::make_unique<TextBox>(); 
    }
}
UnoGUI::~UnoGUI() {}

// Patch to fix UnoGUI::run method in Game.cpp
// Find the UnoGUI::run method in Game.cpp and replace it with this version

void UnoGUI::run() {
    // Remove InitWindow since we're going to handle it here
    InitWindow(800, 600, "Custom UNO - Raylib GUI");
    SetTargetFPS(60);
   
    // Update button positions after window is initialized
    unoButton = Button(50, GetScreenHeight() - 70.0f, 120, 50, "Call UNO!", 20, ::RED, ::RED, ::WHITE);
    throwCardButton = Button(180, GetScreenHeight() - 70.0f, 150, 50, "Throw Card", 20, ::GREEN, ::DARKGREEN, ::WHITE);
    drawCardButton = Button(340, GetScreenHeight() - 70.0f, 150, 50, "Draw Card", 20, ::BLUE, ::DARKBLUE, ::WHITE);
    
    // Initial button/textbox positions for title screen
    numPlayerInput = TextBox(GetScreenWidth() / 2.0f - 125, 280, 250, 40, 20, ::WHITE, ::BLACK, 2);
    startGameButton = Button(GetScreenWidth()/2.0f - 100, 340, 200, 50, "Start Setup", 24, ::LIGHTGRAY, ::GRAY, ::BLACK);

    while (!WindowShouldClose()) {
        // Update logic
        switch (currentScreen) {
            case GameScreen::TITLE:
                handleTitleScreenInput();
                numPlayerInput.update(); // Update active textbox
                break;
            case GameScreen::PLAYER_SETUP:
                handlePlayerSetupScreenInput();
                for(int i=0; i < numPlayers; ++i) { // Update active player name textbox
                    if(playerNameInputs[i] && playerNameInputs[i]->isActiveBox()) playerNameInputs[i]->update();
                }
                break;
            case GameScreen::GAMEPLAY:
                if (cardPendingColorChoice) { 
                    handleColorPickerInput();
                } else {
                    handleGameplayScreenInput();
                }
                break;
            case GameScreen::GAME_OVER:
                handleGameOverScreenInput();
                break;
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);
        
        switch (currentScreen) {
            case GameScreen::TITLE:
                drawTitleScreen();
                break;
            case GameScreen::PLAYER_SETUP:
                drawPlayerSetupScreen();
                break;
            case GameScreen::GAMEPLAY:
                drawGameplayScreen(); 
                break;
            case GameScreen::GAME_OVER:
                drawGameOverScreen();
                break;
        }
        
        EndDrawing();
    }
    
    CloseWindow();
}

void UnoGUI::drawTitleScreen() {
    DrawText("Welcome to Custom UNO!", GetScreenWidth() / 2 - MeasureText("Welcome to Custom UNO!", 40) / 2, 150, 40, ::DARKGRAY);
    DrawText("Enter Number of Players (2-10):", GetScreenWidth() / 2 - MeasureText("Enter Number of Players (2-10):", 20) / 2, 250, 20, ::GRAY);
    
    numPlayerInput.draw(); 
    startGameButton.draw(); 

    if (!gameMessage.empty()) {
        DrawText(gameMessage.c_str(), 20, GetScreenHeight() - 40, 20, ::RED);
    }
}

void UnoGUI::handleTitleScreenInput() {
    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        if (CheckCollisionPointRec(GetMousePosition(), numPlayerInput.bounds)) {
            numPlayerInput.setActive(true);
        } else {
            numPlayerInput.setActive(false);
        }
    }
    
    if (startGameButton.isClicked()) {
        gameMessage = ""; // Clear previous messages
        try {
            if (!numPlayerInput.getText().empty()){
                numPlayers = std::stoi(numPlayerInput.getText());
            } else {
                numPlayers = 0; 
                gameMessage = "Please enter the number of players.";
                return;
            }
            
            if (numPlayers >= 2 && numPlayers <= 10) {
                playerNames.assign(numPlayers, ""); 
                // playerNameInputs already resized in constructor, now configure them
                for (int i = 0; i < numPlayers; i++) {
                    playerNameInputs[i] = std::make_unique<TextBox>(
                        GetScreenWidth() / 2.0f - 150, 
                        120.0f + i * 60, // Adjusted spacing
                        300, 40, 20, ::WHITE, ::BLACK, 15); 
                }
                
                if (numPlayers > 0 && playerNameInputs[0]) {
                    playerNameInputs[0]->setActive(true);
                }
                currentScreen = GameScreen::PLAYER_SETUP;
                // Update startGameButton for the next screen
                startGameButton = Button(GetScreenWidth()/2.0f - 100, 120.0f + numPlayers * 60 + 20, 200, 50, "Start Game", 24);
            } else {
                gameMessage = "Players must be between 2 and 10.";
            }
        } catch (const std::invalid_argument&) {
            gameMessage = "Invalid number format for players.";
        } catch (const std::out_of_range&) {
            gameMessage = "Number of players is too large.";
        }
    }
}

void UnoGUI::drawPlayerSetupScreen() {
    DrawText("Enter Player Names", GetScreenWidth() / 2 - MeasureText("Enter Player Names", 30) / 2, 60, 30, ::DARKGRAY);
    
    for (int i = 0; i < numPlayers; i++) {
        if (playerNameInputs[i]) { // Check if unique_ptr is valid
            DrawText(("Player " + std::to_string(i + 1) + ":").c_str(), 
                    playerNameInputs[i]->bounds.x - MeasureText(("Player " + std::to_string(i + 1) + ":").c_str(), 20) - 10, 
                    playerNameInputs[i]->bounds.y + 10, 20, ::GRAY);
            playerNameInputs[i]->draw();
        }
    }
    
    startGameButton.draw(); 
    if (!gameMessage.empty()) {
         DrawText(gameMessage.c_str(), 20, GetScreenHeight() - 40, 20, ::RED);
    }
}

void UnoGUI::handlePlayerSetupScreenInput() {
     if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        bool anyTextboxClicked = false;
        for (int i = 0; i < numPlayers; ++i) {
            if (playerNameInputs[i] && CheckCollisionPointRec(GetMousePosition(), playerNameInputs[i]->bounds)) {
                for (int j = 0; j < numPlayers; ++j) if(playerNameInputs[j]) playerNameInputs[j]->setActive(false);
                playerNameInputs[i]->setActive(true);
                anyTextboxClicked = true;
                break;
            }
        }
        // If clicked outside any textbox, deactivate all (optional)
        // if (!anyTextboxClicked && !startGameButton.isClicked()) { 
        //     for (int i = 0; i < numPlayers; ++i) if(playerNameInputs[i]) playerNameInputs[i]->setActive(false);
        // }
    }

    if (IsKeyPressed(KEY_TAB)) {
        int activeIndex = -1;
        for (int i = 0; i < numPlayers; i++) {
            if (playerNameInputs[i] && playerNameInputs[i]->isActiveBox()) {
                activeIndex = i;
                playerNameInputs[i]->setActive(false);
                break;
            }
        }
        if (numPlayers > 0) { // Ensure numPlayers is positive
             activeIndex = (activeIndex + 1) % numPlayers;
             if(playerNameInputs[activeIndex]) playerNameInputs[activeIndex]->setActive(true);
        }
    }
    
    if (startGameButton.isClicked()) {
        gameMessage = ""; // Clear previous messages
        playerNames.clear();
        bool allNamesEntered = true; // Assume true, then check
        for (int i = 0; i < numPlayers; ++i) {
            if (playerNameInputs[i]) {
                std::string name = playerNameInputs[i]->getText();
                if (name.empty()) {
                    name = "Player " + std::to_string(i + 1); // Default name
                }
                playerNames.push_back(name);
            } else { 
                allNamesEntered = false; // Should not happen
                gameMessage = "Error: Player input box missing.";
                break;
            }
        }
        
        if (allNamesEntered) {
            initializeGame();
            currentScreen = GameScreen::GAMEPLAY;
        }
    }
}

// Patch to fix UnoGUI::drawGameplayScreen in Game.cpp
// Find this method in Game.cpp and replace it with this version

void UnoGUI::drawGameplayScreen() {
    if (!game || !game->getCurrentPlayer()) {
        DrawText("Error: Game or player not initialized!", 10, 10, 20, ::RED);
        return;
    }
    
    Player* currentPlayer = game->getCurrentPlayer(); 
    Card* topCard = game->getTopCard(); 

    // Draw Top Card
    if (topCard) {
        DrawText("Top Card:", GetScreenWidth() / 2.0f - 50, 20, 20, ::DARKGRAY);
        CardRenderer::drawCard(topCard, GetScreenWidth() / 2.0f - 50, 50, 100, 150);
    }

    // Draw Player Info & Game Message
    std::string turnText = currentPlayer->getName() + "'s Turn"; 
    DrawText(turnText.c_str(), GetScreenWidth()/2.0f - MeasureText(turnText.c_str(), 24)/2.0f, 210, 24, ::BLACK);

    if (!gameMessage.empty()) {
        Color messageColor = ::DARKBLUE;
        if (gameMessage.find("cannot be played") != std::string::npos || gameMessage.find("forgot to call UNO") != std::string::npos) {
            messageColor = ::RED;
        } else if (gameMessage.find("called UNO") != std::string::npos || gameMessage.find("chose") != std::string::npos) {
            messageColor = ::DARKGREEN;
        }
        DrawText(gameMessage.c_str(), GetScreenWidth()/2.0f - MeasureText(gameMessage.c_str(), 20)/2.0f, 240, 20, messageColor);
    }

    if (waitingForNextPlayer) {
        DrawText(("Waiting for " + game->getCurrentPlayer()->getName() + " to continue...").c_str(), 
            GetScreenWidth()/2.0f - MeasureText(("Waiting for " + game->getCurrentPlayer()->getName()+" to continue...").c_str(), 22)/2.0f, 
            GetScreenHeight()/2.0f - 30, 22, ::DARKGRAY);
        continueButton.draw();
    } else if (cardPendingColorChoice) {
        drawColorPicker();
    } else { 
        int handSize = currentPlayer->getHandSize(); 
        float cardWidth = 70;
        float cardHeight = 105;
        float spacing = 10;
        // Dynamic hand width calculation
        float totalHandDisplayWidth = (handSize * (cardWidth + spacing)) - spacing; 
        if (handSize == 0) totalHandDisplayWidth = 0;

        float startX = (GetScreenWidth() - totalHandDisplayWidth) / 2.0f;
        float startY = GetScreenHeight() - cardHeight - 80; // Position hand lower

        if (handSize > 0) {
            for (int i = 0; i < handSize; i++) {
                Card* card = currentPlayer->getCardAtIndex(i); 
                Rectangle cardRect = {startX + i * (cardWidth + spacing), startY, cardWidth, cardHeight};
                bool isSelected = (i == selectedCardIndex);
                // Hover effect for cards (simple highlight or scale)
                bool isHovered = CheckCollisionPointRec(GetMousePosition(), cardRect);
                float currentCardY = startY;
                if(isHovered && !isSelected) currentCardY -=10; // Lift card slightly on hover if not selected
                if(isSelected) currentCardY -=20; // Lift selected card more

                CardRenderer::drawCard(card, cardRect.x, currentCardY, cardRect.width, cardRect.height, isSelected);
                
                if (isHovered && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
                    selectedCardIndex = isSelected ? -1 : i;
                }
            }
        } else if (!currentPlayer->hasWon()) { // Only show if not won yet
            DrawText("No cards in hand!", GetScreenWidth()/2.0f - MeasureText("No cards in hand!", 24)/2.0f, startY + cardHeight/2 -12, 24, ::GRAY);
        }
        
        // IMPORTANT: Fix button positioning and ensure they're shown
        // Position buttons at the bottom of the screen with proper spacing
        float buttonY = GetScreenHeight() - 70.0f;
        unoButton = Button(50, buttonY, 120, 50, "Call UNO!", 20, ::RED, ::RED, ::WHITE);
        throwCardButton = Button(180, buttonY, 150, 50, "Throw Card", 20, ::GREEN, ::DARKGREEN, ::WHITE);
        drawCardButton = Button(340, buttonY, 150, 50, "Draw Card", 20, ::BLUE, ::DARKBLUE, ::WHITE);
        
        // Set button states properly
        unoButton.setEnabled(currentPlayer->getHandSize() == 2 && !currentPlayer->hasCalledUNOStatus());
        throwCardButton.setEnabled(selectedCardIndex != -1);
        drawCardButton.setEnabled(true);
        
        // Draw buttons unconditionally
        unoButton.draw();
        throwCardButton.draw();
        drawCardButton.draw();
        
        // For debugging - draw a text showing if buttons should be visible
        DrawText("Buttons", 20, buttonY - 20, 20, ::DARKGRAY);
    }
}
void UnoGUI::drawColorPicker() {
    DrawText("Choose a Color for the Wild Card:", GetScreenWidth() / 2.0f - MeasureText("Choose a Color for the Wild Card:", 24) / 2.0f, GetScreenHeight() / 2.0f - 80, 24, ::DARKGRAY);
    float buttonWidth = 100;
    float buttonHeight = 50;
    float spacing = 20;
    float totalWidth = 4 * buttonWidth + 3 * spacing;
    float startX = (GetScreenWidth() - totalWidth) / 2.0f;
    float startY = GetScreenHeight() / 2.0f - 25; // Centered

    // Update button positions for drawing
    redColorButton = Button(startX, startY, buttonWidth, buttonHeight, "RED", 20, ::RED);
    greenColorButton = Button(startX + (buttonWidth + spacing), startY, buttonWidth, buttonHeight, "GREEN", 20, ::GREEN);
    blueColorButton = Button(startX + 2 * (buttonWidth + spacing), startY, buttonWidth, buttonHeight, "BLUE", 20, ::BLUE);
    yellowColorButton = Button(startX + 3 * (buttonWidth + spacing), startY, buttonWidth, buttonHeight, "YELLOW", 20, ::YELLOW);

    redColorButton.draw();
    greenColorButton.draw();
    blueColorButton.draw();
    yellowColorButton.draw();
}


void UnoGUI::handleColorPickerInput() {
    Player* currentPlayer = game->getCurrentPlayer(); 
    if (!currentPlayer || !cardPendingColorChoice) return;

    enum deckColor chosenGameColor = deckColor::none; // Game's enum
    bool colorWasChosen = false;

    if (redColorButton.isClicked()) { chosenGameColor = deckColor::red; colorWasChosen = true; }
    else if (greenColorButton.isClicked()) { chosenGameColor = deckColor::green; colorWasChosen = true; }
    else if (blueColorButton.isClicked()) { chosenGameColor = deckColor::blue; colorWasChosen = true; }
    else if (yellowColorButton.isClicked()) { chosenGameColor = deckColor::yellow; colorWasChosen = true; }

    if (colorWasChosen) {
        cardPendingColorChoice->setColor(chosenGameColor); 
        
        gameMessage = currentPlayer->getName() + " chose " + colorToString(chosenGameColor) + " for " + cardPendingColorChoice->getName();
        // Note: Backend will still prompt via std::cin. This is a known limitation.
        // To fix, backend `WildCard::play` and `DrawFourCard::play` need to check
        // if `this->getColor() != Color::NONE` and skip cin if color is already set.
        std::cout << "[GUI NOTE] Backend will now likely prompt for color via console due to its current Card::play implementation for Wilds." << std::endl;


        currentPlayer->playCard(indexOfCardPendingColorChoice, *game); 
        
        cardPendingColorChoice = nullptr;
        indexOfCardPendingColorChoice = -1;
        selectedCardIndex = -1; 

        game->enforceUNOCall(currentPlayer); 
        if (currentPlayer->hasWon()) { 
            gameOverState = true;
            winnerName = currentPlayer->getName(); 
            currentScreen = GameScreen::GAME_OVER;
            return;
        }
        // Check for game over again, as some cards might end the game (though less common for Wilds alone)
        if (game->isGameOver()){
             gameOverState = true; 
             // Determine winner if necessary (simplified: assume current player if game ends now by their action)
             if(currentPlayer->hasWon()) winnerName = currentPlayer->getName();
             else { // Try to find another winner if game ended for other reasons (e.g. last player)
                for(int i = 0; i < game->getPlayerCount(); ++i) {
                    Player* p = game->getCurrentPlayer(); // Use current and cycle
                    if(p->hasWon()){ winnerName = p->getName(); break; }
                    if(i < game->getPlayerCount() - 1 && game->getPlayerCount() > 1) game->nextTurn(); // Prevent infinite loop on 1 player
                }
             }
             currentScreen = GameScreen::GAME_OVER;
             return;
        }

        waitingForNextPlayer = true;
        game->nextTurn(); 
    }
}


void UnoGUI::handleGameplayScreenInput() {
    if (!game || gameOverState || cardPendingColorChoice || waitingForNextPlayer) return; 

    Player* currentPlayer = game->getCurrentPlayer(); 
    if (!currentPlayer) return;
    
    // UNO Button: Only enable if hand size is 2 (before playing a card to go to 1)
    // and they haven't already called UNO this turn (backend handles this implicitly by resetting status)
    if (unoButton.isClicked() && currentPlayer->getHandSize() == 2 && !currentPlayer->hasCalledUNOStatus()) { 
        currentPlayer->callUNO(); 
        gameMessage = currentPlayer->getName() + " called UNO!"; 
    }
    
    // Throw Card Button
    if (throwCardButton.isClicked() && selectedCardIndex >= 0 && selectedCardIndex < currentPlayer->getHandSize()) { 
        Card* selectedCard = currentPlayer->getCardAtIndex(selectedCardIndex); 
        
        if (game->isCardPlayable(selectedCard)) { 
            if (dynamic_cast<WildCard*>(selectedCard) || dynamic_cast<DrawFourCard*>(selectedCard)) {
                cardPendingColorChoice = selectedCard;
                indexOfCardPendingColorChoice = selectedCardIndex;
                // Clear previous game message before setting new one
                gameMessage = "Choose a color for " + selectedCard->getName() + "!";
            } else {
                std::string cardNameStr = selectedCard->getName(); 
                // Prepend color for non-wild cards if not already in name (most action cards include color in backend name)
                // if (selectedCard->getColor() != Color::NONE && cardNameStr.find(colorToString(selectedCard->getColor())) == std::string::npos) {
                //      cardNameStr = colorToString(selectedCard->getColor()) + " " + cardNameStr; 
                // }
                
                currentPlayer->playCard(selectedCardIndex, *game); 
                gameMessage = currentPlayer->getName() + " played " + cardNameStr; 
                
                 if (dynamic_cast<DropTwoCard*>(selectedCard)) {
                    gameMessage += ". Follow console to drop cards.";
                     std::cout << "[GUI NOTE] DropTwoCard played. Backend will now prompt for cards to drop via console." << std::endl;
                }

                game->enforceUNOCall(currentPlayer); 
                
                if (currentPlayer->hasWon()) { 
                    gameOverState = true;
                    winnerName = currentPlayer->getName(); 
                    currentScreen = GameScreen::GAME_OVER;
                    return; 
                }
                
                waitingForNextPlayer = true;
                game->nextTurn(); 
            }
            selectedCardIndex = -1; 
        } else {
            gameMessage = "Card cannot be played on current top card.";
        }
    }
    
    // Draw Card Button
    if (drawCardButton.isClicked()) {
        game->drawCard(currentPlayer); 
        gameMessage = currentPlayer->getName() + " drew a card."; 
        
        waitingForNextPlayer = true;
        game->nextTurn(); 
        selectedCardIndex = -1; 
    }
    
    if (!gameOverState && game->isGameOver()) { 
        gameOverState = true;
        // Simplified winner finding, assumes player who made game over by emptying hand is the winner
        bool foundWinner = false;
        for(int i=0; i < game->getPlayerCount(); ++i) {
             Player* p_check = game->getCurrentPlayer(); // This cycles players.
             if (p_check->hasWon()){ winnerName = p_check->getName(); foundWinner = true; break;}
             if (game->getPlayerCount() > 1 && i < game->getPlayerCount() -1 ) game->nextTurn();
        }
        if(!foundWinner && game->getPlayerCount() == 1) winnerName = game->getCurrentPlayer()->getName(); // Last one

        currentScreen = GameScreen::GAME_OVER;
    }
}

void UnoGUI::drawGameOverScreen() {
    DrawText("Game Over!", GetScreenWidth()/2.0f - MeasureText("Game Over!", 40)/2.0f, 150, 40, ::BLACK);
    
    if (!winnerName.empty()) {
        std::string winnerText = winnerName + " wins the game!";
        DrawText(winnerText.c_str(), GetScreenWidth()/2.0f - MeasureText(winnerText.c_str(), 30)/2.0f, 250, 30, ::DARKGREEN);
    } else {
        DrawText("The game has ended.", GetScreenWidth()/2.0f - MeasureText("The game has ended.", 30)/2.0f, 250, 30, ::GRAY);
    }
    
    DrawText("Press ENTER to Play Again or ESC to Exit", GetScreenWidth()/2.0f - MeasureText("Press ENTER to Play Again or ESC to Exit", 20)/2.0f, 350, 20, ::DARKGRAY);
}

void UnoGUI::handleGameOverScreenInput() {
    if (IsKeyPressed(KEY_ENTER)) {
        currentScreen = GameScreen::TITLE;
        numPlayerInput.setText("");
        numPlayerInput.setActive(false);
        gameMessage = "";
        gameOverState = false;
        winnerName = "";
        // Reset button text and position for title screen
        numPlayerInput = TextBox(GetScreenWidth() / 2.0f - 125, 280, 250, 40, 20, ::WHITE, ::BLACK, 2);
        startGameButton = Button(GetScreenWidth()/2.0f - 100, 340, 200, 50, "Start Setup", 24);
        // Game itself will be re-initialized by initializeGame if they start a new one from title.
    }
}

void UnoGUI::initializeGame() {
    game = std::make_unique<UnoGame>(); 
    
    for (const auto& name : playerNames) {
        game->addPlayer(new Player(name)); 
    }
    
    game->startGame(); 
    
    waitingForNextPlayer = false;
    selectedCardIndex = -1;
    cardPendingColorChoice = nullptr;
    indexOfCardPendingColorChoice = -1;
    if(game->getCurrentPlayer()) { // Check if current player exists after starting game
      gameMessage = game->getCurrentPlayer()->getName() + " begins!"; 
    } else {
      gameMessage = "Game ready to start!"; // Fallback
    }
    gameOverState = false;
    winnerName = "";
}