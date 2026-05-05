#pragma once
#include "piece.h"

class Board;

class Rook : public Piece {
public:
    Rook(int x, int y, Color c);

    std::vector<std::pair<int, int>> getMoves(Board* board) override;
    char getSymbol() const override;
};