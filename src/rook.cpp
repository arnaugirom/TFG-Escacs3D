#include "rook.h"
#include "board.h"

Rook::Rook(int x, int y, Color c) : Piece(x, y, c) {}

std::vector<std::pair<int, int>> Rook::getMoves(Board* board)
{
    std::vector<std::pair<int, int>> moves;

    int dirs[4][2] = { {1,0},{-1,0},{0,1},{0,-1} };

    for (auto& d : dirs)
    {
        int nx = x + d[0];
        int ny = y + d[1];

        while (board->inside(nx, ny))
        {
            if (board->isEmpty(nx, ny))
            {
                moves.push_back({ nx, ny });
            }
            else
            {
                if (board->hasEnemy(nx, ny, color))
                    moves.push_back({ nx, ny });
                break;
            }

            nx += d[0];
            ny += d[1];
        }
    }

    return moves;
}

char Rook::getSymbol() const {
    return 'R';
}