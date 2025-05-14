// ColorUtils.h
#ifndef COLOR_UTILS_H
#define COLOR_UTILS_H

#include "raylib.h"
#include "CardColour.h"

inline Color getRaylibColor(CardColor color) {
    switch (color) {
        case CardColor::Red: return RED;
        case CardColor::Blue: return BLUE;
        case CardColor::Green: return GREEN;
        case CardColor::Yellow: return YELLOW;
        default: return GRAY;
    }
}

#endif // COLOR_UTILS_H
