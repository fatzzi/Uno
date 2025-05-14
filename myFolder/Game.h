#ifndef UNO_GUI_H
#define UNO_GUI_H

// It's generally better to include raylib.h before your custom headers
// if your headers might also use types that could conflict,
// but in this case, Game.h includes Combined_uno.h which defines 'enum class Color'.
// We will rely on explicit scoping.
#include "Combined_uno.h" // Defines enum class Color
#include <raylib.h>       // Defines struct Color (as Color)
#include <string>
#include <vector>
#include <memory>

enum class GameScreen {
    TITLE,
    PLAYER_SETUP,
    GAMEPLAY,
    GAME_OVER
};

class Button {
private:
    Rectangle bounds;
    std::string text;
    int fontSize;
    Color color; // Use Raylib's Color struct
    Color hoverColor; // Use Raylib's Color struct
    Color textColor; // Use Raylib's Color struct
    bool enabled;

public:
    Button(float x = 0, float y = 0, float width = 0, float height = 0, const std::string& buttonText = "", 
           int textSize = 20, Color buttonColor = LIGHTGRAY, Color buttonHoverColor = GRAY, 
           Color buttonTextColor = BLACK); // Explicitly Color for Raylib colors
    
    bool isClicked() const;
    bool isHovered() const;
    void draw() const;
    void setEnabled(bool isEnabled);
    bool isEnabled() const;
    void setText(const std::string& newText);
    Rectangle getBounds() const { return bounds; } 
};

class TextBox {
public: // Made bounds public
    Rectangle bounds; 
private:
    std::string text;
    int fontSize;
    Color bgColor; // Use Raylib's Color struct
    Color textColor; // Use Raylib's Color struct
    bool isActive;
    size_t maxLength;

public:
    TextBox(float x = 0, float y = 0, float width = 0, float height = 0, int textSize = 20, 
            Color boxBgColor = WHITE, Color boxTextColor = BLACK, size_t maxTextLength = 20); // Explicitly Color
    
    void update();
    void draw() const;
    std::string getText() const;
    void setText(const std::string& newText);
    void setActive(bool active);
    bool isActiveBox() const;
};

class CardRenderer {
public:
    // Takes game's enum Color, returns Raylib's struct Color
    static Color getRaylibColorForGameEnum(enum deckColor gameCardColor); 
    static void drawCard(const Card* card, float x, float y, float width, float height, bool selected = false);
};

class UnoGUI {
private:
    GameScreen currentScreen;
    std::unique_ptr<UnoGame> game;
    std::vector<std::unique_ptr<TextBox>> playerNameInputs;
    std::vector<std::string> playerNames;
    int numPlayers;

    TextBox numPlayerInput;
    Button startGameButton; 

    Button unoButton;
    Button throwCardButton;
    Button drawCardButton;
    Button continueButton;

    Button redColorButton;
    Button greenColorButton;
    Button blueColorButton;
    Button yellowColorButton;
    Card* cardPendingColorChoice; 
    int indexOfCardPendingColorChoice; 

    bool waitingForNextPlayer;
    std::string gameMessage;
    int selectedCardIndex; 
    bool gameOverState;
    std::string winnerName;
    
    void drawTitleScreen();
    void drawPlayerSetupScreen();
    void drawGameplayScreen();
    void drawGameOverScreen();
    void drawColorPicker(); 

    void handleTitleScreenInput();
    void handlePlayerSetupScreenInput();
    void handleGameplayScreenInput();
    void handleGameOverScreenInput();
    void handleColorPickerInput(); 
    
    void initializeGame();
    
public:
    UnoGUI();
    ~UnoGUI();
    
    void run();
};

#endif // UNO_GUI_H