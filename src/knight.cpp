#include "knight.h"
#include "board.h"

Knight::Knight(int x, int y, Color c) : Piece(x, y, c) {}

std::vector<std::pair<int, int>> Knight::getMoves(Board* board)
{
    std::vector<std::pair<int, int>> moves;

    int offsets[8][2] = {
        {1,2},{2,1},{-1,2},{-2,1},
        {1,-2},{2,-1},{-1,-2},{-2,-1}
    };

    for (auto& o : offsets)
    {
        int nx = x + o[0];
        int ny = y + o[1];

        if (!board->inside(nx, ny)) continue;

        if (board->isEmpty(nx, ny) || board->hasEnemy(nx, ny, color))
            moves.push_back({ nx, ny });
    }

    return moves;
}

char Knight::getSymbol() const {
    return 'N';
}