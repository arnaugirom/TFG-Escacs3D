#pragma once
#include "piece.h"

class Board;

class Bishop : public Piece {
public:
    Bishop(int x, int y, Color c);

    std::vector<std::pair<int, int>> getMoves(Board* board) override;
    char getSymbol() const override;
};