#pragma once
#include "Enemy.h"
#include <vector>

#define METRALLADORA 0
#define CONGELADORA 1
#define LASER 2
#define VERI 3
#define FRANCOTIRADORA 4

#define SHOOT_SPEED 0
#define DAMAGE 1
#define RANGE 2

#define NTURRETS 9

class Turret {
public:
	Turret(int id){
		m_id = id;
		m_type = -1;
		m_poid = (m_id) * 26544;
	}

	~Turret() {
		//delete m_floor;
		delete m_baseObj;
		delete m_headObj;
		//delete m_radio;
		deleteAuxObj();
	}

	void draw(GLuint shader);
	void loadTurret(int type, std::vector<COBJModel*> models);
	void mainUpdate(float deltaTime);
	int getType() { return m_type; }
	void setEnemiesVector(std::vector<Enemy*>* e) { enemies = e; }
	void setPos(glm::vec2 pos) { m_pos = pos; }
	
	void setTurretFloor(COBJModel* f) 
	{ 
		m_floor = new GameObject(f);
		m_floor->translate(glm::vec3(m_pos, 0.034f));
		m_floor->setPOID((m_id) * 26544);

	}

	void setRadio(COBJModel* r)
	{
		m_radio = new GameObject(r);
		m_radio->translate(glm::vec3(m_pos, 0.2f));
	}
	
	void showRadio()
	{
		m_radio->scale(glm::vec3(m_range, m_range, 1.0f));
	}

	void hideRadio()
	{
		m_radio->scale(glm::vec3(0, 0, 0));
	}
	
	int getPOID() { return m_poid; }
	int getID() { return m_id; }
	void upgradeStat(int stat);
	void updateStats();
	glm::vec3 getStatsUpgrade();
	glm::vec3 getUpgradeLevel() { return glm::vec3(m_levelCD, m_levelDmg, m_levelRange); }
	glm::vec3 getStatValue() { return glm::vec3(m_defCD, m_damage, m_range); }

private:
	void updateLaser(float deltaTime);
	void updateCannon(float deltaTime);
	void updateIce(float deltaTime);
	void TurnHead(float deltaTime, glm::vec3 ePos);
	void shootAnimation();
	void animate(float deltatime);
	void spawnAuxObj(int id);
	void deleteAuxObj();

	Enemy* selectTarget();
	std::vector<Enemy*> selectAllTargetsInRange();

	int m_type = -1, m_id = 0, m_poid = 0, m_levelRange = 0, m_levelDmg = 0, m_levelCD = 0;
	float m_damage = 0, m_baseDMG = 0, m_range = 0, m_baseRNG = 0, m_defCD = 0, m_baseDEFCD = 0, m_cd = 0, m_headZ = 0;
	glm::vec2 m_pos;

	//Variables auxiliars per animacions
	glm::vec3 auxVec3 = glm::vec3(0.0f, 0.0f, 0.0f);
	bool auxBool = false;

	GameObject* m_baseObj = nullptr;
	GameObject* m_headObj = nullptr;
	GameObject* m_auxObject = nullptr;
	GameObject* m_floor = nullptr;
	GameObject* m_radio = nullptr;
	std::vector<COBJModel*> m_auxModels;
	std::vector<Enemy*>* enemies;
};