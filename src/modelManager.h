#pragma once
#include "material.h"
#include "objLoader.h"
#include "player.h"
#include <vector>

class modelManager
{
public:
	modelManager();
	~modelManager();
	void initialSetup();
	COBJModel* loadModel(const std::string& filename);
	std::vector<COBJModel*> getModelRange(const std::vector<COBJModel*>& src, int start, int end);
	COBJModel* getMapa() { return m_modelMapa; }
	std::vector<COBJModel*> getEnemy(int type);
	std::vector<COBJModel*> getTurret(int type);
	COBJModel* getFloor() { return m_turretFloor; }
	COBJModel* getRadius() { return m_turretRadius; }


	COBJModel* getTaulell() { return M_Taulell; }


	COBJModel* getBPeo() { return B_Peo; }
	COBJModel* getBTorre() { return B_Torre; }
	COBJModel* getBCavall() { return B_Cavall; }
	COBJModel* getBAlfil() { return B_Alfil; }
	COBJModel* getBReina() { return B_Reina; }
	COBJModel* getBRei() { return B_Rei; }


	COBJModel* getNPeo() { return N_Peo; }
	COBJModel* getNTorre() { return N_Torre; }
	COBJModel* getNCavall() { return N_Cavall; }
	COBJModel* getNAlfil() { return N_Alfil; }
	COBJModel* getNReina() { return N_Reina; }
	COBJModel* getNRei() { return N_Rei; }



private:
	COBJModel* M_Taulell;


	COBJModel* B_Peo;
	COBJModel* B_Torre;
	COBJModel* B_Cavall;
	COBJModel* B_Alfil;
	COBJModel* B_Reina;
	COBJModel* B_Rei;


	COBJModel* N_Peo;
	COBJModel* N_Torre;
	COBJModel* N_Cavall;
	COBJModel* N_Alfil;
	COBJModel* N_Reina;
	COBJModel* N_Rei;


	COBJModel* m_modelMapa;
	COBJModel* m_turretFloor;
	COBJModel* m_turretRadius;
	std::vector<COBJModel*> m_modelsEnemics;
	std::vector<COBJModel*> m_modelsTorres;
};

