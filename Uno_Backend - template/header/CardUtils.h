#ifndef CARD_UTILS_H
#define CARD_UTILS_H

template <typename T1, typename T2>
bool areCardsPlayable(const T1* card1, const T2* card2) {
    if (!card1 || !card2) return false;
    return *card1 == *card2;
}

#endif
