#include "gameObject.h"

GameObject::GameObject(COBJModel* objModel)
{
	m_parentMatrix = glm::mat4(1.0f);
	m_objModel = objModel;
	m_pos = glm::vec3(0, 0, 0);
	m_rot = glm::mat4(1.0);
	m_scale = glm::vec3(1, 1, 1);
	objectID = -1;
	pickingID = 0;
	m_texture = false;
	m_parent = nullptr;
	m_maskColor = false;
	m_colorBase = glm::vec4(1, 1, 1, 1);
}

void GameObject::translate(glm::vec3 pos) 
{
	m_pos = pos;
}

void GameObject::rotate(glm::mat4 rot)
{
	m_rot = m_rot * rot;
}


extern std::vector<GameObject*> objects;

GameObject* createObject(COBJModel* model)
{
	GameObject* newObject = new GameObject(model);
	newObject->setId(objects.size());


	objects.push_back(newObject);
	
	std::cout << "Created object, size now dins el createObject: " << objects.size() << std::endl;
	
	return newObject;
}

void placePiece(GameObject* obj, glm::vec3 pos, bool isKnight) {
	obj->scale(glm::vec3(0.5f));
	obj->rotate(glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1, 0, 0)));

	if (isKnight) {
		obj->rotate(glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0, 1, 0)));
	}

	obj->translate(pos);
}












void GameObject::dibuixarObjecte(GLuint shaderID)
{

	int r = (pickingID & 0x000000FF) >> 0;
	int g = (pickingID & 0x0000FF00) >> 8;
	int b = (pickingID & 0x00FF0000) >> 16;
	glUniform4f(glGetUniformLocation(shaderID, "PickingColor"), r / 255.0f, g / 255.0f, b / 255.0f, 1.0f);

	glm::mat4 model(1.0f), normal(1.0f);


	glUniform1i(glGetUniformLocation(shaderID, "maskColor"), m_maskColor ? 1 : 0);
	glUniform4f(glGetUniformLocation(shaderID, "baseColor"), m_colorBase.r, m_colorBase.g, m_colorBase.b, m_colorBase.a);

	model = getModelMatrix();
	glUniformMatrix4fv(glGetUniformLocation(shaderID, "modelMatrix"), 1, GL_FALSE, &model[0][0]);
	
	//normal = transpose(inverse(view * model));
	normal = transpose(inverse(model));
	glUniformMatrix4fv(glGetUniformLocation(shaderID, "normalMatrix"), 1, GL_FALSE, &normal[0][0]);
	glUniform1i(glGetUniformLocation(shaderID, "textur"), false);
	m_objModel->draw_TriVAO_OBJ(shaderID);
}

glm::mat4 GameObject::getModelMatrix()
{
	glm::mat4 transMatrix = glm::mat4(1.0f);

	transMatrix = glm::translate(transMatrix, m_pos);   // mover al lugar
	transMatrix = transMatrix * m_rot;                  // rotar alrededor del centro
	transMatrix = glm::scale(transMatrix, m_scale);     // escalar

	if (m_parent)
		transMatrix = m_parent->getModelMatrix() * transMatrix;     // aplicar transformaciones del padre

	return transMatrix;
}

glm::mat4 GameObject::getNormalMatrix()
{
	glm::mat4 modelM = getModelMatrix();
	return transpose(inverse(modelM));
}
