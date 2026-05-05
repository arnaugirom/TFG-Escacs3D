//MAIN.CPP

//******** PRACTICA VISUALITZACI� GR�FICA INTERACTIVA (Escola Enginyeria - UAB)
//******** Entorn b�sic VS2022 MONOFINESTRA amb OpenGL 4.6, interf�cie GLFW 3.4, ImGui i llibreries GLM
//******** Ferran Poveda, Marc Vivet, Carme Juli�, D�bora Gil, Enric Mart� (Setembre 2025)
// main.cpp : Definici� de main
//    Versi� 0.5:	- Interficie ImGui
//					- Per a dialeg de cerca de fitxers, s'utilitza la llibreria NativeFileDialog


// Entorn VGI.ImGui: Includes llibreria ImGui
//#define STB_IMAGE_IMPLEMENTATION  // <--- ESTO ES VITAL
//#include "stb_image.h"

#include "ImGui\imgui.h"
#include "ImGui\imgui_impl_glfw.h"
#include "ImGui\imgui_impl_opengl3.h"
#include "ImGui\nfd.h" // Native File Dialog

#include "stdafx.h"
#include "shader.h"
#include "visualitzacio.h"
#include "escena.h"
#include "main.h"
#include "menu.h"
#include "gameObject.h"
#include "gameState.h"

#include "vosk_api.h"
#include <portaudio.h>
#include <map>
#include <string>
#include <sstream>

#include <algorithm>

#include <iostream>
using namespace std;

VoskModel* model = nullptr;
VoskRecognizer* rec = nullptr;



bool pieceSelected = false;
std::string selectedSquare = "";
GameObject* selectedObject = nullptr;
Piece* selectedPiece = nullptr;
GameObject* selectionMarker = nullptr;




GameObject* movingObject = nullptr;
std::vector<GameObject*> objects;




bool moving = false;
glm::vec3 startPos, targetPos;
float moveSpeed = 2.0f; // ajusta velocitat
float moveProgress = 0.0f;

void movePieceTo(GameObject* obj, glm::vec3 newPos)
{
	movingObject = obj;   // guarda quin objecte es mou
	startPos = obj->getPos();
	targetPos = newPos;
	moveProgress = 0.0f;
	moving = true;
}

void moveKnight(GameObject* knight, glm::vec3 offset)
{
	glm::vec3 current = knight->getPos();
	movePieceTo(knight, current + offset);
}



void highlightObject(GameObject* obj, bool highlight)
{
	if (!obj) return;

	if (highlight)
		obj->setColor(glm::vec4(1, 0, 0, 1)); // vermell
	else
		obj->clearColor(); // torna a normal
}
/*void moveMarkerTo(glm::vec3 pos)
{
	if (!selectionMarker) return;

	glm::vec3 markerPos = pos;
	markerPos.z += 0.25f; // una mica per sobre

	selectionMarker->translate(markerPos);
}
*/



std::string normalize(std::string s)
{
	std::transform(s.begin(), s.end(), s.begin(), ::tolower);

	std::map<std::string, char> mapNums = {
		{" u",'1'},
		{"un", '1'},
		
		{"dos",'2'}, {"tres",'3'}, 
		
		{"quatre",'4'},
		{"quatro",'4'},

		{"cinc",'5'}, {"sis",'6'}, {"set",'7'}, 

		{"vuit", '8'},
		{"vuyt", '8'},
		{"buit", '8'}
	};

	for (auto& [word, num] : mapNums)
	{
		size_t pos;
		while ((pos = s.find(word)) != std::string::npos)
		{
			s.replace(pos, word.length(), std::string(1, num));
		}
	}

	return s; // NO eliminar espais
}

std::string extractSquare(const std::string& word)
{
	std::string s = word;
	std::transform(s.begin(), s.end(), s.begin(), ::tolower);

	// elimina espais
	s.erase(remove(s.begin(), s.end(), ' '), s.end()); // Ara queda a2

	if (s.size() < 2) return "";

	char col = s[0];
	char row = s[1];

	// si ve com "a2"
	if (col >= 'a' && col <= 'h' && row >= '1' && row <= '8')
	{
		return std::string(1, toupper(col)) + row;
	}

	return "";
}

std::pair<int, int> parsePos(std::string pos)
{
	char col = pos[0];   // A
	char row = pos[1];   // 2

	int x = col - 'A';
	int y = (row - '1');

	return { x, y };
}

void promote(std::string type)
{
	if (!promotionPiece || !promotionCell) return;

	int x = promotionPiece->getX();
	int y = promotionPiece->getY();
	Color c = promotionPiece->getColor();

	// 🔥 BORRAR peón
	delete promotionPiece;

	// 🔥 BORRAR modelo antiguo
	destroyObject(promotionCell->obj);

	Piece* newPiece = nullptr;
	GameObject* newObj = nullptr;

	if (type == "torre")
	{
		newPiece = new Rook(x, y, c);
		newObj = createObject(mm.getBTorre());
	}
	else if (type == "cavall")
	{
		newPiece = new Knight(x, y, c);
		newObj = createObject(mm.getBCavall());
	}
	else if (type == "alfil")
	{
		newPiece = new Bishop(x, y, c);
		newObj = createObject(mm.getBAlfil());
	}
	else if (type == "reina")
	{
		newPiece = new Queen(x, y, c);
		newObj = createObject(mm.getBReina());
	}

	// colocar pieza
	glm::vec3 pos = promotionCell->posicions;
	placePiece(newObj, pos);

	promotionCell->piece = newPiece;
	promotionCell->obj = newObj;

	waitingPromotion = false;
	promotionPiece = nullptr;
	promotionCell = nullptr;



	std::cout << "PROMOCIÓ COMPLETADA!" << std::endl;
}

