#include "pickingObjects.h"


PickingObjects3D::PickingObjects3D(int width, int height, GLuint shaderID)
{
	m_poShaderID = shaderID;
	InitPO(width, height);
}

void PickingObjects3D::updatePickingObjectSize(int width, int height)
{
	glDeleteTextures(1, &m_pickingColorTex);
	glDeleteRenderbuffers(1, &m_pickingDepth);
	glDeleteFramebuffers(1, &m_pickingFB);

	InitPO(width, height);
}

void PickingObjects3D::InitPO(int w, int h)
{
	m_width = w;
	m_height = h;



	glGenFramebuffers(1, &m_pickingFB);
	glBindFramebuffer(GL_FRAMEBUFFER, m_pickingFB); //crear y activar el framebuffer


	//glViewport(0, 0, w, h);
	glGenTextures(1, &m_pickingColorTex);
	glBindTexture(GL_TEXTURE_2D, m_pickingColorTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); //crear y asignar textura de color, sino shader incompleto

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_pickingColorTex, 0);

	glGenRenderbuffers(1, &m_pickingDepth); //hacer que use el depth, para q no imprima cosas delante y tal
	glBindRenderbuffer(GL_RENDERBUFFER, m_pickingDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_width, m_height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_pickingDepth);


	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		//liadon, no se a creado bien
		fprintf(stderr, "ERROR");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0); //activar framebuffer default



}



void PickingObjects3D::renderPicking()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_pickingFB);

	glClearColor(0, 0, 0, 1);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 projectionMatrix, viewMatrix;
	projectionMatrix = (*m_cam)->getProjection();
	viewMatrix = (*m_cam)->getView();

	glUseProgram(m_poShaderID);
	glUniformMatrix4fv(glGetUniformLocation(m_poShaderID, "projectionMatrix"), 1, GL_FALSE, &projectionMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(m_poShaderID, "viewMatrix"), 1, GL_FALSE, &viewMatrix[0][0]);

	renderObjects();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

int PickingObjects3D::getClickedObject(GLFWwindow* window, CPoint mousePos)
{

	glBindFramebuffer(GL_FRAMEBUFFER, m_pickingFB);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	unsigned char data[4];
	glReadPixels(mousePos.x, m_height - mousePos.y - 1, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);

	int pickedID = data[0] +
		data[1] * 256 +
		data[2] * 256 * 256;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return pickedID;
	fprintf(stderr, "Objeto clicado con la id: %d\n", pickedID);
}

void PickingObjects3D::debug()
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_pickingFB);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBlitFramebuffer(0, 0, m_width, m_height, 0, 0, m_width, m_height,
	GL_COLOR_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PickingObjects3D::renderObjects()
{
	for (GameObject* o : *m_objectes)
	{
		o->dibuixarObjecte(m_poShaderID);
	}
	/*
	for (int i = 0; i < NTURRETS; i++)
		m_turrets[i]->draw(m_poShaderID);
		*/
}


