#pragma once
#include "piece.h"

class Board;

class Queen : public Piece {
public:
    Queen(int x, int y, Color c);

    std::vector<std::pair<int, int>> getMoves(Board* board) override;
    char getSymbol() const override;
};