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

    if (!hasMoved())
    {
        // ENROC CURT
        Piece* rook = board->get(7, y);

        if (rook &&
            rook->getSymbol() == 'R' &&
            !rook->hasMoved() &&
            board->isEmpty(5, y) &&
            board->isEmpty(6, y))
        {
            moves.push_back({ 6, y });
        }

        // ENROC LLARG
        rook = board->get(0, y);

        if (rook &&
            rook->getSymbol() == 'R' &&
            !rook->hasMoved() &&
            board->isEmpty(1, y) &&
            board->isEmpty(2, y) &&
            board->isEmpty(3, y))
        {
            moves.push_back({ 2, y });
        }
    }

    return moves;
}

char King::getSymbol() const {
    return 'K';
}
