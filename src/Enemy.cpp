#include "Enemy.h"

void Enemy::setUpEnemyStats(float difficulty)
{
	float baseH = 15.0f;
	float baseS = 5.0f;
	switch (m_type) {
	case Basic:
		m_baseHealth = baseH * 0.55f;
		m_defSpeed = baseS * 0.5f;
		m_damage = 1;
		m_weight = 1;
		break;
	case Rapid:
		m_baseHealth = baseH * 0.3f;
		m_defSpeed = baseS * 1.0f;
		m_damage = 1;
		m_weight = 2;
		m_maxOffset = 0.4;
		break;
	case Tanc:
		m_baseHealth = baseH * 1.0f;
		m_defSpeed = baseS * 0.4f;
		m_damage = 2;
		m_weight = 3;
		m_maxOffset = 0.3;
		break;
	case Volador:
		m_baseHealth = baseH * 0.5f;
		m_defSpeed = baseS * 0.35f;
		m_damage = 2;
		m_weight = 3;
		break;
	case Accelerador:
		m_baseHealth = baseH * 0.4f;
		m_defSpeed = baseS * 0.4f;
		m_damage = 2;
		m_weight = 3;
		break;
	case Divisible:
		m_baseHealth = baseH * 0.5f;
		m_defSpeed = baseS * 0.4f;
		m_damage = 2;
		m_weight = 4;
		break;
		//Per donar l'efecte de transició en el l'enemic 5, l'eliminem i creem un nou. L'espai 6 està ocupat per identificar enemics 4 accelerats.
	case DivisibleDIV: //Enemic 5 dividit
		m_baseHealth = baseH * 0.3f;
		m_defSpeed = baseS * 0.4f;
		m_damage = 1;
		m_weight = 2;
		break;
	default:
		die();
	}

	m_baseHealth *= difficulty;
	m_speed *= (1.0f + difficulty) / 2.0f;
	m_health = m_baseHealth;
	m_reward = 100 + (int)((float)m_weight * (1 + difficulty * 0.15f) * 100.0f);
}

//Función auxiliar rotación
float normalizeAngle(float a) {
	a = fmod(a, 2.0f * PI);
	if (a <= -PI) a += 2.0f * PI;
	else if (a > PI) a -= 2.0f * PI;
	return a;
}

void Enemy::move(float deltaTime, float timer)
{
	dieAnimation(deltaTime);
	if (!m_Move || !m_target) return;

	float speed = m_speed * deltaTime * (1 - (max(m_slowCounter, 0.0f) / 4.5f));
	m_pos += glm::vec3(m_dir.x, m_dir.y, 0) * speed;

	TrackPathProgress();

	//Rotar
	float angleDiff = normalizeAngle(atan2(m_dir.y, m_dir.x) - glm::half_pi<float>() - m_rotation);
	m_rotation = normalizeAngle(m_rotation + angleDiff * 7.5f * deltaTime);
	m_rot = glm::rotate(glm::mat4(1.0f), m_rotation, glm::vec3(0, 0, 1));

	animate(timer, deltaTime);

	//Sistema de detección de giros temporal
	glm::vec2 relPos = glm::vec2(m_pos) - m_targetPos;
	if (glm::dot(relPos, m_bisector) >= 0.0f)
		reachPathEnd();

	if (m_slowCounter > 0)
	{
		m_slowCounter -= deltaTime;
		m_colorBase = glm::vec4(0.2f * (1 - sqrt(m_health / m_baseHealth)), m_poisonCounter / 4.5f, m_slowCounter / 4.5f, 1);
	}
	if (m_poisonCounter > 0)
	{
		m_poisonCounter -= deltaTime;
		takeDamage(deltaTime);
	}
}

void Enemy::dieAnimation(float deltatime) {
	if (m_Move) return;
	m_scale = glm::lerp(m_scale, glm::vec3(0.01f), 7.5f * deltatime);

	if (m_scale.x <= 0.1f)
		m_alive = false;
}



