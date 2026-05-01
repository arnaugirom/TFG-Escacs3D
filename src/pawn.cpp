//PAWN.cpp
#include "pawn.h"

Pawn::Pawn(int x, int y, Color color) : Piece(x, y, color) {}

std::vector<std::pair<int, int>> Pawn::getMoves() 
{
    std::vector<std::pair<int, int>> moves;

    int x = this->x;
    int y = this->y;

    // moviment normal (1 casella endavant)
    moves.push_back({ x, y + 1 });

    return moves;
}

char Pawn::getSymbol() const {
    return 'P';
}