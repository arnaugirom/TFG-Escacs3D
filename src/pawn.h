#pragma once
#include "piece.h"

class Pawn : public Piece {
public:
    Pawn(int x, int y, Color c);

    std::vector<std::pair<int, int>> getMoves() override;
    char getSymbol() const override;
};