void processVoiceCommand(std::string command, Board* board)
{
	std::cout << "COMMAND: " << command << std::endl;

	if (waitingPromotion)
	{
		std::string cmd = command;

		if (cmd.find("torre") != std::string::npos)
			promote("torre");
		else if (cmd.find("cavall") != std::string::npos)
			promote("cavall");
		else if (cmd.find("alfil") != std::string::npos)
			promote("alfil");
		else if (cmd.find("reina") != std::string::npos)
			promote("reina");

		return;
	}

	std::string square = extractSquare(command);

	if (square == "")
	{
		std::cout << "Comanda no valida" << std::endl;
		return;
	}

	auto [x, y] = parsePos(square);

	// ----------------------------------------
	// CAS 1: NO HI HA PEÇA SELECCIONADA
	// ----------------------------------------
	if (!pieceSelected)
	{
		Piece* p = board->get(x, y);

		if (!p)
		{
			std::cout << "No hi ha pesa a " << square << std::endl;
			return;
		}

		selectedPiece = p;
		selectedObject = board->getCell(x, y).obj;
		//moveMarkerTo(selectedObject->getPos());
		selectedSquare = square;
		pieceSelected = true;

		highlightObject(selectedObject, true);

		std::cout << "Pesa seleccionada a " << square << std::endl;
		return;
	}

	// ----------------------------------------
	// CAS 2: JA HI HA PEÇA SELECCIONADA
	// ----------------------------------------

	// si repeteixes la mateixa → cancel·la
	if (square == selectedSquare)
	{
		highlightObject(selectedObject, false);
		//selectionMarker->translate(glm::vec3(0, 0, -100)); // amagar-lo

		pieceSelected = false;
		selectedSquare = "";
		selectedObject = nullptr;
		selectedPiece = nullptr;

		std::cout << "Seleccio cancelada" << std::endl;
		return;
	}

	// intent de moviment
	auto [x2, y2] = parsePos(square);

	std::vector<std::pair<int, int>> moves = selectedPiece->getMoves(board);

	bool valid = false;
	for (auto& m : moves)
	{
		if (m.first == x2 && m.second == y2)
		{
			valid = true;
			break;
		}
	}

	if (!valid)
	{
		std::cout << "Moviment invalid" << std::endl;
		return;
	}

	glm::vec3 newPos = taulell->getCell(x2, y2).posicions;

	movePieceTo(selectedObject, newPos);
	board->movePiece(selectedPiece, x2, y2);

	// desmarcar
	highlightObject(selectedObject, false);
	//selectionMarker->translate(glm::vec3(0, 0, -100)); // amagar-lo

	pieceSelected = false;
	selectedSquare = "";
	selectedObject = nullptr;
	selectedPiece = nullptr;

	std::cout << "Moviment fet!" << std::endl;
}










