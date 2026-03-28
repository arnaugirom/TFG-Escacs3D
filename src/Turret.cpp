#include "Turret.h"

Enemy* Turret::selectTarget() 
{
	Enemy* target = nullptr;
	float maxProg = 0;

	for (Enemy* e : *enemies) 
	{
		glm::vec3 epos = e->getPos();
		float len = glm::distance(glm::vec2(epos.x, epos.y), m_pos);
		
		if (len > m_range || !e->isAlive()) continue;
		
		float prog = e->getProgress();
		if (prog > maxProg) 
		{
			maxProg = prog;
			target = e;
		}
	}

	return target;
}

std::vector<Enemy*> Turret::selectAllTargetsInRange()
{
	std::vector<Enemy*> targets;

	for (Enemy* e : *enemies)
	{
		glm::vec3 epos = e->getPos();
		float len = glm::distance(glm::vec2(epos.x, epos.y), m_pos);

		if (len <= m_range && e->isAlive())
			targets.push_back(e);
	}

	return targets;
}

void Turret::draw(GLuint shader)
{
	m_floor->dibuixarObjecte(shader);
	if (m_type == -1) return;
	m_baseObj->dibuixarObjecte(shader);
	m_headObj->dibuixarObjecte(shader);
	m_radio->dibuixarObjecte(shader);
	if (m_auxObject) m_auxObject->dibuixarObjecte(shader);
}

void Turret::loadTurret(int type, std::vector<COBJModel*> models)
{
	m_type = type;

	m_levelCD = 0;
	m_levelDmg = 0;
	m_levelRange = 0;

	if (m_baseObj != nullptr) delete m_baseObj;
	if (m_headObj != nullptr) delete m_headObj;
	m_auxModels.clear();

	if (type == -1) 
	{
		m_damage = 0; 
		m_range = 0; 
		m_defCD = 0; 
		m_cd = 0;
		m_headZ = 0;
		m_baseObj = nullptr;
		m_headObj = nullptr;
		return;
	}

	m_baseObj = new GameObject(models[0]);
	m_headObj = new GameObject(models[1]);

	for (int i = 2; i < models.size(); i++)
		m_auxModels.push_back(models[i]);

	m_baseObj->setPOID(m_poid);
	m_headObj->setPOID(m_poid);

	float zBase = 0.0f;

	switch (type) {
	case METRALLADORA:
		//Ajustar stats i z
		m_baseDMG = 0.5f;
		m_baseDEFCD = 0.5f;
		m_baseRNG = 8;
		zBase = -0.05f;
		m_headZ = 0.8f;
		break;
	case CONGELADORA:
		m_baseDMG = 0.5f;
		m_baseDEFCD = 3.5f;
		m_baseRNG = 5;
		zBase = -0.05f;
		m_headZ = 0.0f;
		break;
	case LASER:
		m_baseDMG = 0.65f;
		m_baseDEFCD = 1.5f;
		m_baseRNG = 5;
		zBase = -0.05f;
		m_headZ = 0.87f;
		break;
	case VERI:
		m_baseDMG = 0.4f;
		m_baseDEFCD = 1.0f;
		m_baseRNG = 6;
		zBase = -0.05f;
		m_headZ = 0.8f;
		break;
	case FRANCOTIRADORA:
		m_baseDMG = 2.5f;
		m_baseDEFCD = 2.0f;
		m_baseRNG = 13;
		zBase = -0.15f;
		m_headZ = 0.5f;
		break;
	default:
		return;
	}
	
	updateStats();
	m_baseObj->translate(glm::vec3(m_pos, zBase));
	m_headObj->translate(glm::vec3(m_pos, m_headZ));
}

void Turret::mainUpdate(float deltaTime)
{
	if (m_type < 0 || m_type > 4) return;

	animate(deltaTime);
	m_radio->rotate(glm::rotate(glm::mat4(1.0f), glm::radians(deltaTime) * 20, glm::vec3(0, 0, 1)));
	if (m_type == LASER) updateLaser(deltaTime);
	else if (m_type == CONGELADORA) updateIce(deltaTime);
	else updateCannon(deltaTime);
}

void Turret::updateStats() 
{
	m_defCD = m_baseDEFCD * (1 - m_levelCD * 0.15f);
	m_damage = m_baseDMG * (1 + m_levelDmg * 0.2f);
	m_range = m_baseRNG * (1 + m_levelRange * 0.10f);
	m_cd = m_defCD;
}

glm::vec3 Turret::getStatsUpgrade()
{
	return glm::vec3(
		m_baseDEFCD * (1 - (m_levelCD + 1) * 0.15f),
		m_baseDMG * (1 + (m_levelDmg + 1) * 0.2f),
		m_baseRNG * (1 + (m_levelRange + 1) * 0.10f));
}