void Enemy::startMoving()
{
	if (!m_target) return;

	m_speed = m_defSpeed * m_target->getSpeedMultiplier();
	m_bisector = m_target->getBisector();
	m_targetPos = m_target->getPos();
	m_dir = glm::normalize(m_targetPos - m_prevTargetPos);

	m_rotation = atan2(m_dir.y, m_dir.x) - glm::half_pi<float>();
	m_rot = glm::rotate(glm::mat4(1.0f), m_rotation, glm::vec3(0, 0, 1));

	setStartPoint(m_prevTargetPos);
}

void Enemy::reachPathEnd()
{
	m_prevTargetPos = m_targetPos;
	m_target = m_target->getNextPath();
	m_nSegments++;
	m_segmentProgress = 0.0f;


	if (m_target == nullptr)
	{
		Player::GetInstance().modifyHealth(-m_damage);
		fprintf(stderr, "Health: %d\n", Player::GetInstance().getHealth());
		die();
		return;
	}

	m_speed = m_defSpeed * m_target->getSpeedMultiplier();
	m_bisector = m_target->getBisector();
	m_targetPos = m_target->getPos();
	m_dir = glm::normalize(m_targetPos - m_prevTargetPos);
	int newPType = m_target->getTipus();
	if (newPType != m_pathType)
	{
		m_pathType = newPType;
		//Esto se ejecuta solo en las transiciones entre diferentes tipos de caminos
		switch (m_type) {
		case Basic:
			translate(glm::vec3(m_pos.x, m_pos.y, 0.3));
			break;
		case Rapid:
			translate(glm::vec3(m_pos.x, m_pos.y, 0.4f));
			break;
		case Tanc:
			translate(glm::vec3(m_pos.x, m_pos.y, 0.22f));
			break;
		case Accelerador:
			translate(glm::vec3(m_pos.x, m_pos.y, 0.5f));
			break;
		case Divisible: case DivisibleDIV:
			translate(glm::vec3(m_pos.x, m_pos.y, 0.45f));
			break;

		default:
			break;
		}
	}


}

void Enemy::TrackPathProgress()
{
	glm::vec2 AB = m_targetPos - m_prevTargetPos;
	glm::vec2 AP = glm::vec2(m_pos.x, m_pos.y) - m_prevTargetPos;

	m_segmentProgress = glm::clamp(glm::dot(AP, AB) / glm::dot(AB, AB), 0.0f, 1.0f);
}