void InitGL()
{
// TODO: agregar aqu� el c�digo de construcci�n

//------ Entorn VGI: Inicialitzaci� de les variables globals de CEntornVGIView
	int i;

// Entorn VGI: Variable de control per a Status Bar (consola) 
	statusB = false;

// Entorn VGI: Variables de control per Men� Vista: Pantalla Completa, Pan, dibuixar eixos i grids 
	fullscreen = false;
	eixos = true;

// Entorn VGI: Variables de control Skybox Cube
	SkyBoxCube = false;		skC_programID = 0;
	skC_VAOID.vaoId = 0;	skC_VAOID.vboId = 0;	skC_VAOID.nVertexs = 36;
	cubemapTexture = 0;


// Entorn VGI: Variables de control del men� Shaders
	shader = CAP_SHADER;	shader_programID = 0;	
	shaderLighting.releaseAllShaders();
	// C�rrega Shader de Gouraud
	shader_programID = 0;
	fprintf(stderr, "phong_shdrML: \n"); 
	shader = PHONG_SHADER;

	fprintf(stderr, "Custom shader: \n");
	customShader.releaseAllShaders();
	customShaderID = customShader.loadFileShaders(".\\shaders\\customShader.vert", ".\\shaders\\customShader.frag");


	fprintf(stderr, "Picking objects shader: \n");
	poShader.releaseAllShaders();
	poShaderID = poShader.loadFileShaders(".\\shaders\\pickingObject.vert", ".\\shaders\\pickingObject.frag");


// C�rrega SHADERS
// C�rrega Shader Eixos
	fprintf(stderr, "Eixos: \n");
	if (!eixos_programID) eixos_programID = shaderEixos.loadFileShaders(".\\shaders\\eixos.VERT", ".\\shaders\\eixos.FRAG");

// C�rrega Shader Skybox
	fprintf(stderr, "SkyBox: \n");
	if (!skC_programID) skC_programID = shader_SkyBoxC.loadFileShaders(".\\shaders\\skybox.VERT", ".\\shaders\\skybox.FRAG");

// C�rrega VAO Skybox Cube
	if (skC_VAOID.vaoId == 0) skC_VAOID = loadCubeSkybox_VAO();
	Set_VAOList(CUBE_SKYBOX, skC_VAOID);

	if (!cubemapTexture)
	{	// load Skybox textures
		// -------------
		std::vector<std::string> faces =
		{ ".\\textures\\skybox\\right.jpg",
			".\\textures\\skybox\\left.jpg",
			".\\textures\\skybox\\top.jpg",
			".\\textures\\skybox\\bottom.jpg",
			".\\textures\\skybox\\front.jpg",
			".\\textures\\skybox\\back.jpg"
		};
		cubemapTexture = loadCubemap(faces);
	}


// Entorn VGI: Variables de control dels botons de mouse
	m_PosEAvall.x = 0;			m_PosEAvall.y = 0;			m_PosDAvall.x = 0;			m_PosDAvall.y = 0;
	m_ButoEAvall = false;		m_ButoDAvall = false;
	m_EsfeEAvall.R = 0.0;		m_EsfeEAvall.alfa = 0.0;	m_EsfeEAvall.beta = 0.0;
	m_EsfeIncEAvall.R = 0.0;	m_EsfeIncEAvall.alfa = 0.0;	m_EsfeIncEAvall.beta = 0.0;

// Entorn VGI: Variables que controlen par�metres visualitzaci�: Mides finestra Windows i PV
	w = 640;			h = 480;			// Mides de la finestra Windows (w-amplada,h-al�ada)
	width_old = 640;	height_old = 480;	// Mides de la resoluci� actual de la pantalla (finestra Windows)
	w_old = 640;		h_old = 480;		// Mides de la finestra Windows (w-amplada,h-al�ada) per restaurar Finestra des de fullscreen
	OPV.R = cam_Esferica[0];	OPV.alfa = cam_Esferica[1];		OPV.beta = cam_Esferica[2];		// Origen PV en esf�riques
	//OPV.R = 15.0;		OPV.alfa = 0.0;		OPV.beta = 0.0;										// Origen PV en esf�riques
	Vis_Polar = POLARZ;	oPolars = -1;

// Entorn VGI: Altres variables
	mida = 1.0;			nom = "";		buffer = "";
	

	
	/* ------------------------------------------------------------------------- */
	/*                               CARGA DE MODELS                             */
	/* ------------------------------------------------------------------------- */

	
	

	mm = modelManager();
	mm.initialSetup();


	// CREAR OBJECTE TAULELL
	taulell = new Board();
	taulell->Inicialitza_Taulell(mm);

	std::cout << "Objects size: " << objects.size() << std::endl;


	/*
	selectionMarker = createObject(mm.getTaulell()); // reutilitzem model simple
	selectionMarker->scale(glm::vec3(0.2f, 0.2f, 0.1f)); // pla finet
	selectionMarker->rotate(glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1, 0, 0)));
	selectionMarker->setColor(glm::vec4(0, 0, 1, 1)); // blau
	selectionMarker->setPOID(0); // perquè no interfereixi amb picking
	*/

	/*
	//Posar TAULELL
	GameObject* TAULELL = createObject(mm.getTaulell());

	TAULELL->rotate(glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1, 0, 0)));
	
	


	peoA2 = createObject(mm.getBPeo());
	placePiece(peoA2, glm::vec3(-3.5f, -2.5f, -0.2f)); // A2

	cavallB1 = createObject(mm.getBCavall());
	placePiece(cavallB1, glm::vec3(-2.5f, -3.5f, -0.15f), true);

	
	placePiece(createObject(mm.getBTorre()), glm::vec3(3.5f, -3.5f, -0.2f)); // H1
	placePiece(createObject(mm.getBCavall()), glm::vec3(2.5f, -3.5f, -0.2f), true);
	placePiece(createObject(mm.getBAlfil()), glm::vec3(1.5f, -3.5f, -0.2f));
	placePiece(createObject(mm.getBReina()), glm::vec3(0.5f, -3.5f, -0.2f));
	placePiece(createObject(mm.getBRei()), glm::vec3(-0.5f, -3.5f, -0.2f));
	placePiece(createObject(mm.getBAlfil()), glm::vec3(-1.5f, -3.5f, -0.2f));
	//placePiece(createObject(mm.getBCavall()), glm::vec3(-2.5f, -3.5f, -0.15f), true);
	placePiece(createObject(mm.getBTorre()), glm::vec3(-3.5f, -3.5f, -0.2f)); //A1

	//placePiece(createObject(mm.getBPeo()), glm::vec3(-3.5f, -2.5f, -0.2f));
	placePiece(createObject(mm.getBPeo()), glm::vec3(-2.5f, -2.5f, -0.2f));
	placePiece(createObject(mm.getBPeo()), glm::vec3(-1.5f, -2.5f, -0.2f));
	placePiece(createObject(mm.getBPeo()), glm::vec3(-0.5f, -2.5f, -0.2f));
	placePiece(createObject(mm.getBPeo()), glm::vec3(0.5f, -2.5f, -0.2f));
	placePiece(createObject(mm.getBPeo()), glm::vec3(1.5f, -2.5f, -0.2f));
	placePiece(createObject(mm.getBPeo()), glm::vec3(2.5f, -2.5f, -0.2f));
	placePiece(createObject(mm.getBPeo()), glm::vec3(3.5f, -2.5f, -0.2f));




	//-------------------------------------------------------------------------------------------------------------------------------------------------------NEGRES

	placePiece(createObject(mm.getNTorre()), glm::vec3(3.5f, 3.5f, -0.2f));
	placePiece(createObject(mm.getNCavall()), glm::vec3(2.5f, 3.5f, -0.2f));
	placePiece(createObject(mm.getNAlfil()), glm::vec3(1.5f, 3.5f, -0.2f));
	placePiece(createObject(mm.getNReina()), glm::vec3(0.5f, 3.5f, -0.2f));
	placePiece(createObject(mm.getNRei()), glm::vec3(-0.5f, 3.5f, -0.2f));
	placePiece(createObject(mm.getNAlfil()), glm::vec3(-1.5f, 3.5f, -0.2f));
	placePiece(createObject(mm.getNCavall()), glm::vec3(-2.5f, 3.5f, -0.15f));
	placePiece(createObject(mm.getNTorre()), glm::vec3(-3.5f, 3.5f, -0.2f));

	placePiece(createObject(mm.getNPeo()), glm::vec3(-3.5f, 2.5f, -0.2f));
	placePiece(createObject(mm.getNPeo()), glm::vec3(-2.5f, 2.5f, -0.2f));
	placePiece(createObject(mm.getNPeo()), glm::vec3(-1.5f, 2.5f, -0.2f));
	placePiece(createObject(mm.getNPeo()), glm::vec3(-0.5f, 2.5f, -0.2f));
	placePiece(createObject(mm.getNPeo()), glm::vec3(0.5f, 2.5f, -0.2f));
	placePiece(createObject(mm.getNPeo()), glm::vec3(1.5f, 2.5f, -0.2f));
	placePiece(createObject(mm.getNPeo()), glm::vec3(2.5f, 2.5f, -0.2f));
	placePiece(createObject(mm.getNPeo()), glm::vec3(3.5f, 2.5f, -0.2f));

	*/
	




	

	//Carga player
	player = &Player::GetInstance();
	pl = player;

	//Carga camaras
	nCamaras = 7;
	camaras = std::vector<Camara>(nCamaras, Camara(w,h));

	camaraActual = CAM_INICIAL;
	mainCamara = &camaras[CAM_INICIAL];

	camaras[CAM_TOP].translate(glm::vec3(-2, 1.51f, 30));
	camaras[CAM_TOP].target(glm::vec3(-2, 1.5f, 0));

	camaras[CAM_FRONT].translate(glm::vec3(-22, 0, 6));
	camaras[CAM_FRONT].target(glm::vec3(0, 0, 0));

	camaras[CAM_RIGHT].translate(glm::vec3(0, -20, 20));
	camaras[CAM_RIGHT].target(glm::vec3(0, 0, 0));

	camaras[CAM_INICIAL].translate(glm::vec3(0, 0, 15));
	camaras[CAM_INICIAL].target(glm::vec3(0, 0, 0));

	freeCameraPos = glm::vec3(0, 0, 0);

	cameraSpeed = 10;
	distancia = 25;
	yawCamera = -135;
	pitchCamera = 45; 
	sensibilidad = 0.5;

	//Carga iluminación
	luz.InitIluminacion(w, h);
	direccionSol = glm::vec3(-0.9f, -3.0f, 2.0f);
	ambientIntensity = 0.3;
	lightColor = glm::vec3(1, 0.86f, 0.77f);

	boxSize = 25;
	near_plane = 1.0f;
	far_plane = 100.0f;

	luz.m_cam = &mainCamara;
	luz.objetos = &objects;
	luz.enemigos = &enemies;
	luz.turrets = turrets;

	//Carga PO
	po = PickingObjects3D(w, h, poShaderID);
	po.m_cam = &mainCamara;
	po.m_turrets = turrets;
	po.m_objectes = &objects;
	luz.m_po = &po;
	
	//Carga variables juego
	

	initVAOList();	// Inicialtzar llista de VAO'S.
}

