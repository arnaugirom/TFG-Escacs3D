#include "king.h"
#include "board.h"

King::King(int x, int y, Color c) : Piece(x, y, c) {}

std::vector<std::pair<int, int>> King::getMoves(Board* board)
{
    std::vector<std::pair<int, int>> moves;

    for (int dx = -1; dx <= 1; dx++)
        for (int dy = -1; dy <= 1; dy++)
        {
            if (dx == 0 && dy == 0) continue;

            int nx = x + dx;
            int ny = y + dy;

            if (!board->inside(nx, ny)) continue;

            if (board->isEmpty(nx, ny) || board->hasEnemy(nx, ny, color))
                moves.push_back({ nx, ny });
        }

    return moves;
}

char King::getSymbol() const {
    return 'K';
}