void Enemy::animate(float timer, float deltaTime)
{
	//De moment farem una animació per cada enemic, però podem crear variants pels líquids o terres trencats
	glm::mat4 matriz_rotacion, matriz_rotacionAceite, MatrizCuerpo;

	switch (m_type) {
	case Basic:
		matriz_rotacion = glm::rotate(glm::mat4(1.0f), -5.0f * m_speed * deltaTime / m_defSpeed, glm::vec3(1.0f, 0.0f, 0.0f));

		// --- GRUPO IZQUIERDO (Piezas 0, 1, 2)
		// No necesitan la rotación de 180 grados (están mirando hacia adelante por defecto)
		m_bodyParts[0]->translate(glm::vec3(-0.18, -0.077, -0.25));
		m_bodyParts[1]->translate(glm::vec3(-0.18, 0.077, -0.25));
		m_bodyParts[2]->translate(glm::vec3(-0.18, 0.004, -0.119));

		// Aplicar la rotación continua (rueda)
		m_bodyParts[0]->rotate(matriz_rotacion);
		m_bodyParts[1]->rotate(matriz_rotacion);
		m_bodyParts[2]->rotate(matriz_rotacion);

		// --- GRUPO DERECHO (Piezas 3, 4, 5)

		// 1. Posicionar
		m_bodyParts[3]->translate(glm::vec3(0.18, -0.07, -0.25));
		m_bodyParts[4]->translate(glm::vec3(0.18, 0.076, -0.25));
		m_bodyParts[5]->translate(glm::vec3(0.18, 0.004, -0.12));

		// 3. Aplicar la rotación continua (rueda)
		m_bodyParts[3]->rotate(matriz_rotacion);
		m_bodyParts[4]->rotate(matriz_rotacion);
		m_bodyParts[5]->rotate(matriz_rotacion);

		if (m_pathType == Aceite)
		{
			translate(glm::vec3(m_pos.x + sin(timer * 10) * 0.0015f, m_pos.y, m_pos.z));
			matriz_rotacionAceite = glm::rotate(glm::mat4(1.0f), sin(timer * 10.0f) * glm::radians(3.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			rotate(matriz_rotacionAceite); // Aplicar la rotación
		}
		else {
			if (m_pathType == Baches)
			{
				translate(glm::vec3(m_pos.x, m_pos.y, 0.3 + abs(sin(timer * 10) * 0.1f)));
			}
		}
		break;

	case Rapid:
		m_bodyParts[0]->translate(glm::vec3(0, -0.33, -0.24));
		m_bodyParts[1]->translate(glm::vec3(0, 0.54, -0.24));

		matriz_rotacion = glm::rotate(glm::mat4(1.0f), -7.5f * m_speed * deltaTime / m_defSpeed, glm::vec3(1.0f, 0.0f, 0.0f));

		m_bodyParts[0]->rotate(matriz_rotacion);
		m_bodyParts[1]->rotate(matriz_rotacion);

		if (m_pathType == Aceite)
		{
			translate(glm::vec3(m_pos.x + sin(timer * 10) * 0.0015f, m_pos.y, m_pos.z));
			matriz_rotacionAceite = glm::rotate(glm::mat4(1.0f), sin(timer * 10.0f) * glm::radians(3.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			rotate(matriz_rotacionAceite); // Aplicar la rotación
		}
		else {
			if (m_pathType == Baches)
			{
				translate(glm::vec3(m_pos.x, m_pos.y, 0.4 + abs(sin(timer * 10) * 0.1f)));
			}
		}
		break;

	case Tanc:
		matriz_rotacion = glm::rotate(glm::mat4(1.0f), -2.0f * m_speed * deltaTime / m_defSpeed, glm::vec3(1.0f, 0.0f, 0.0f));

		// --- GRUPO IZQUIERDO (Piezas 0, 1)
		// No necesitan la rotación de 180 grados (están mirando hacia adelante por defecto)
		m_bodyParts[0]->scale(glm::vec3(1.8, 1.8, 1.8));
		m_bodyParts[0]->translate(glm::vec3(-0.35, 0.5, -0.12));
		m_bodyParts[1]->scale(glm::vec3(3, 3, 3));
		m_bodyParts[1]->translate(glm::vec3(-0.35, -0.18, 0));
		// Aplicar la rotación continua (rueda)
		m_bodyParts[0]->rotate(matriz_rotacion);
		m_bodyParts[1]->rotate(matriz_rotacion);
		// --- GRUPO DERECHO (Piezas 3, 4)
		// 1. Posicionar
		m_bodyParts[2]->scale(glm::vec3(1.8, 1.8, 1.8));
		m_bodyParts[2]->translate(glm::vec3(0.35, 0.5, -0.12));
		m_bodyParts[3]->scale(glm::vec3(3, 3, 3));
		m_bodyParts[3]->translate(glm::vec3(0.35, -0.18, 0));
		// 3. Aplicar la rotación continua (rueda)
		m_bodyParts[2]->rotate(matriz_rotacion);
		m_bodyParts[3]->rotate(matriz_rotacion);
		if (m_pathType == Aceite)
		{
			translate(glm::vec3(m_pos.x + sin(timer * 10) * 0.0015f, m_pos.y, m_pos.z));
			matriz_rotacionAceite = glm::rotate(glm::mat4(1.0f), sin(timer * 10.0f) * glm::radians(3.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			rotate(matriz_rotacionAceite); // Aplicar la rotación
		}
		break;

	case Volador:
		matriz_rotacion = glm::rotate(glm::mat4(1.0f), -15.0f * m_speed * deltaTime / m_defSpeed, glm::vec3(0.0f, 0.0f, 1.0f));

		m_bodyParts[0]->translate(glm::vec3(-0.4, -0.55, 0.05));
		m_bodyParts[1]->translate(glm::vec3(-0.35, 0.5, 0.05));
		m_bodyParts[2]->translate(glm::vec3(0.37, -0.55, 0.05));
		m_bodyParts[3]->translate(glm::vec3(0.37, 0.5, 0.05));

		m_bodyParts[0]->rotate(matriz_rotacion);
		m_bodyParts[1]->rotate(matriz_rotacion);
		m_bodyParts[2]->rotate(matriz_rotacion);
		m_bodyParts[3]->rotate(matriz_rotacion);

		//MatrizCuerpo = glm::rotate(glm::mat4(1.0f), sin(timer * 10.0f) * glm::radians(10.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		//rotate(MatrizCuerpo);
		translate(glm::vec3(m_pos.x, m_pos.y, m_pos.z + sin(timer * 2.5) * 0.0025f));

		break;

	case Accelerador:

		// Crear y aplicar la matriz de rotación
		matriz_rotacion = glm::rotate(glm::mat4(1.0f), -5.0f * m_speed * deltaTime / m_defSpeed, glm::vec3(1.0f, 0.0f, 0.0f));


		m_bodyParts[0]->rotate(matriz_rotacion); // Aplicar la rotación
		m_bodyParts[0]->scale(glm::vec3(1, 1, 1));
		m_bodyParts[1]->scale(glm::vec3(0.5, 0.5, 0.5));

		if (m_pathType == Aceite)
		{
			translate(glm::vec3(m_pos.x + sin(timer * 10) * 0.0025f, m_pos.y, m_pos.z));
			matriz_rotacionAceite = glm::rotate(glm::mat4(1.0f), sin(timer * 10.0f) * glm::radians(5.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			rotate(matriz_rotacionAceite); // Aplicar la rotación
		}
		else if (m_pathType == Baches)
		{
			translate(glm::vec3(m_pos.x, m_pos.y, 0.5 + abs(sin(timer * 10) * 0.2f)));
		}
		break;

	case AcceleradorACT:

		// Crear y aplicar la matriz de rotación
		matriz_rotacion = glm::rotate(glm::mat4(1.0f), -5.0f * m_speed * deltaTime / m_defSpeed, glm::vec3(1.0f, 0.0f, 0.0f));

		m_bodyParts[1]->rotate(matriz_rotacion); // Aplicar la rotación
		m_bodyParts[0]->scale(glm::vec3(0.5, 0.5, 0.5));
		m_bodyParts[1]->scale(glm::vec3(1, 1, 1));

		if (m_pathType == Aceite)
		{
			translate(glm::vec3(m_pos.x + sin(timer * 10) * 0.0025f, m_pos.y, m_pos.z));
			matriz_rotacionAceite = glm::rotate(glm::mat4(1.0f), sin(timer * 10.0f) * glm::radians(5.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			rotate(matriz_rotacionAceite); // Aplicar la rotación
		}
		else if (m_pathType == Baches)
		{
			translate(glm::vec3(m_pos.x, m_pos.y, 0.5 + abs(sin(timer * 10) * 0.2f)));
		}


		break;


	case Divisible:
		// Lado derecho
		m_bodyParts[0]->translate(glm::vec3(-0.14, -0.2, -0.32));
		m_bodyParts[1]->translate(glm::vec3(-0.14, 0.22, -0.32));

		// Lado izquierdo
		m_bodyParts[2]->translate(glm::vec3(0.14, -0.2, -0.32));
		m_bodyParts[3]->translate(glm::vec3(0.14, 0.22, -0.32));

		matriz_rotacion = glm::rotate(glm::mat4(1.0f), -5.0f * m_speed * deltaTime / m_defSpeed, glm::vec3(1.0f, 0.0f, 0.0f));

		m_bodyParts[0]->rotate(matriz_rotacion);
		m_bodyParts[1]->rotate(matriz_rotacion);
		m_bodyParts[2]->rotate(matriz_rotacion);
		m_bodyParts[3]->rotate(matriz_rotacion);

		if (m_pathType == Aceite)
		{
			translate(glm::vec3(m_pos.x + sin(timer * 10) * 0.0015f, m_pos.y, m_pos.z));
			matriz_rotacionAceite = glm::rotate(glm::mat4(1.0f), sin(timer * 10.0f) * glm::radians(3.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			rotate(matriz_rotacionAceite); // Aplicar la rotación
		}
		else {
			if (m_pathType == Baches)
			{
				translate(glm::vec3(m_pos.x, m_pos.y, 0.4 + abs(sin(timer * 10) * 0.1f)));
			}
		}
		break;

	case DivisibleDIV:
		m_bodyParts[0]->translate(glm::vec3(0, -0.18, -0.31));
		m_bodyParts[1]->translate(glm::vec3(0, 0.24, -0.31));

		matriz_rotacion = glm::rotate(glm::mat4(1.0f), -5.0f * m_speed * deltaTime / m_defSpeed, glm::vec3(1.0f, 0.0f, 0.0f));

		m_bodyParts[0]->rotate(matriz_rotacion);
		m_bodyParts[1]->rotate(matriz_rotacion);

		if (m_pathType == Aceite)
		{
			translate(glm::vec3(m_pos.x + sin(timer * 10) * 0.0015f, m_pos.y, m_pos.z));
			matriz_rotacionAceite = glm::rotate(glm::mat4(1.0f), sin(timer * 10.0f) * glm::radians(3.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			rotate(matriz_rotacionAceite); // Aplicar la rotación
		}
		else {
			if (m_pathType == Baches)
			{
				translate(glm::vec3(m_pos.x, m_pos.y, 0.4 + abs(sin(timer * 10) * 0.1f)));
			}
		}

		break;

	default:
		break;
	}
}

void Enemy::takeDamage(float damage)
{
	m_health -= damage;
	if (m_health <= 0)
	{
		die();
	}
	else
	{
		m_colorBase = glm::vec4(0.2f * (1 - sqrt(m_health / m_baseHealth)), m_poisonCounter / 4.5f, m_slowCounter / 4.5f, 1);
	}
}

void Enemy::die()
{
	if (m_target != nullptr)
		switch (m_type) {
		case Basic: case Rapid: case Tanc: case Volador: case AcceleradorACT: case DivisibleDIV:
			Player::GetInstance().modifyMoney(m_reward);
			Player::GetInstance().enemyDefeated();
			break;
		case Accelerador:
			m_health = m_baseHealth;
			m_defSpeed *= 2.5f;
			m_speed = m_defSpeed * m_target->getSpeedMultiplier();
			m_damage = 1;
			m_type = 6;
			return;
		default:
			break;
		}

	fprintf(stderr, "Money: %d\n", Player::GetInstance().getMoney());
	m_Move = false;
}

void Enemy::draw(GLuint shader)
{
	this->dibuixarObjecte(shader);
	for (GameObject* g : m_bodyParts)
		g->dibuixarObjecte(shader);
}

void Enemy::setStartPoint(glm::vec2 startPoint)
{
	m_prevTargetPos = startPoint;
	float m_offset = (((float)rand() / (float)RAND_MAX) * 2.0f - 1.0f) * m_maxOffset;
	float z;
	switch (m_type) {
	case Basic:
		z = 0.3f;
		break;
	case Rapid:
		z = 0.4f;
		break;
	case Volador:
		z = 1.25;
		break;
	case Tanc:
		z = 0.22f;
		break;
	case Accelerador: case AcceleradorACT:
		z = 0.5f;
		break;
	case Divisible: case DivisibleDIV:
		z = 0.45f;
		break;
	default:
		z = 0.3f;
		break;
	}
	m_pos = glm::vec3(startPoint.x, startPoint.y + m_offset, z);
}

float Enemy::getProgress()
{
	if (m_type == Volador)
		return m_segmentProgress * 30;

	else
		return (float)m_nSegments + m_segmentProgress;
}

void Enemy::copyMovementData(Path* target)
{
	m_target = target;
	m_prevTargetPos = target->getPrevPath()->getPos();
	m_speed = m_defSpeed * m_target->getSpeedMultiplier();
	m_bisector = m_target->getBisector();
	m_targetPos = m_target->getPos();
	m_dir = glm::normalize(m_targetPos - m_prevTargetPos);

	m_rotation = atan2(m_dir.y, m_dir.x) - glm::half_pi<float>();
	m_rot = glm::rotate(glm::mat4(1.0f), m_rotation, glm::vec3(0, 0, 1));
}