void InitAPI()
{
// Vendor, Renderer, Version, Shading Laguage Version i Extensions suportades per la placa gr�fica gravades en fitxer extensions.txt
	std::string nomf = "extensions.txt";
	char const* nomExt = "";
	const char* nomfitxer;
	nomfitxer = nomf.c_str();	// Conversi� tipus string --> char *
	int num_Ext;

	char* str = (char*)glGetString(GL_VENDOR);
	FILE* f = fopen(nomfitxer, "w");
	if(f)	{	fprintf(f,"VENDOR: %s\n",str);
				fprintf(stderr, "VENDOR: %s\n", str);
				str = (char*)glGetString(GL_RENDERER);
				fprintf(f, "RENDERER: %s\n", str);
				fprintf(stderr, "RENDERER: %s\n", str);
				str = (char*)glGetString(GL_VERSION);
				fprintf(f, "VERSION: %s\n", str);
				fprintf(stderr, "VERSION: %s\n", str);
				str = (char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
				fprintf(f, "SHADING_LANGUAGE_VERSION: %s\n", str);
				fprintf(stderr, "SHADING_LANGUAGE_VERSION: %s\n", str);
				glGetIntegerv(GL_NUM_EXTENSIONS, &num_Ext);
				fprintf(f, "EXTENSIONS: \n");
				fprintf(stderr, "EXTENSIONS: \n");
				for (int i = 0; i < num_Ext; i++)
				{	nomExt = (char const*)glGetStringi(GL_EXTENSIONS, i);
					fprintf(f, "%s \n", nomExt);
					//fprintf(stderr, "%s", nomExt);	// Displaiar extensions per pantalla
				}
				//fprintf(stderr, "\n");				// Displaiar <cr> per pantalla despr�s extensions
//				str = (char*)glGetString(GL_EXTENSIONS);
//				fprintf(f, "EXTENSIONS: %s\n", str);
				//fprintf(stderr, "EXTENSIONS: %s\n", str);
				fclose(f);
			}

// Program
	glCreateProgram = (PFNGLCREATEPROGRAMPROC)wglGetProcAddress("glCreateProgram");
	glDeleteProgram = (PFNGLDELETEPROGRAMPROC)wglGetProcAddress("glDeleteProgram");
	glUseProgram = (PFNGLUSEPROGRAMPROC)wglGetProcAddress("glUseProgram");
	glAttachShader = (PFNGLATTACHSHADERPROC)wglGetProcAddress("glAttachShader");
	glDetachShader = (PFNGLDETACHSHADERPROC)wglGetProcAddress("glDetachShader");
	glLinkProgram = (PFNGLLINKPROGRAMPROC)wglGetProcAddress("glLinkProgram");
	glGetProgramiv = (PFNGLGETPROGRAMIVPROC)wglGetProcAddress("glGetProgramiv");
	glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)wglGetProcAddress("glGetShaderInfoLog");
	glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation");
	glUniform1i = (PFNGLUNIFORM1IPROC)wglGetProcAddress("glUniform1i");
	glUniform1iv = (PFNGLUNIFORM1IVPROC)wglGetProcAddress("glUniform1iv");
	glUniform2iv = (PFNGLUNIFORM2IVPROC)wglGetProcAddress("glUniform2iv");
	glUniform3iv = (PFNGLUNIFORM3IVPROC)wglGetProcAddress("glUniform3iv");
	glUniform4iv = (PFNGLUNIFORM4IVPROC)wglGetProcAddress("glUniform4iv");
	glUniform1f = (PFNGLUNIFORM1FPROC)wglGetProcAddress("glUniform1f");
	glUniform1fv = (PFNGLUNIFORM1FVPROC)wglGetProcAddress("glUniform1fv");
	glUniform2fv = (PFNGLUNIFORM2FVPROC)wglGetProcAddress("glUniform2fv");
	glUniform3fv = (PFNGLUNIFORM3FVPROC)wglGetProcAddress("glUniform3fv");
	glUniform4fv = (PFNGLUNIFORM4FVPROC)wglGetProcAddress("glUniform4fv");
	glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)wglGetProcAddress("glUniformMatrix4fv");
	glGetAttribLocation = (PFNGLGETATTRIBLOCATIONPROC)wglGetProcAddress("glGetAttribLocation");
	glVertexAttrib1f = (PFNGLVERTEXATTRIB1FPROC)wglGetProcAddress("glVertexAttrib1f");
	glVertexAttrib1fv = (PFNGLVERTEXATTRIB1FVPROC)wglGetProcAddress("glVertexAttrib1fv");
	glVertexAttrib2fv = (PFNGLVERTEXATTRIB2FVPROC)wglGetProcAddress("glVertexAttrib2fv");
	glVertexAttrib3fv = (PFNGLVERTEXATTRIB3FVPROC)wglGetProcAddress("glVertexAttrib3fv");
	glVertexAttrib4fv = (PFNGLVERTEXATTRIB4FVPROC)wglGetProcAddress("glVertexAttrib4fv");
	glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress("glEnableVertexAttribArray");
	glDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress("glDisableVertexAttribArray");
	glBindAttribLocation = (PFNGLBINDATTRIBLOCATIONPROC)wglGetProcAddress("glBindAttribLocation");
	glGetActiveUniform = (PFNGLGETACTIVEUNIFORMPROC)wglGetProcAddress("glGetActiveUniform");

// Shader
	glCreateShader = (PFNGLCREATESHADERPROC)wglGetProcAddress("glCreateShader");
	glDeleteShader = (PFNGLDELETESHADERPROC)wglGetProcAddress("glDeleteShader");
	glShaderSource = (PFNGLSHADERSOURCEPROC)wglGetProcAddress("glShaderSource");
	glCompileShader = (PFNGLCOMPILESHADERPROC)wglGetProcAddress("glCompileShader");
	glGetShaderiv = (PFNGLGETSHADERIVPROC)wglGetProcAddress("glGetShaderiv");

// VBO
	glGenBuffers = (PFNGLGENBUFFERSPROC)wglGetProcAddress("glGenBuffers");
	glBindBuffer = (PFNGLBINDBUFFERPROC)wglGetProcAddress("glBindBuffer");
	glBufferData = (PFNGLBUFFERDATAPROC)wglGetProcAddress("glBufferData");
	glBufferSubData = (PFNGLBUFFERSUBDATAPROC)wglGetProcAddress("glBufferSubData");
	glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)wglGetProcAddress("glDeleteBuffers");
	glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)wglGetProcAddress("glVertexAttribPointer");
}

/*
void finishRound()
{
	enConstruccion = true;
	fprintf(stderr, "Round Finished:\n");
	player->increaseRound();
}
*/