void Turret::upgradeStat(int stat)
{
	if (stat == SHOOT_SPEED && m_levelCD < 3)
		m_levelCD++;
	else if (stat == DAMAGE && m_levelDmg < 3)
		m_levelDmg++;
	else if (stat == RANGE && m_levelRange < 3)
		m_levelRange++;

	updateStats();
	showRadio();

}

void Turret::updateLaser(float deltaTime)
{
	Enemy* target = selectTarget();
	if (target)
	{
		TurnHead(deltaTime, target->getPos());
		if (auxBool) 
		{
			auxVec3 = target->getPos() - vec3(0, 0, 0.25f);
			float laserDMG = max(target->getHealth() * 0.5f, m_defCD);
			target->takeDamage(laserDMG * deltaTime * m_damage);
			if (!m_auxObject) 
			{
				spawnAuxObj(0);
				glm::vec3 worldFW = glm::vec3(m_headObj->getRot() * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f));
				glm::quat rot = glm::rotation(glm::vec3(0, 0, 1), worldFW);
				m_auxObject->setRotation(glm::mat4_cast(rot));

				glm::vec3 cannonTip = glm::vec3(m_pos, m_headZ + 0.6f) + worldFW;
				glm::vec3 dir = auxVec3 - cannonTip;
				m_auxObject->translate(cannonTip + dir / 2.0f);

				m_auxObject->scale(glm::vec3(0.25f, 0.25f, glm::length(dir) / 2.0f));
			}
		}
		else
		{
			if (m_auxObject) deleteAuxObj();
		}
		
		return;
	}
	else if (m_auxObject) deleteAuxObj();
}

void Turret::updateCannon(float deltaTime) 
{
	Enemy* target = selectTarget();
	if (!target && m_cd < m_defCD) 
	{
		m_cd += deltaTime;
	}
	else if(target)
	{
		TurnHead(deltaTime, target->getPos());
		m_cd -= deltaTime;
		if (m_cd <= 0) 
		{
			shootAnimation();
			auxVec3 = target->getPos() - vec3(0, 0, 0.25f);

			if(m_type == VERI) target->setPoison(2.5f + m_damage * 1.25f);

			target->takeDamage(m_damage);
			m_cd = m_defCD;
		}
	}
}

void Turret::updateIce(float deltaTime)
{
	if (m_cd <= 0) 
	{
		std::vector<Enemy*> affectedEnemies = selectAllTargetsInRange();
		if (affectedEnemies.empty()) 
		{
			m_cd = 0.5f;
		}
		else
		{
			m_cd = m_defCD;

			shootAnimation();

			for (Enemy* e : affectedEnemies)
			{
				e->takeDamage(m_damage);
				e->setSlow(2.5f + m_damage);
			}
		}
	}
	else m_cd -= deltaTime;
}

void Turret::TurnHead(float deltaTime, glm::vec3 ePos)
{
	glm::vec3 headPos = m_headObj->getPos();
	glm::mat4 view = glm::lookAt(headPos, ePos - vec3(0, 0, 0.25f), glm::vec3(0, 0, 1));
	
	glm::mat4 modelRot = glm::inverse(view);

	glm::quat targetRot = glm::quat_cast(modelRot);

	glm::quat fixXForward = glm::angleAxis(glm::radians(90.0f), glm::vec3(0, 1, 0));
	glm::quat fixSideOffset = glm::angleAxis(glm::radians(90.0f), glm::vec3(0, 0, 1));

	targetRot = targetRot * fixSideOffset * fixXForward;

	glm::quat currentRot = glm::quat_cast(m_headObj->getRot());
	glm::quat smoothRot = glm::slerp(currentRot, targetRot, 0.2f);

	if (m_type == LASER) auxBool = abs(glm::dot(smoothRot, targetRot)) > 0.999f;

	m_headObj->setRotation(glm::mat4_cast(smoothRot));
}

void Turret::spawnAuxObj(int id) 
{
	if (m_auxObject) return;

	m_auxObject = new GameObject(m_auxModels[id]);
}

void Turret::deleteAuxObj() 
{
	if (m_auxObject != nullptr)
	{
		delete m_auxObject;
		m_auxObject = nullptr;
	}

}

