#include "camara.h"

Camara::Camara() {};

Camara::Camara(int width, int height)
{
	m_h = height;
	m_w = width;
	m_projectionMatrix = glm::mat4(1.0f);
	m_viewMatrix = glm::mat4(1.0f);

	m_position = glm::vec3(0, 0, 0);
	m_rotation = glm::mat4(1.0f);
	
	
	m_near = p_near;
	m_far = p_far;
	m_zoom = 60;
}

void Camara::rotate(glm::vec3 eulerAngles)
{
	glm::mat4 rotationMatrix(1.0f);

	//pitch - arriba abajo
	rotationMatrix = glm::rotate(rotationMatrix, glm::radians(eulerAngles.x), glm::vec3(1.0f, 0.0f, 0.0f));

	//yaw - izquierda derecha
	rotationMatrix = glm::rotate(rotationMatrix, glm::radians(eulerAngles.y), glm::vec3(0.0f, 1.0f, 0.0f));

	//roll - inclinar
	rotationMatrix = glm::rotate(rotationMatrix, glm::radians(eulerAngles.z), glm::vec3(0.0f, 0.0f, 1.0f));

	m_rotation = rotationMatrix;
}

void Camara::target(glm::vec3 target)
{


	glm::vec3 dir = target - m_position;
	m_rotation = glm::lookAt(glm::vec3(0, 0, 0), dir, glm::vec3(0, 0, 1));
}


glm::mat4 Camara::getProjection()
{
	if (m_h == 0) { m_h = 1; }
	m_projectionMatrix = glm::perspective(glm::radians(m_zoom), 1.0 * m_w / m_h, m_near, m_far);

	return m_projectionMatrix;
}

glm::mat4 Camara::getView()
{

	//vector forward de la camara
	glm::vec4 forwardBase = glm::vec4(0.0f, 0.0f, -1.0f, 0.0f); 
	glm::vec3 forward = glm::vec3(glm::inverse(m_rotation) * forwardBase);
	forward = glm::normalize(forward);

	//glm::vec3 target = m_position + forward;
	glm::vec3 target = m_position + forward;

	m_viewMatrix = glm::lookAt(m_position, target, glm::vec3(0, 0, 1));


	return m_viewMatrix;
}