//Enemy* spawnEnemy(int type)

//void destroyEnemies(Enemy* en)
/*
GameObject* createObject(COBJModel* model)
{
	GameObject* newObject = new GameObject(model);
	newObject->setId(objects.size());
	

	objects.push_back(newObject);
	return newObject;
}
*/
//Path* createPath(glm::vec2 pos, float speedMultiplier = 1.0f, int tipo = 0)



//void setUpPath() 

void destroyObject(GameObject* obj)
{
	int id = obj->getId();
	int vectorSize = objects.size();

	if (id < vectorSize)
	{
		GameObject* lastObject = objects.back();
		if (id != vectorSize - 1)
			lastObject->setId(id);

		std::swap(objects[id], objects.back());
		objects.pop_back();
		delete obj;
	}
}

//int getTurretPrice(int type)

//void modifyTurret(int id, int type)

/*void setUpTurrets()
{
	glm::vec2 pos[NTURRETS];

	//Posicions torres

	pos[0] = glm::vec2(3.25f, -2.1f);
	pos[1] = glm::vec2(-1.2f, 2.35f);
	pos[2] = glm::vec2(6.15f, 6.75f);
	pos[3] = glm::vec2(1.85f, 11.125f);
	pos[4] = glm::vec2(-5.45f, 6.45f);
	pos[5] = glm::vec2(-6.2f, -9.1f);
	pos[6] = glm::vec2(-13.0f, -4.75f);
	pos[7] = glm::vec2(-9.9f, -0.6f);
	pos[8] = glm::vec2(-15.5f, 3.9f);


	for (int i = 0; i < NTURRETS; i++)
	{
		turrets[i] = new Turret(i+1);
		turrets[i]->setPos(pos[i]);
		turrets[i]->setEnemiesVector(&enemies);
	}
}*/

//glm::vec3 getTurretUpgradesPrices(int id) 

//void buyTurret(int id, int type) 

//void buyTurretUpgrade(int id, int stat) 
/*
void resetGame() 
{
	enConstruccion = true;
	debug_speedMult = 1.0f;
	//Enemies
	for (Enemy* en : enemies)
		delete en;

	enemies.clear();
	enemyCount = 0;
	enemySpawnTimer = 0;
	currentWeight = 0;
	maxEnemy = 0;

	//Turrets
	for (int i = 0; i < NTURRETS; i++) 
		modifyTurret(i, -1);

	turretAmount = 0;

	for (int i = 0; i < 5; i++)
		turretAmountByType[i] = 0;

	//Money & stats
	player->resetPlayer();

	//Camaras
	camaraActual = 0;
	freeCameraPos = glm::vec3(0, 0, 0);
	distancia = 25;
	yawCamera = -135;
	pitchCamera = 45;

	float yawRadiants = glm::radians(yawCamera);
	float pitchRadiants = glm::radians(pitchCamera);

	glm::vec3 forward;

	forward.x = glm::cos(yawRadiants) * glm::cos(pitchRadiants);
	forward.y = glm::sin(yawRadiants) * glm::cos(pitchRadiants);
	forward.z = glm::sin(pitchRadiants);

	forward = glm::normalize(forward);
	cameraDir = forward;
	cameraRight = glm::normalize(glm::cross(forward, glm::vec3(0, 0, 1)));

	freeCameraPos = forward * distancia;


	//Menus
	juego_pausado = false;
	show_menu_construccion = false;
	show_menu_mejora = false;
	show_menu_muerte = false;
	enConstruccion = true;
}
*/

void OnSize(GLFWwindow* window, int width, int height)
{
	w = width;	h = height;
	luz.UpdateWindow(width, height);
	for (Camara& c : camaras)
	{
		c.UpdateWindow(w, h);
	}
	mainCamara->UpdateWindow(width, height);
	po.updatePickingObjectSize(width, height);
}

// Skybox
void OnVistaSkyBox()
{
// C�rrega Shader Skybox
	if (!skC_programID) skC_programID = shader_SkyBoxC.loadFileShaders(".\\shaders\\skybox.VERT", ".\\shaders\\skybox.FRAG");

// C�rrega VAO Skybox Cube
	if (skC_VAOID.vaoId == 0) skC_VAOID = loadCubeSkybox_VAO();
	Set_VAOList(CUBE_SKYBOX, skC_VAOID);

	if (!cubemapTexture)
	{	// load Skybox textures
		// -------------
		std::vector<std::string> faces =
		{ ".\\textures\\skybox\\right.jpg",
			".\\textures\\skybox\\left.jpg",
			".\\textures\\skybox\\top.jpg",
			".\\textures\\skybox\\bottom.jpg",
			".\\textures\\skybox\\front.jpg",
			".\\textures\\skybox\\back.jpg"
		};
		cubemapTexture = loadCubemap(faces);
	}
}

//void spawnEnemies(float deltaTime)

//void startSpawningEnemies() 

//void startNextRound()



/* ------------------------------------------------------------------------- */
/*                           CONTROL DEL RATOLI                              */
/* ------------------------------------------------------------------------- */

void OnMouseButton(GLFWwindow* window, int button, int action, int mods)
{
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	ImGuiIO& io = ImGui::GetIO();
	io.AddMouseButtonEvent(button, action);

	if (!io.WantCaptureMouse) {
		// OnLButtonDown
		if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
		{
			m_ButoEAvall = true;
			m_PosEAvall.x = xpos;	m_PosEAvall.y = ypos;
			m_EsfeEAvall = OPV;
		}
		// OnLButtonUp
		else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
			m_ButoEAvall = false;
		// OnRButtonDown
		else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		{
			m_ButoDAvall = true;
			m_PosDAvall.x = xpos;	m_PosDAvall.y = ypos;

			po.renderPicking();
			clickedObject = po.getClickedObject(window, m_PosDAvall);

		}
		// OnRButtonUp
		else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
			m_ButoDAvall = false;
	}
}

void OnScroll(GLFWwindow* window, double xoffset, double yoffset)
{
	distancia -= yoffset;
	if (distancia < 2) { distancia = 2; }
	if (distancia > 50) { distancia = 50; }
}