void Turret::shootAnimation() 
{
	glm::vec3 headPos;
	glm::vec3 worldBackward;
	glm::vec3 recoilOffset;
	switch (m_type) {
	case METRALLADORA:
		auxBool = true;
		headPos = glm::vec3(m_pos, m_headZ);
		worldBackward = glm::vec3(m_headObj->getRot() * glm::vec4(-1.0f, 0.0f, 0.0f, 0.0f));
		recoilOffset = worldBackward * 0.25f;

		m_headObj->translate(headPos + recoilOffset);

		spawnAuxObj(0);
		m_auxObject->translate(glm::vec3(m_pos, 1.0f) - worldBackward * 1.45f);
		m_auxObject->scale(glm::vec3(0.1f, 0.1f, 0.1f));

		break;
	case CONGELADORA:
		spawnAuxObj(0);
		m_auxObject->translate(glm::vec3(m_pos, 0));
		m_auxObject->scale(glm::vec3(0.1f, 0.1f, 1));
		break;
	case VERI:
		auxBool = true;
		headPos = glm::vec3(m_pos, m_headZ);
		worldBackward = glm::vec3(m_headObj->getRot() * glm::vec4(-1.0f, 0.0f, 0.0f, 0.0f));
		recoilOffset = worldBackward * 0.2f;

		m_headObj->translate(headPos + recoilOffset);

		spawnAuxObj(0);
		m_auxObject->translate(glm::vec3(m_pos, 1.0f) - worldBackward * 1.45f);
		m_auxObject->scale(glm::vec3(0.25f, 0.25f, 0.25f));
		break;
	case FRANCOTIRADORA:
		auxBool = true;
		headPos = glm::vec3(m_pos, m_headZ);
		worldBackward = glm::vec3(m_headObj->getRot() * glm::vec4(-1.0f, 0.0f, 0.0f, 0.0f));
		recoilOffset = worldBackward * 0.25f;

		m_headObj->translate(headPos + recoilOffset);

		spawnAuxObj(0);
		m_auxObject->translate(glm::vec3(m_pos, 1.5f) - worldBackward * 1.8f);
		m_auxObject->scale(glm::vec3(0.1f, 0.1f, 0.1f));

		break;
	default: 
		return;
	}
}

void Turret::animate(float deltaTime)
{
	switch (m_type) {
	case METRALLADORA:
		if (auxBool) 
		{
			m_headObj->translate(glm::lerp(m_headObj->getPos(), glm::vec3(m_pos, m_headZ), 3.0f * deltaTime));
			if (glm::length(m_headObj->getPos() - glm::vec3(m_pos, m_headZ)) < 0.001f)
			{
				m_headObj->translate(glm::vec3(m_pos, m_headZ));
				auxBool = false;
			}
		}

		if (m_auxObject) 
		{
			m_auxObject->translate(glm::lerp(m_auxObject->getPos(), auxVec3, 25.0f * deltaTime));
			if (glm::length(m_auxObject->getPos() - auxVec3) < 0.1f)
				deleteAuxObj();
		}

		return;
	case CONGELADORA:
				
		if (m_auxObject == nullptr) return;
		m_auxObject->scale(glm::lerp(m_auxObject->getScale(), glm::vec3(m_range, m_range, 1), 7.5f * deltaTime));

		if (m_auxObject)
		{
			if (m_auxObject->getScale().x > m_range - 0.25f)
				deleteAuxObj();
		}

		return;
	case LASER:
		if (!m_auxObject) return;
		
		glm::vec3 worldFW = glm::vec3(m_headObj->getRot() * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f));
		glm::quat rot = glm::rotation(glm::vec3(0, 0, 1), worldFW);
		m_auxObject->setRotation(glm::mat4_cast(rot));

		glm::vec3 cannonTip = glm::vec3(m_pos, m_headZ + 0.6f) + worldFW;
		glm::vec3 dir = auxVec3 - cannonTip;
		m_auxObject->translate(cannonTip + dir / 2.0f);

		m_auxObject->scale(glm::vec3(0.25f, 0.25f, glm::length(dir) / 2.0f));

		return;
	case VERI:
		if (auxBool)
		{
			m_headObj->translate(glm::lerp(m_headObj->getPos(), glm::vec3(m_pos, m_headZ), 3.0f * deltaTime));
			if (glm::length(m_headObj->getPos() - glm::vec3(m_pos, m_headZ)) < 0.001f)
			{
				m_headObj->translate(glm::vec3(m_pos, m_headZ));
				auxBool = false;
			}
		}

		if (m_auxObject)
		{
			m_auxObject->translate(glm::lerp(m_auxObject->getPos(), auxVec3, 25.0f * deltaTime));
			if (glm::length(m_auxObject->getPos() - auxVec3) < 0.1f)
				deleteAuxObj();
		}

		return;
	case FRANCOTIRADORA:
		if (auxBool)
		{
			m_headObj->translate(glm::lerp(m_headObj->getPos(), glm::vec3(m_pos, m_headZ), 3.0f * deltaTime));
			if (glm::length(m_headObj->getPos() - glm::vec3(m_pos, m_headZ)) < 0.001f)
			{
				m_headObj->translate(glm::vec3(m_pos, m_headZ));
				auxBool = false;
			}
		}

		if (m_auxObject)
		{
			m_auxObject->translate(glm::lerp(m_auxObject->getPos(), auxVec3, 25.0f * deltaTime));
			if (glm::length(m_auxObject->getPos() - auxVec3) < 0.1f)
				deleteAuxObj();
		}

		return;
	default:
		return;
	}
}
