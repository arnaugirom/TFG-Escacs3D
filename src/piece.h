#pragma once
#include <vector>

class Board;

enum Color { WHITE, BLACK };

class Piece {
protected:
    int x, y;
    Color color;

public:
    Piece(int x, int y, Color c);
    virtual ~Piece() {}

    int getX() const;
    int getY() const;
    Color getColor() const;

    void setPosition(int newX, int newY);

    virtual std::vector<std::pair<int, int>> getMoves(Board* board) = 0;
    virtual char getSymbol() const = 0;
};