void OnMouseMove(GLFWwindow* window, double xpos, double ypos)
{
	if (m_ButoEAvall)
	{
		double deltaX = xpos - m_PosEAvall.x;
		double deltaY = ypos - m_PosEAvall.y;

		
		yawCamera += (float)deltaX * sensibilidad * ((camaraActual == CAM_FREE) ? -1 : 1);
		pitchCamera -= (float)deltaY * sensibilidad * ((camaraActual == CAM_FREE) ? -1 : 1);

		m_PosEAvall.x = xpos;
		m_PosEAvall.y = ypos;

		if (camaraActual != CAM_FREE)
		{
			if (pitchCamera > 89.0f)  pitchCamera = 89.0f;
			if (pitchCamera < 15) pitchCamera = 15;
		}
		else
		{
			if (pitchCamera > 89.0f)  pitchCamera = 89.0f;
			if (pitchCamera < -89.0f) pitchCamera = -89.0f;
		}

		while (yawCamera >= 360.0f) yawCamera -= 360.0f;
		while (yawCamera < 0.0f)    yawCamera += 360.0f;
	}

	float yawRadiants = glm::radians(yawCamera);
	float pitchRadiants = glm::radians(pitchCamera);

	glm::vec3 forward;

	forward.x = glm::cos(yawRadiants) * glm::cos(pitchRadiants);
	forward.y = glm::sin(yawRadiants) * glm::cos(pitchRadiants);
	forward.z = glm::sin(pitchRadiants);

	forward = glm::normalize(forward);
	cameraDir = forward;
	cameraRight = glm::normalize(glm::cross(forward, glm::vec3(0, 0, 1)));
	
	if (camaraActual != CAM_FREE)
	{
		freeCameraPos = forward * distancia;
	}
}

void OnKeyDown(GLFWwindow* window, int key, int scancode, int action, int mods) {

	ImGuiIO& io = ImGui::GetIO();
	if (io.WantCaptureKeyboard)
		return;

	if (action == GLFW_PRESS)
	{
		switch (key)
		{
		case GLFW_KEY_F:
			OnFull_Screen(primary, window);
			break;

		case GLFW_KEY_X:
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			camaraActual++;
			break;
		}

		case GLFW_KEY_V:
			processVoiceCommand("A2 A3", taulell);
			break;

		/*case GLFW_KEY_P:
		{
			glm::vec3 current = peoA2->getPos();
			movePieceTo(peoA2, current + glm::vec3(0, 1.0f, 0));
			break;
		}

		case GLFW_KEY_O:
		{
			glm::vec3 current = peoA2->getPos();
			movePieceTo(peoA2, current + glm::vec3(0, 2.0f, 0));
			break;
		}

		case GLFW_KEY_I:
		{
			glm::vec3 current = peoA2->getPos();
			movePieceTo(peoA2, current + glm::vec3(1.0f, 1.0f, 0));
			break;
		}

		case GLFW_KEY_U: // diagonal esquerra
		{
			glm::vec3 current = peoA2->getPos();
			movePieceTo(peoA2, current + glm::vec3(-1.0f, 1.0f, 0));
			break;
		}


		case GLFW_KEY_K:
		{
			//moveKnight(cavallB1, glm::vec3(1.0f, 2.0f, 0.0f));
			glm::vec3 current = cavallB1->getPos();
			movePieceTo(cavallB1, current + glm::vec3(1.0f, 2.0f, 0.0f));
			break;
		}
		*/

		default:
			break;
		}

		camaraActual = (camaraActual % nCamaras + nCamaras) % nCamaras;

		mainCamara = &camaras[camaraActual];

		/*
		if (key == GLFW_KEY_P)
		{
			glm::vec3 current = peoA2->getPos();
			movePieceTo(peoA2, current + glm::vec3(0, 1.0f, 0));
		}

		if (key == GLFW_KEY_O)
		{
			glm::vec3 current = peoA2->getPos();
			movePieceTo(peoA2, current + glm::vec3(0, 2.0f, 0));
		}
		*/

		if (key == GLFW_KEY_W)
		{
			inputVector += glm::vec3(0, 1, 0);
		}
		if (key == GLFW_KEY_A)
		{
			inputVector += glm::vec3(-1, 0, 0);
		}
		if (key == GLFW_KEY_S)
		{
			inputVector += glm::vec3(0, -1, 0);
		}
		if (key == GLFW_KEY_D)
		{
			inputVector += glm::vec3(1, 0, 0);
		}
	}


	if (action == GLFW_RELEASE)
	{
		if (key == GLFW_KEY_W)
		{
			inputVector += glm::vec3(0, -1, 0);
		}
		if (key == GLFW_KEY_A)
		{
			inputVector += glm::vec3(1, 0, 0);
		}
		if (key == GLFW_KEY_S)
		{
			inputVector += glm::vec3(0, 1, 0);
		}
		if (key == GLFW_KEY_D)
		{
			inputVector += glm::vec3(-1, 0, 0);
		}
	}

	inputVector = glm::vec3(0);
	if (keyW) inputVector += glm::vec3(0, 1, 0);
	if (keyS) inputVector += glm::vec3(0, -1, 0);
	if (keyA) inputVector += glm::vec3(-1, 0, 0);
	if (keyD) inputVector += glm::vec3(1, 0, 0);
}

// Entorn VGI. OnFull_Screen: Funci� per a pantalla completa
void OnFull_Screen(GLFWmonitor* monitor, GLFWwindow *window)
{
	fullscreen = !fullscreen;
	if (fullscreen) 
	{	// backup window position and window size
		const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
		w = mode->width;	h = mode->height;
		glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
	}
	else // Restore last window size and position
		glfwSetWindowMonitor(window, nullptr, 216, 239, 640, 480, mode->refreshRate);
}

// -------------------- TRACTAMENT ERRORS NO TOCAR
// error_callback: Displaia error que es pugui produir
void error_callback(int code, const char* description)
{
    //const char* descripcio;
    //int codi = glfwGetError(&descripcio);

 //   display_error_message(code, description);
	fprintf(stderr, "Codi Error: %i", code);
	fprintf(stderr, "Descripcio: %s\n",description);
}

