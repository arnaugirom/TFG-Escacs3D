#pragma once
#include <vector>

class Board;

enum Color { WHITE, BLACK };

class Piece {
protected:
    int x, y;
    Color color;
    bool moved = false;

public:
    Piece(int x, int y, Color c);
    virtual ~Piece() {}

    int getX() const;
    int getY() const;
    Color getColor() const;

    void setPosition(int newX, int newY);

    virtual std::vector<std::pair<int, int>> getMoves(Board* board) = 0;
    virtual char getSymbol() const = 0;

    bool hasMoved() const { return moved; }
    void setMoved(bool v) { moved = v; }
};