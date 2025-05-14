#include "../header/ColorSelector.h"

ColorSelector::ColorSelector() 
    : isActive(false), selectedColor(CardColor::NONE) {
    // Initialize color buttons
    int buttonSize = 60;
    int spacing = 20;
    int startX = GetScreenWidth()/2 - (2*buttonSize + spacing)/2;
    int startY = GetScreenHeight()/2 - (2*buttonSize + spacing)/2;
    
    colorButtons[0] = {(float)startX, (float)startY, (float)buttonSize, (float)buttonSize, RED};
    colorButtons[1] = {(float)(startX + buttonSize + spacing), (float)startY, (float)buttonSize, (float)buttonSize, YELLOW};
    colorButtons[2] = {(float)startX, (float)(startY + buttonSize + spacing), (float)buttonSize, (float)buttonSize, GREEN};
    colorButtons[3] = {(float)(startX + buttonSize + spacing), (float)(startY + buttonSize + spacing), (float)buttonSize, (float)buttonSize, BLUE};
}

void ColorSelector::Show() {
    isActive = true;
    selectedColor = CardColor::NONE;
    
    // Update positions based on current screen size
    int buttonSize = 60;
    int spacing = 20;
    int startX = GetScreenWidth()/2 - (2*buttonSize + spacing)/2;
    int startY = GetScreenHeight()/2 - (2*buttonSize + spacing)/2;
    
    colorButtons[0] = {(float)startX, (float)startY, (float)buttonSize, (float)buttonSize, RED};
    colorButtons[1] = {(float)(startX + buttonSize + spacing), (float)startY, (float)buttonSize, (float)buttonSize, YELLOW};
    colorButtons[2] = {(float)startX, (float)(startY + buttonSize + spacing), (float)buttonSize, (float)buttonSize, GREEN};
    colorButtons[3] = {(float)(startX + buttonSize + spacing), (float)(startY + buttonSize + spacing), (float)buttonSize, (float)buttonSize, BLUE};
}

void ColorSelector::Hide() {
    isActive = false;
}

CardColor ColorSelector::GetSelectedColor() const {
    return selectedColor;
}

bool ColorSelector::IsActive() const {
    return isActive;
}

bool ColorSelector::Update() {
    if (!isActive) return false;
    
    Vector2 mousePos = GetMousePosition();
    bool clicked = false;
    
    // Draw dark overlay
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), (Color){0, 0, 0, 200});
    
    // Draw message
    DrawText("Select a color", GetScreenWidth()/2 - MeasureText("Select a color", 30)/2, GetScreenHeight()/2 - 100, 30, WHITE);
    
    // Draw and process color buttons
    for (int i = 0; i < 4; i++) {
        bool hover = CheckCollisionPointRec(mousePos, colorButtons[i].rect);
        
        // Create a larger highlight rectangle
        Rectangle highlightRect = {
            colorButtons[i].rect.x - 5,
            colorButtons[i].rect.y - 5,
            colorButtons[i].rect.width + 10,
            colorButtons[i].rect.height + 10
        };
        
        // Draw highlight if hovered
        if (hover) {
            DrawRectangleRec(highlightRect, WHITE);
        }
        
        // Draw color button
        DrawRectangleRec(colorButtons[i].rect, colorButtons[i].color);
        
        // Handle click
        if (hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            switch (i) {
                case 0: selectedColor = CardColor::Red; break;
                case 1: selectedColor = CardColor::Yellow; break;
                case 2: selectedColor = CardColor::Green; break;
                case 3: selectedColor = CardColor::Blue; break;
            }
            isActive = false;
            clicked = true;
        }
    }
    
    return clicked;
}