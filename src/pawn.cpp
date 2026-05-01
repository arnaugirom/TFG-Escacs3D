#include "pawn.h"

Pawn::Pawn(int x, int y, Color color) : Piece(x, y, color) {}

std::vector<std::pair<int, int>> Pawn::getMoves() {
    std::vector<std::pair<int, int>> moves;

    int dir = (color == WHITE) ? 1 : -1;

    moves.push_back({ x, y + dir });

    return moves;
}

char Pawn::getSymbol() const {
    return 'P';
}