GLenum glCheckError_(const char* file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
		case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		fprintf(stderr, "ERROR %s | File: %s | Line ( %3i ) \n", error.c_str(), file, line);
		//fprintf(stderr, "ERROR %s | ", error.c_str());
		//fprintf(stderr, "File: %s | ", file);
		//fprintf(stderr, "Line ( %3i ) \n", line);
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void APIENTRY glDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
	const GLchar* message, const void* userParam)
{
	if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return; // ignore these non-significant error codes

	fprintf(stderr, "---------------\n");
	fprintf(stderr, "Debug message ( %3i %s \n", id, message);

	switch (source)
	{
	case GL_DEBUG_SOURCE_API:             fprintf(stderr, "Source: API \n"); break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   fprintf(stderr, "Source: Window System \n"); break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER: fprintf(stderr, "Source: Shader Compiler \n"); break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:     fprintf(stderr, "Source: Third Party \n"); break;
	case GL_DEBUG_SOURCE_APPLICATION:     fprintf(stderr, "Source: Application \n"); break;
	case GL_DEBUG_SOURCE_OTHER:           fprintf(stderr, "Source: Other \n"); break;
	} //std::cout << std::endl;

	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR:               fprintf(stderr, "Type: Error\n"); break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: fprintf(stderr, "Type: Deprecated Behaviour\n"); break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  fprintf(stderr, "Type: Undefined Behaviour\n"); break;
	case GL_DEBUG_TYPE_PORTABILITY:         fprintf(stderr, "Type: Portability\n"); break;
	case GL_DEBUG_TYPE_PERFORMANCE:         fprintf(stderr, "Type: Performance\n"); break;
	case GL_DEBUG_TYPE_MARKER:              fprintf(stderr, "Type: Marker\n"); break;
	case GL_DEBUG_TYPE_PUSH_GROUP:          fprintf(stderr, "Type: Push Group\n"); break;
	case GL_DEBUG_TYPE_POP_GROUP:           fprintf(stderr, "Type: Pop Group\n"); break;
	case GL_DEBUG_TYPE_OTHER:               fprintf(stderr, "Type: Other\n"); break;
	} //std::cout << std::endl;

	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:         fprintf(stderr, "Severity: high\n"); break;
	case GL_DEBUG_SEVERITY_MEDIUM:       fprintf(stderr, "Severity: medium\n"); break;
	case GL_DEBUG_SEVERITY_LOW:          fprintf(stderr, "Severity: low\n"); break;
	case GL_DEBUG_SEVERITY_NOTIFICATION: fprintf(stderr, "Severity: notification\n"); break;
	} //std::cout << std::endl;
	//std::cout << std::endl;
	fprintf(stderr, "\n");
}

void CamerasUpdate() 
{
	float maxProg = 0;

	camaras[CAM_FOLLOW] = camaras[CAM_ORBIT];


	if (inputVector.y == 1)	freeCameraPos -= cameraDir * cameraSpeed * deltaTime;
	if (inputVector.x == -1) freeCameraPos += cameraRight * cameraSpeed * deltaTime;
	if (inputVector.y == -1) freeCameraPos += cameraDir * cameraSpeed * deltaTime;
	if (inputVector.x == 1) freeCameraPos -= cameraRight * cameraSpeed * deltaTime;

	camaras[CAM_FREE].translate(freeCameraPos);
	camaras[CAM_FREE].target(freeCameraPos - cameraDir);

	//fprintf(stderr, "Pos: %f,%f,%f\n", freeCameraPos.x, freeCameraPos.y, freeCameraPos.z);


	camaras[CAM_ORBIT].translate(cameraDir * distancia);
	camaras[CAM_ORBIT].target(glm::vec3(0, 0, 0));

	mainCamara = &camaras[camaraActual];

}

//-----------------Variables globales
/*
void Update(float timer, float deltaTime) 
{
	spawnEnemies(deltaTime);

	for (Enemy* e : enemies) 
	{
		e->move(deltaTime, timer);
		divideEnemy(e);
		destroyEnemies(e);
	}

	for (int i = 0; i < NTURRETS; i++) 
	{
		turrets[i]->mainUpdate(deltaTime);
	}
}
*/

