#include "iluminacion.h"

void Iluminacion::InitIluminacion(int width, int height)
{
	fprintf(stderr, "Shadow mapping shader: \n");
	m_shadowShader.releaseAllShaders();
	m_shadowShaderID = m_shadowShader.loadFileShaders(".\\shaders\\shadowMapping.vert", ".\\shaders\\shadowMapping.frag");


	w = width;
	h = height;

	glGenTextures(1, &m_depthMap); //textura depth
	glBindTexture(GL_TEXTURE_2D, m_depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 4096, 4096, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glGenFramebuffers(1, &m_depthMapFB);  //framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, m_depthMapFB);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);


}

void Iluminacion::UpdateWindow(int width, int height)
{
	w = width;
	h = height;
}

void Iluminacion::RenderShadows(glm::vec3 lightDir, float boxSize, float near_plane, float far_plane)
{

	
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(2.0f, 5.0f); // Valores comunes que suelen funcionar

	//float boxSize = 25;
	//float near_plane = 1.0f, far_plane = 100.0f;
	m_lightDirection = lightDir;

	glm::vec3 lightPos;

	lightPos = (normalize(m_lightDirection) * 35.0f);
	//lightPos = (*m_cam)->m_position + (normalize(m_lightDirection) * 50.0f);


	glm::mat4 lightProjection = glm::ortho(-boxSize, boxSize, -boxSize, boxSize, near_plane, far_plane);
	glm::mat4 lightView = glm::lookAt(lightPos, lightPos - normalize(m_lightDirection), glm::vec3(0.0f, 1.0f, 0.0f));
	m_lightSpaceMatrix = lightProjection * lightView;

	glUseProgram(m_shadowShaderID);
	glUniformMatrix4fv(glGetUniformLocation(m_shadowShaderID, "lightSpaceMatrix"), 1, GL_FALSE, &m_lightSpaceMatrix[0][0]);

	glViewport(0, 0, 4096, 4096);
	glBindFramebuffer(GL_FRAMEBUFFER, m_depthMapFB);
	glClear(GL_DEPTH_BUFFER_BIT);
	glCullFace(GL_FRONT);

	//dibujar objetos

	for (GameObject* obj : *objetos)
		obj->dibuixarObjecte(m_shadowShaderID);

	/*
	for (Enemy* en : *enemigos)
		en->draw(m_shadowShaderID);

	if(m_turretsLoaded)
		for (int i = 0; i < NTURRETS; i++) 
			turrets[i]->draw(m_shadowShaderID);
			*/
	glDisable(GL_POLYGON_OFFSET_FILL);
	glCullFace(GL_BACK);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void Iluminacion::RenderGame(GLuint shaderID, float ambientIntensity, glm::vec3 lightColor, int renderMode)
{


	m_ambientIntensity = ambientIntensity;
	m_lightColor = lightColor;


	glViewport(0, 0, w, h);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderID);

	glUniform1i(glGetUniformLocation(shaderID, "renderMode"), renderMode);



	glm::mat4 projectionMatrix, viewMatrix;
	projectionMatrix = (*m_cam)->getProjection();
	viewMatrix = (*m_cam)->getView();

	glUniformMatrix4fv(glGetUniformLocation(shaderID, "projectionMatrix"), 1, GL_FALSE, &projectionMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shaderID, "viewMatrix"), 1, GL_FALSE, &viewMatrix[0][0]);

	glUniformMatrix4fv(glGetUniformLocation(shaderID, "lightSpaceMatrix"), 1, GL_FALSE, &m_lightSpaceMatrix[0][0]);


	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_depthMap);
	glUniform1i(glGetUniformLocation(shaderID, "shadowMap"), 0);


	glUniform1f(glGetUniformLocation(shaderID, "ambientIntensity"), GLfloat(m_ambientIntensity));
	glUniform3fv(glGetUniformLocation(shaderID, "lightDirection"), 1, &m_lightDirection[0]);
	glUniform3fv(glGetUniformLocation(shaderID, "lightColor"), 1, &m_lightColor[0]);

	for (GameObject* obj : *objetos)
		obj->dibuixarObjecte(shaderID);

	/*for (Enemy* en : *enemigos)
		en->draw(shaderID);

	if (m_turretsLoaded)
		for (int i = 0; i < NTURRETS; i++)
			turrets[i]->draw(shaderID);
*/
	if (renderMode == PICKING_OBJECTS)
	{
		m_po->debug();
		return;
	}
}

