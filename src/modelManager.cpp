#include "modelManager.h"
#include "Enemy.h"
#include <unordered_set>



modelManager::modelManager()
{
	m_modelMapa = nullptr;
	m_turretFloor = nullptr;
	m_turretRadius = nullptr;


	M_Taulell = nullptr;


	B_Peo = nullptr;
	B_Torre = nullptr;
	B_Cavall = nullptr;
	B_Alfil = nullptr;
	B_Reina = nullptr;
	B_Rei = nullptr;


	N_Peo = nullptr;
	N_Torre = nullptr;
	N_Cavall = nullptr;
	N_Alfil = nullptr;
	N_Reina = nullptr;
	N_Rei = nullptr;


}

modelManager::~modelManager()
{
	delete M_Taulell;


	delete B_Peo;
	delete B_Torre;
	delete B_Cavall;
	delete B_Alfil;
	delete B_Reina;
	delete B_Rei;


	delete N_Peo;
	delete N_Torre;
	delete N_Cavall;
	delete N_Alfil;
	delete N_Reina;
	delete N_Rei;



	delete m_modelMapa;
	delete m_turretFloor;
	delete m_turretRadius;
	std::unordered_set<COBJModel*> deleted;

	for (COBJModel* m : m_modelsEnemics)
		if (m && !deleted.count(m)) {
			deleted.insert(m);
			delete m;
		}
	for (COBJModel* m : m_modelsTorres)
		delete m;
}

