//GAMEOBJECT.H
#pragma once
#include "modelManager.h"
#include <iostream>

class GameObject
{
public:
	GameObject(COBJModel* objModel);
	void translate(glm::vec3 pos);
	glm::vec3 getPos() { return m_pos; }
	void rotate(glm::mat4 rot);
	void setRotation(glm::mat4 rot) { m_rot = rot; }
	glm::mat4 getRot() { return m_rot; }
	void scale(glm::vec3 scale) { m_scale = scale; }
	glm::vec3 getScale() { return m_scale; }
	void setId(int id) { objectID = id; }
	void setParent(GameObject* parent) { m_parent = parent; }
	int getId() { return objectID; }
	glm::mat4 getModelMatrix();
	glm::mat4 getNormalMatrix();
	void setPOID(int id) { pickingID = id; }

	void setColor(glm::vec4 color);
	void clearColor();

	

	void dibuixarObjecte(GLuint shaderID);

protected:
	glm::mat4 m_parentMatrix;
	glm::vec3 m_pos;
	glm::mat4 m_rot;
	glm::vec3 m_scale;

	glm::vec4 m_colorBase;
	bool m_maskColor;
private:

	COBJModel* m_objModel;
	bool m_texture = false;
	int objectID;
	int pickingID;
	GameObject* m_parent = nullptr;
};

GameObject* createObject(COBJModel* model);
void placePiece(GameObject* obj, glm::vec3 pos, bool isKnight = false);