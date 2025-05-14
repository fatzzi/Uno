#ifndef COLOR_SELECTOR_H
#define COLOR_SELECTOR_H

#include "raylib.h"
#include "CardColour.h"

struct ColorButton {
    Rectangle rect;
    Color color;
};

class ColorSelector {
private:
    bool isActive;
    CardColor selectedColor;
    ColorButton colorButtons[4]; // Red, Yellow, Green, Blue
    
public:
    ColorSelector();
    
    void Show();
    void Hide();
    CardColor GetSelectedColor() const;
    bool IsActive() const;
    bool Update(); // Returns true if a color was selected
};

#endif // COLOR_SELECTOR_H