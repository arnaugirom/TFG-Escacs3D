//BOARD.CPP

#include "board.h"



Board::Board() {
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++) {
            grid[i][j].piece = nullptr;
            grid[i][j].obj = nullptr;
            grid[i][j].posicions = glm::vec3(-3.5f + i, -3.5f + j, -0.2f);
        }
}

void Board::Inicialitza_Taulell(modelManager& mm)
{
    GameObject* TAULELL = createObject(mm.getTaulell());
    TAULELL->rotate(glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1, 0, 0)));

    for (int i = 0; i < 8; i++) {
        Pawn* p = new Pawn(i, 1, WHITE);

        GameObject* obj = createObject(mm.getBPeo());
        glm::vec3 pos = glm::vec3(-3.5f + i, -2.5f, -0.2f);
        placePiece(obj, pos);

        grid[i][1].piece = p;
        grid[i][1].obj = obj;
    }
}





Piece* Board::get(int x, int y) {
    if (x < 0 || x >= 8 || y < 0 || y >= 8) return nullptr;
    return grid[x][y].piece;
}

/*/
GameObject* Board::getObj(int x, int y) {
    if (x < 0 || x >= 8 || y < 0 || y >= 8) return nullptr;
    return grid[x][y].obj;
}
*/

void Board::set(int x, int y, Piece* p) {
    grid[x][y].piece = p;
}

bool Board::isEmpty(int x, int y) {
    return get(x, y) == nullptr;
}


void Board::movePiece(Piece* p, int newX, int newY) {
    Cell& oldCell = grid[p->getX()][p->getY()];
    Cell& newCell = grid[newX][newY];

    newCell = oldCell;
    oldCell.piece = nullptr;
    oldCell.obj = nullptr;

    p->setPosition(newX, newY);
}