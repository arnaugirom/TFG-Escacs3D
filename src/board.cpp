//BOARD.CPP

#include "board.h"
#include "gameState.h"


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

    // =========================
    // ♙ PEONES
    // =========================
    for (int i = 0; i < 8; i++) {
        Pawn* p = new Pawn(i, 1, WHITE);

        GameObject* obj = createObject(mm.getBPeo());
        glm::vec3 pos = glm::vec3(-3.5f + i, -2.5f, -0.2f);
        placePiece(obj, pos);

        grid[i][1].piece = p;
        grid[i][1].obj = obj;
    }

    // =========================
    // ♜ TORRES
    // =========================
    grid[0][0].piece = new Rook(0, 0, WHITE);
    grid[7][0].piece = new Rook(7, 0, WHITE);

    grid[0][0].obj = createObject(mm.getBTorre());
    placePiece(grid[0][0].obj, glm::vec3(-3.5f, -3.5f, -0.2f));

    grid[7][0].obj = createObject(mm.getBTorre());
    placePiece(grid[7][0].obj, glm::vec3(3.5f, -3.5f, -0.2f));

    // =========================
    // ♞ CABALLOS
    // =========================
    grid[1][0].piece = new Knight(1, 0, WHITE);
    grid[6][0].piece = new Knight(6, 0, WHITE);

    grid[1][0].obj = createObject(mm.getBCavall());
    placePiece(grid[1][0].obj, glm::vec3(-2.5f, -3.5f, -0.2f), true);

    grid[6][0].obj = createObject(mm.getBCavall());
    placePiece(grid[6][0].obj, glm::vec3(2.5f, -3.5f, -0.2f), true);

    // =========================
    // ♝ ALFILES
    // =========================
    grid[2][0].piece = new Bishop(2, 0, WHITE);
    grid[5][0].piece = new Bishop(5, 0, WHITE);

    grid[2][0].obj = createObject(mm.getBAlfil());
    placePiece(grid[2][0].obj, glm::vec3(-1.5f, -3.5f, -0.2f));

    grid[5][0].obj = createObject(mm.getBAlfil());
    placePiece(grid[5][0].obj, glm::vec3(1.5f, -3.5f, -0.2f));

    // =========================
    // ♛ REINA
    // =========================
    grid[3][0].piece = new Queen(3, 0, WHITE);
    grid[3][0].obj = createObject(mm.getBReina());
    placePiece(grid[3][0].obj, glm::vec3(-0.5f, -3.5f, -0.2f));

    // =========================
    // ♚ REY
    // =========================
    grid[4][0].piece = new King(4, 0, WHITE);
    grid[4][0].obj = createObject(mm.getBRei());
    placePiece(grid[4][0].obj, glm::vec3(0.5f, -3.5f, -0.2f));
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

    if (p->getSymbol() == 'K')
    {
        int oldX = p->getX();

        // ENROC CURT
        if (newX == 6 && oldX == 4)
        {
            Cell& rookOld = grid[7][newY];
            Cell& rookNew = grid[5][newY];

            rookNew.piece = rookOld.piece;
            rookNew.obj = rookOld.obj;

            rookOld.piece = nullptr;
            rookOld.obj = nullptr;

            rookNew.piece->setPosition(5, newY);

            glm::vec3 pos = rookNew.posicions;
            rookNew.obj->translate(pos);
        }

        // ENROC LLARG
        else if (newX == 2 && oldX == 4)
        {
            Cell& rookOld = grid[0][newY];
            Cell& rookNew = grid[3][newY];

            rookNew.piece = rookOld.piece;
            rookNew.obj = rookOld.obj;

            rookOld.piece = nullptr;
            rookOld.obj = nullptr;

            rookNew.piece->setPosition(3, newY);

            glm::vec3 pos = rookNew.posicions;
            rookNew.obj->translate(pos);
        }
    }

    newCell.piece = p;
    newCell.obj = oldCell.obj;

    oldCell.piece = nullptr;
    oldCell.obj = nullptr;

    p->setPosition(newX, newY);
    p->setMoved(true);

    // RESET highlights anteriores (IMPORTANTE)
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            grid[i][j].highlight = false;

    // PROMOCIÓN
    if (p->getSymbol() == 'P') {
        if ((p->getColor() == WHITE && newY == 7) ||
            (p->getColor() == BLACK && newY == 0))
        {
            std::cout << "PEO LISTO PARA PROMOCIÓN!" << std::endl;

            newCell.highlight = true;

            promotionPiece = p;
            promotionCell = &newCell;
            waitingPromotion = true;
        }
    }
}


bool Board::inside(int x, int y) {
    return x >= 0 && x < 8 && y >= 0 && y < 8;
}

bool Board::hasEnemy(int x, int y, Color myColor) {
    Piece* p = get(x, y);
    return p != nullptr && p->getColor() != myColor;
}