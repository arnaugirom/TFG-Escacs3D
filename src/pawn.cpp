//PAWN.cpp
#include "pawn.h"
#include "board.h"

Pawn::Pawn(int x, int y, Color color) : Piece(x, y, color) {}

std::vector<std::pair<int, int>> Pawn::getMoves(Board* board)
{
    std::vector<std::pair<int, int>> moves;

    int dir = (color == WHITE) ? 1 : -1;

    // 1. moviment endavant (1)
    if (board->inside(x, y + dir) && board->isEmpty(x, y + dir))
    {
        moves.push_back({ x, y + dir });

        // 2. moviment inicial (2 caselles)
        if ((color == WHITE && y == 1) || (color == BLACK && y == 6))
        {
            if (board->isEmpty(x, y + 2 * dir))
            {
                moves.push_back({ x, y + 2 * dir });
            }
        }
    }

    // 3. captures diagonals
    for (int dx : { -1, 1 })
    {
        int nx = x + dx;
        int ny = y + dir;

        if (board->inside(nx, ny) && board->hasEnemy(nx, ny, color))
        {
            moves.push_back({ nx, ny });
        }
    }

    return moves;
}

char Pawn::getSymbol() const {
    return 'P';
}