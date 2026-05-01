//PIECE.CPP
#include "piece.h"

Piece::Piece(int x, int y, Color color) : x(x), y(y), color(color) {}

int Piece::getX() const { return x; }
int Piece::getY() const { return y; }
Color Piece::getColor() const { return color; }

void Piece::setPosition(int newX, int newY) {
    x = newX;
    y = newY;
}