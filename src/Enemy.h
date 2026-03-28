#pragma once
#include "gameObject.h"
#include "Path.h"
#include <vector>

#define Basic 0
#define Rapid 1
#define Tanc 2
#define Volador 3
#define Accelerador 4
#define Divisible 5
#define AcceleradorACT 6
#define DivisibleDIV 7

#define Normal 0
#define Aceite 1
#define Nuclear 2
#define Baches 3



class Enemy : public GameObject {
public:
	Enemy(std::vector<COBJModel*> objModels, int enemyType) : GameObject(objModels[0])
	{
		m_alive = true;
		m_type = enemyType;
		setUpEnemyStats(Player::GetInstance().getDifficulty()); // Leer dificultad de Player.h y actualizar

		for (auto it = ++objModels.begin(); it != objModels.end(); ++it) {
			GameObject* newBodyPart = new GameObject(*it);
			newBodyPart->setParent(this);
			m_bodyParts.push_back(newBodyPart);
		}

		m_maskColor = true;
		m_colorBase = glm::vec4(0, 0, 0, 1);
	}

	~Enemy()
	{
		for (GameObject* obj : m_bodyParts)
			delete obj;
	}

	void setUpEnemyStats(float difficulty);
	void setTarget(Path* path) { m_target = path; }
	void setStartPoint(glm::vec2 startPoint);
	void animate(float timer, float deltaTime);
	void dieAnimation(float Deltatime);
	void move(float deltaTime, float timer);
	void reachPathEnd();
	void TrackPathProgress();
	void takeDamage(float damage); 	
	void die();
	void draw(GLuint shader);
	void startMoving();
	bool mustDestroy() { return m_alive; };
	bool isAlive() { return m_Move; };
	void kill() { m_alive = false; }
	float getProgress();
	float getHealth() { return m_health; }
	void setSlow(float sC) { m_slowCounter = sC; }
	void setPoison(float pC) { m_poisonCounter = pC; }
	int getEnemyType() { return m_type; }
	Path* getTarget() { return m_target; }
	void copyMovementData(Path* target);
	void setNPath(int nPath) { m_nSegments = nPath; }
	int getNPath() { return m_nSegments; }

private:
	int m_damage, m_reward, m_type, m_weight, m_pathType = 0, m_nSegments = 0;
	float m_health, m_defSpeed, m_speed, m_baseHealth, m_maxOffset = 0.5f, m_rotation = 0.0f, m_segmentProgress = 0.0f, m_poisonCounter = 0.0f, m_slowCounter = 0.0f;
	glm::vec2 m_dir, m_bisector, m_targetPos, m_prevTargetPos;
	bool m_alive = true;
	bool m_Move = true;

	std::vector<GameObject*> m_bodyParts;

	Path* m_target;
};
