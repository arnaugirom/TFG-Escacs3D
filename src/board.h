//BOARD.H

#pragma once
#include "piece.h"
#include "modelManager.h"
#include "gameObject.h"
#include "pawn.h"
#include "rook.h"
#include "knight.h"
#include "bishop.h"
#include "queen.h"
#include "king.h"


struct Cell {
    Piece* piece = nullptr;
    GameObject* obj = nullptr;
    glm::vec3 posicions = glm::vec3(-3.5f, -3.5f, -0.2f);
    bool highlight = false;
};


class Board {
private:
    // El grid hauria de ser de tipus vec3 per guardar els punts centrals de les caselles i aixo sinicialitza en el constructor
    Cell grid[8][8];

public:
    Board();
    void Inicialitza_Taulell(modelManager& mm);

    Cell getCell(int x, int y)
    {
        return grid[x][y];
    }

    // Necessito un getPos
    Piece* get(int x, int y);
    void set(int x, int y, Piece* p);

    //GameObject* getObj(int x, int y);

    bool isEmpty(int x, int y);
    bool hasEnemy(int x, int y, Color myColor);
    bool inside(int x, int y);

    void movePiece(Piece* p, int newX, int newY);
};
#pragma once