void modelManager::initialSetup()
{
	//SetUp Taulell
	M_Taulell = loadModel("Escacs\\Taulell.obj");
	
	
	//BLANQUES
	B_Peo = loadModel("Escacs\\PeoBlanc.obj");
	B_Torre = loadModel("Escacs\\TorreBlanc.obj");
	B_Cavall = loadModel("Escacs\\CavallBlanc.obj");
	B_Alfil = loadModel("Escacs\\AlfilBlanc.obj");
	B_Reina = loadModel("Escacs\\ReinaBlanc.obj");
	B_Rei = loadModel("Escacs\\ReiBlanc.obj");


	//NEGRES
	N_Peo = loadModel("Escacs\\PeoNegre.obj");
	N_Torre = loadModel("Escacs\\TorreNegre.obj");
	N_Cavall = loadModel("Escacs\\CavallNegre.obj");
	N_Alfil = loadModel("Escacs\\AlfilNegre.obj");
	N_Reina = loadModel("Escacs\\ReinaNegre.obj");
	N_Rei = loadModel("Escacs\\ReiNegre.obj");

	//Setup Mapa
	//m_modelMapa = loadModel("Mapas\\MAPAFINAL.obj");

	m_turretFloor = loadModel("Torretas\\torreBase.obj");
	m_turretRadius = loadModel("Torretas\\radius2.obj");
	//Setup Enemics, modelos en intérvalos [x, y] incluídos

	/////////////////////////////////////////////////////////////////// Basic [0,6] ///////////////////////////////////////////////////////////////////

	m_modelsEnemics.push_back(loadModel("Enemigos\\Basico.obj")); //Cuerpo
	COBJModel* ruedaDer = loadModel("Enemigos\\RuedaDER.obj");
	COBJModel* ruedaIzq = loadModel("Enemigos\\RuedaIZQ.obj");

	for (int i = 0; i < 3; i++) {
		m_modelsEnemics.push_back(ruedaDer); //Rueda
	}
	for (int i = 0; i < 3; i++) {
		m_modelsEnemics.push_back(ruedaIzq); //Rueda
	}
	/////////////////////////////////////////////////////////////////// Rapid [7,9] ///////////////////////////////////////////////////////////////////
	COBJModel* ruedaTipo2 = loadModel("Enemigos\\Rueda.obj");

	m_modelsEnemics.push_back(loadModel("Enemigos\\Rapido.obj")); //Cuerpo
	for (int i = 0; i < 2; i++) {
		m_modelsEnemics.push_back(ruedaTipo2); //Rueda
	}

	/////////////////////////////////////////////////////////////////// Tanc [10,14] ///////////////////////////////////////////////////////////////////
	m_modelsEnemics.push_back(loadModel("Enemigos\\Tanque.obj")); //Cuerpo
	for (int i = 0; i < 2; i++) {
		m_modelsEnemics.push_back(ruedaDer); //Rueda
	}
	for (int i = 0; i < 2; i++) {
		m_modelsEnemics.push_back(ruedaIzq); //Rueda
	}

	/////////////////////////////////////////////////////////////////// Volador [15,19]  ///////////////////////////////////////////////////////////////////
	COBJModel* helize = loadModel("Enemigos\\Helice.obj");

	m_modelsEnemics.push_back(loadModel("Enemigos\\Volador.obj")); //Cuerpo
	for (int i = 0; i < 4; i++) {
		m_modelsEnemics.push_back(helize); //Rueda
	}

	/////////////////////////////////////////////////////////////////// Accelerador/corredor [20,22] ///////////////////////////////////////////////////////////////////
	COBJModel* accelerador = loadModel("Enemigos\\Accelerador.obj");
	m_modelsEnemics.push_back(accelerador); //Cuerpo
	m_modelsEnemics.push_back(loadModel("Enemigos\\AcceleradorM1.obj")); //Rueda
	//AcceleradorACT  
	m_modelsEnemics.push_back(loadModel("Enemigos\\AcceleradorM2.obj")); //Rueda2

	/////////////////////////////////////////////////////////////////// Divisible [23,27] ///////////////////////////////////////////////////////////////////
	m_modelsEnemics.push_back(loadModel("Enemigos\\Divisible.obj")); //Cuerpo
	for (int i = 0; i < 4; i++) {
		m_modelsEnemics.push_back(ruedaTipo2); //Rueda
	}
	/////////////////////////////////////////////////////////////////// Dividid [28,20] ///////////////////////////////////////////////////////////////////
	m_modelsEnemics.push_back(loadModel("Enemigos\\Dividid.obj")); //Cuerpo
	for (int i = 0; i < 2; i++) {
		m_modelsEnemics.push_back(ruedaTipo2); //Rueda
	}


	//Setup Torretas [pares base, impares cabeza]
	//Metralleta
	m_modelsTorres.push_back(loadModel("Torretas\\turret3_base.obj"));
	m_modelsTorres.push_back(loadModel("Torretas\\turret3_head.obj"));

	//Hielo
	m_modelsTorres.push_back(loadModel("Torretas\\tesla2.obj"));
	m_modelsTorres.push_back(loadModel("Torretas\\empty.obj"));

	//Laser
	m_modelsTorres.push_back(loadModel("Torretas\\laser2_base.obj"));
	m_modelsTorres.push_back(loadModel("Torretas\\laser2_head.obj"));

	//Toxica
	m_modelsTorres.push_back(loadModel("Torretas\\toxic.obj"));
	m_modelsTorres.push_back(loadModel("Torretas\\toxicMOV.obj"));

	//Francotirador
	m_modelsTorres.push_back(loadModel("Torretas\\sniper.obj"));
	m_modelsTorres.push_back(loadModel("Torretas\\sniperMOV.obj"));

	//A partir de aquí se añaden todos modelos auxiliares extras como balas, efectos, etc...
	m_modelsTorres.push_back(loadModel("Torretas\\bala.obj"));
	m_modelsTorres.push_back(loadModel("Torretas\\radio.obj"));
	m_modelsTorres.push_back(loadModel("Torretas\\bola.obj"));
	m_modelsTorres.push_back(loadModel("Torretas\\rayo.obj"));

}

COBJModel* modelManager::loadModel(const std::string& filename)
{
	std::string fullPath = ".\\modelos\\" + filename;

	COBJModel* newModel = new COBJModel();
	newModel->LoadModel(fullPath.c_str());
	return newModel;
}

std::vector<COBJModel*> modelManager::getModelRange(const std::vector<COBJModel*>& src, int start, int end)
{
	//Control OOB
	if (src.empty() || start < 0 || end < start || start >= src.size())
		return {};

	//Limitar end
	if (end >= src.size())
		end = src.size() - 1;

	return std::vector<COBJModel*>(src.begin() + start, src.begin() + end + 1);
}

