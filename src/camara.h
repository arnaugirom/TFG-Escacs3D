#pragma once

#include "stdafx.h"
#include "constants.h"

class Camara {
public:
	Camara();
	Camara(int width, int height);
	void UpdateWindow(int width, int height) { m_w = width; m_h = height; }

	void translate(glm::vec3 pos) { m_position = pos; }
	void rotate(glm::mat4 rot) { m_rotation = rot; }
	void rotate(glm::vec3 eulerRot);
	void target(glm::vec3 target);

	Camara& operator=(const Camara& other)
	{
		if (this != &other)
		{
			this->m_projectionMatrix = other.m_projectionMatrix;
			this->m_viewMatrix = other.m_viewMatrix;
			this->m_position = other.m_position;
			this->m_rotation = other.m_rotation;

			this->m_near = other.m_near;
			this->m_far = other.m_far;
			this->m_zoom = other.m_zoom;
		}
		return *this;
	}

	glm::mat4 getProjection();
	glm::mat4 getView();

	glm::mat4 m_projectionMatrix;
	glm::mat4 m_viewMatrix;

	glm::vec3 m_position;
	glm::mat4 m_rotation;

private:
	
	double m_near, m_far;
	int m_w, m_h;
	double m_zoom;


};