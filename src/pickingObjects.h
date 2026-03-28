#pragma once
#include "stdafx.h"
#include "gameObject.h"
#include "shader.h"
#include "camara.h"
#include "Turret.h"

class PickingObjects3D {
public:
	PickingObjects3D() {};
	PickingObjects3D(int width, int height, GLuint shaderID);
	void InitPO(int w, int h);

	void renderPicking();

	int getClickedObject(GLFWwindow* window, CPoint mousePos);
	void updatePickingObjectSize(int width, int height);
	void debug();

	std::vector<GameObject*>* m_objectes;
	Turret** m_turrets;
	Camara** m_cam;

private:
	int m_width, m_height;

	Shader m_shader;
	GLuint m_poShaderID;
	GLuint m_pickingFB;
	GLuint m_pickingColorTex;
	GLuint m_pickingDepth;


	void renderObjects();

};
