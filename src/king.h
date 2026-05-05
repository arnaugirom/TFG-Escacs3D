#pragma once
#include "piece.h"

class Board;

class King : public Piece {
public:
    King(int x, int y, Color c);

    std::vector<std::pair<int, int>> getMoves(Board* board) override;
    char getSymbol() const override;
};