int main(void)
{
	//    GLFWwindow* window;
	// Entorn VGI. Timer: Variables
	float time = elapsedTime;
	float now;
	float delta;


	//VOSK
	model = vosk_model_new(
		"C:\\Users\\uanra\\Desktop\\VGI GRUP altre joc\\Tower-Defense-master\\x64\\Release\\model"
	);

	if (!model) {
		std::cout << "Error carregant model" << std::endl;
		return -1;
	}

	rec = vosk_recognizer_new(model, 16000.0);

	std::cout << "Vosk funciona!" << std::endl;


	//PORT AUDIO
	Pa_Initialize();
	PaStream* stream;
	Pa_OpenDefaultStream(
		&stream,
		1,          // input channels
		0,          // output channels
		paInt16,
		16000,
		256,
		nullptr,
		nullptr
	);

	Pa_StartStream(stream);



	// glfw: initialize and configure
	// ------------------------------
	if (!glfwInit())
	{	fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}

	// Retrieving main monitor
    primary = glfwGetPrimaryMonitor();

	// To get current video mode of a monitor
    mode = glfwGetVideoMode(primary);

    int countVM;
    const GLFWvidmode* modes = glfwGetVideoModes(primary, &countVM);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

	// Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Entorn Grafic VS2022 amb GLFW i OpenGL 4.6 (Visualitzacio Grafica Interactiva - Grau en Enginyeria Informatica - Escola Enginyeria - UAB)", NULL, NULL);
    if (!window)
    {	fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 4.6 compatible. Try the 2.1 version of the tutorials.\n");
		getchar();
		glfwTerminate();
        return -1;
    }

	// Make the window's context current
    glfwMakeContextCurrent(window);

	// Llegir resoluci� actual de pantalla
	glfwGetWindowSize(window, &width_old, &height_old);

	// Initialize GLEW
	if (GLEW_VERSION_3_3) glewExperimental = GL_TRUE; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		glGetError();	// Esborrar codi error generat per bug a llibreria GLEW
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Initialize Application control varibles
	InitGL();


	glEnable(GL_FRAMEBUFFER_SRGB);


	//cargarModelos();

	// ------------- Entorn VGI: Callbacks
	// Set GLFW event callbacks. I removed glfwSetWindowSizeCallback for conciseness
	glfwSetWindowSizeCallback(window, OnSize);										// - Windows Size in screen Coordinates
	glfwSetFramebufferSizeCallback(window, OnSize);									// - Windows Size in Pixel Coordinates
	glfwSetMouseButtonCallback(window, (GLFWmousebuttonfun)OnMouseButton);			// - Directly redirect GLFW mouse button events
	glfwSetCursorPosCallback(window, (GLFWcursorposfun)OnMouseMove);				// - Directly redirect GLFW mouse position events
	glfwSetScrollCallback(window, OnScroll);
	glfwSetKeyCallback(window, OnKeyDown);										// - Directly redirect GLFW key events
	//glfwSetCharCallback(window, OnTextDown);										// - Directly redirect GLFW char events
	glfwSetErrorCallback(error_callback);											// Error callback

	// Entorn VGI; Timer: Lectura temps
	float previous = glfwGetTime();

	// Entorn VGI.ImGui: Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;


	// Entorn VGI.ImGui: Setup Dear ImGui style
	//ImGui::StyleColorsDark();
	ImGui::StyleColorsLight();

	// Entorn VGI.ImGui: Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 130");
	// Entorn VGI.ImGui: End Setup Dear ImGui context

	//Dibuixa els eixos
	if (eixos) 
	{
		if (!eixos_programID) eixos_programID = shaderEixos.loadFileShaders(".\\shaders\\eixos.VERT", ".\\shaders\\eixos.FRAG");
		if (!eixos_Id) eixos_Id = deixos(); // Funci� que defineix els Eixos Coordenades M�n com un VAO.
	}
	if (SkyBoxCube) OnVistaSkyBox();
	if (fullscreen)
	{
		const GLFWvidmode* currentMode = glfwGetVideoMode(primary);
		if (currentMode) {
			// Store original window size before going fullscreen
			w_old = w;
			h_old = h;
			glfwSetWindowMonitor(window, primary, 0, 0, currentMode->width, currentMode->height, currentMode->refreshRate);
			w = currentMode->width;
			h = currentMode->height;
			glViewport(0, 0, w, h);
		}
		else {
			fprintf(stderr, "Failed to get video mode for primary monitor.\n");
			fullscreen = false;
		}
	}

	//setUpPath();
	//setUpTurrets();
	luz.TurretsWereLoaded();
	/*modifyTurret(0, -1);
	modifyTurret(1, -1);
	modifyTurret(2, -1);
	modifyTurret(3, -1);
	modifyTurret(4, -1);
	modifyTurret(5, -1);
	modifyTurret(6, -1);
	modifyTurret(7, -1);
	modifyTurret(8, -1);*/

	glEnable(GL_DEPTH_TEST);
	bool salir = false;
















	//Inicailiza las imagenes usadas
	InicializarGestorImagenes();
    while (!glfwWindowShouldClose(window) and !salir)
    {  
		//AUDIO
		int16_t buffer[512];

		Pa_ReadStream(stream, buffer, 512);

		bool final = vosk_recognizer_accept_waveform(rec, (const char*)buffer, sizeof(buffer));

		const char* result = nullptr;
		const char* partial = nullptr;

		if (final)
		{
			const char* result = vosk_recognizer_result(rec);
			std::string json(result);

			std::cout << "VOSK FINAL: " << json << std::endl;

			size_t pos = json.find("\"text\"");
			if (pos != std::string::npos)
			{
				size_t start = json.find("\"", pos + 6) + 1;
				size_t end = json.find("\"", start);

				std::string command = json.substr(start, end - start);

				if (!command.empty())
				{
					command = normalize(command);
					processVoiceCommand(command, taulell);
				}
			}
		}
		else
		{
			const char* partial = vosk_recognizer_partial_result(rec);
			std::cout << "PARTIAL: " << partial << std::endl;
		}
		

		
		if (result)
		{
			std::string json(result);
			std::cout << "VOSK: " << json << std::endl;

			size_t pos = json.find("\"partial\"");

			if (pos != std::string::npos)
			{
				size_t start = json.find("\"", pos + 9);
				if (start != std::string::npos)
				{
					start++;
					size_t end = json.find("\"", start);

					if (end != std::string::npos)
					{
						std::string command = json.substr(start, end - start);

						if (!command.empty())
						{
							std::cout << "COMMAND: " << command << std::endl;
							command = normalize(command);
							processVoiceCommand(command, taulell);
						}
					}
				}
			}
		}

	
		// Poll for and process events
		glfwPollEvents();

		now = glfwGetTime();
		delta = now - previous;
		previous = now;
		deltaTime = delta;

		if (delta > 0.15)
		{
			deltaTime = 0.15;
		}
		else
		{
			deltaTime = delta; 
		}



		// Draws the UI
		menu(salir);


		


		// CONTROL DE ACTUALIZACIÓN (UPDATE)
		// Se actualiza si:
		// 1. NO está en pausa global (ESC)
		// 2. Y ADEMÁS: (Estamos jugando) O (Estamos en pruebas Y el timer no está detenido manualmente)
		bool debeActualizar = show_jugar;

		if (debeActualizar)
		{
			//Update(frameTimer, deltaTime * debug_speedMult);
			frameTimer += deltaTime;
			if (camaraActual == CAM_INICIAL)
			{
				camaraActual = CAM_ORBIT;

			}
		}

	// MOURE OBJECTE AMB FLUIDESA __________________________________________________________________________________________________________________________________________________________
		if (moving && movingObject != nullptr)
		{
			moveProgress += deltaTime * moveSpeed;

			if (moveProgress >= 1.0f)
			{
				moveProgress = 1.0f;
				moving = false;
			}

			glm::vec3 newPos = glm::mix(startPos, targetPos, moveProgress);
			movingObject->translate(newPos); // ✅ ara mou el correcte
		}
		


		CamerasUpdate();
		if (debug_renderMode == PICKING_OBJECTS)
		{
			po.renderPicking();
		}
		
		//fprintf(stderr, "Cam: %d\n", camaraActual);


		// CONSTRUIMOS EL VECTOR DE LUZ DESDE LOS SLIDERS
		glm::vec3 lightDir(debug_lightDir[0], debug_lightDir[1], debug_lightDir[2]);

		// LLAMADA MODIFICADA RENDER SHADOWS
		luz.RenderShadows(lightDir, debug_boxSize, debug_nearPlane, debug_farPlane);

		glm::vec3 lightColor(debug_lightColor[0], debug_lightColor[1], debug_lightColor[2]);

		// LLAMADA MODIFICADA RENDER GAME
		luz.RenderGame(customShaderID, debug_ambientIntensity, lightColor, debug_renderMode);



		dibuixa_Skybox(skC_programID, cubemapTexture, Vis_Polar, mainCamara->getProjection(), mainCamara->getView());
		//dibuixa_Eixos(eixos_programID, eixos, eixos_Id, grid, hgrid, mainCamara.getProjection(), mainCamara.getView());

		

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		// Entorn VGI: Activa la finestra actual
		glfwMakeContextCurrent(window);

		// Entorn VGI: Transfer�ncia del buffer OpenGL a buffer de pantalla
		glfwSwapBuffers(window);

    }

	// Check if the ESC key was pressed or the window was closed o boton salir presionado
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0 && !salir);

	// Delete all the objects in the scene
	for (GameObject* obj : objects)
		DeleteObject(obj);

	/*
	for (Enemy* en : enemies) 
		destroyEnemies(en);
	for (Turret* tu : turrets)
		delete tu;			
		*/


	// Entorn VGI.ImGui: Cleanup ImGui
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);


	// Terminating GLFW: Destroy the windows, monitors and cursor objects
    glfwTerminate();

	if (shaderLighting.getProgramID() != -1) shaderLighting.DeleteProgram();
	if (shaderSkyBox.getProgramID() != -1) shaderSkyBox.DeleteProgram();
    return 0;
}


