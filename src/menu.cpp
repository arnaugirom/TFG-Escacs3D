#include "ImGui\imgui.h"
#include "ImGui\imgui_internal.h"
#include "ImGui\imgui_impl_glfw.h"
#include "ImGui\imgui_impl_opengl3.h"
#include "ImGui\nfd.h" // Native File Dialog

#include "stdafx.h"
#include "shader.h"
#include "visualitzacio.h"
#include "escena.h"
#include "menu.h"
#include "player.h"


// ---------------------------------------------------------
// DECLARACIÓN EXTERNA (Conecta con main.cpp)
// ---------------------------------------------------------
//extern int getTurretPrice(int type);




//------------- Variables Globales -------------
bool show_menu_inicio = true;
bool show_jugar = false;

bool debugMode = false;
bool enterDebugMode = false;


Player* pl = nullptr;


bool show_submenu_light = false;
bool show_submenu_shadows = false;

//------------- Variables para Sombras y Render -----------------------
float debug_lightDir[3] = { -0.9f, -3.0f,2.0f };
float debug_boxSize = 25.0f;
float debug_nearPlane = 1.0f;
float debug_farPlane = 100.0f;
float debug_ambientIntensity = 0.3f;
float debug_lightColor[3] = { 1.0f, 0.86f, 0.77f };
int debug_renderMode = 0; // 0 = DEFAULT

// Variable de Brillo (1.0 = Normal, 0.0 = Negro total)
float nivelBrillo = 1.0f;

// ================ FUNCIONES IMAGENES ================

#include "stb_image.h" 

ImagenData imgVida;
ImagenData imgDinero;
ImagenData imgRonda;
ImagenData imgTorretas[5];
ImagenData imgTorretasMejora[5];
ImagenData imgMuerte;
ImagenData imgTitulo;

// --- FUNCION  DE CARGA ---
bool CargarTexturaInterna(const char* filename, ImagenData& out_img)
{
	int w, h, channels;
	unsigned char* data = stbi_load(filename, &w, &h, &channels, 4);

	if (data == NULL) return false;

	glGenTextures(1, &out_img.id);
	glBindTexture(GL_TEXTURE_2D, out_img.id);

	// Configuración para iconos nítidos
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	stbi_image_free(data);

	out_img.ancho = w;
	out_img.alto = h;

	return true;
}

// --- Llama a todas las fotos que se quieren usar ---
void InicializarGestorImagenes()
{
	// Carga HUD
	CargarTexturaInterna(".\\textures\\imagenes\\vida.png", imgVida);
	CargarTexturaInterna(".\\textures\\imagenes\\ronda.png", imgRonda);
	CargarTexturaInterna(".\\textures\\imagenes\\dinero.png", imgDinero);

	// Cargar titulo del juego
	CargarTexturaInterna(".\\textures\\imagenes\\titulo.png", imgTitulo);

}

// --- FUNCIÓN DE DIBUJADO ---
void DibujarImagen(const ImagenData& img, float porX, float porY, float escala, const std::vector<TextoOverlay>& textos)
{
	//En caso de que no hay la imagen no hace nada
	if (img.id == 0) return;
	ImVec2 winSize = ImGui::GetWindowSize();

	// Reajusta la escala de la imagen
	float anchoFinal = img.ancho * escala;
	float altoFinal = img.alto * escala;

	// Posicion esquina superior izquierda (Centrada según el nuevo tamaño)
	float imgX = (winSize.x * porX) - (anchoFinal * 0.5f);
	float imgY = (winSize.y * porY) - (altoFinal * 0.5f);

	ImGui::SetCursorPos(ImVec2(imgX, imgY));

	// ImGui dibuja con el tamaño escalado
	ImGui::Image((void*)(intptr_t)img.id, ImVec2(anchoFinal, altoFinal));

	for (const auto& item : textos) {
		// El texto se coloca relativo a la esquina de la imagen
		ImGui::SetCursorPos(ImVec2(imgX + item.offsetX, imgY + item.offsetY));
		ImGui::TextColored(item.color, "%s", item.texto.c_str());
	}
}
// ================ FIN FUNCIONES IMAGENES ================


//------------- PROTOTIPOS DE FUNCIONES ------------------
//(al tener el menu arriba hace falta precargar las funciones)

void iniciarPartida(bool& salir);


void menuShadows();
void menuLight();

ImVec2 colocarBoton(float porX, float porY);
void cambiarEstiloBotones();
void regresarEstiloBotones();
void cambiarEstiloSlider();
void regresarEstiloSlider();
void aplicarEfectoBrillo();


//--------------------------------------------------------------//
//					Funciones de menus							//
//--------------------------------------------------------------//


// Menu inicial al ejecutar
void menu(bool& salir)
{
	// Inicializa frame de ImGui
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// 1. APLICAR EFECTO DE BRILLO (Overlay)
	// Se dibuja antes que todo lo demás para que afecte al juego 3D de fondo
	// pero NO a los menús que dibujamos ahora encima.
	aplicarEfectoBrillo();

	// viewport para posicionar ventana
	const ImGuiViewport* viewport = ImGui::GetMainViewport();


	// =========================================================
	// MENÚ DE INICIO (Pantalla Completa)
	// =========================================================
	if (show_menu_inicio)
	{
		// Configuración ventana fullscreen transparente
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);

		static ImGuiWindowFlags flags = ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;

		if (ImGui::Begin("Menu_Principal_Fullscreen", &show_menu_inicio, flags))
		{
			
			//DibujarImagen(imgTitulo, 0.5f, 0.2f, 0.2f, {});

			// Aplicamos estilos y fuente 
			cambiarEstiloBotones();
			ImGui::SetWindowFontScale(1.5f);

			// --- BOTÓN JUGAR ---
			ImVec2 btnSize = colocarBoton(0.5f, 0.50f);
			if (ImGui::Button("Jugar", btnSize))
			{
				
				show_menu_inicio = false;
				show_jugar = true;
				
				
				debugMode = false;
			}
			
			

			

			// --- BOTÓN SALIR --- (Esquina inferior izquierda)
			btnSize = colocarBoton(0.1f, 0.90f);
			if (ImGui::Button("Salir", btnSize))
			{
				show_menu_inicio = false;
				salir = true;
			}

			// Restauramos estilos y fuente normal
			ImGui::SetWindowFontScale(1.0f);
			regresarEstiloBotones();
		}
		ImGui::End();
	}

	// =========================================================
	// ESTADO DE JUEGO (HUD + PAUSA)
	// =========================================================
	else if (show_jugar && !debugMode)
	{
		iniciarPartida(salir);
	}


	ImGui::Render();
}

//Control de partida normal
void iniciarPartida(bool& salir)
{
	const ImGuiViewport* viewport = ImGui::GetMainViewport();

	// ------------ Cargar imagenes ------------

	// Configurar ventana transparente que cubre toda la pantalla
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowBgAlpha(0.0f); // Totalmente transparente

	ImGuiWindowFlags hudFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs |
		ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
		ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNav;

	if (ImGui::Begin("HUD_Overlay_Images", nullptr, hudFlags))
	{
		char txtVida[16];
		char txtDinero[16];
		char txtRonda[32];

		int ronda_mock = Player::GetInstance().getRound();

		if (debugMode) 
		{
			sprintf_s(txtVida, "%d", 999);
			sprintf_s(txtDinero, "%d", 9999999);
			sprintf_s(txtRonda, "%d", ronda_mock);
		}
		else 
		{
			// Geters variables informativas
			int vida_mock = Player::GetInstance().getHealth();
			int dinero_mock = Player::GetInstance().getMoney();

			// Establecer los codigos de las variables informativas
			sprintf_s(txtVida, "%d", vida_mock);
			sprintf_s(txtDinero, "%d", dinero_mock);
			sprintf_s(txtRonda, "%d", ronda_mock);
		}

		ImGui::SetWindowFontScale(2.2f);

		/*
		// Icono Vida
		DibujarImagen(imgVida, 0.9f, 0.05f, 0.23f, {
			TextoOverlay(txtVida, 120.0f, 25.0f, ImVec4(0.0f, 0.0f, 0.0f, 1.0f))
			});
		
		// Icono Dinero
		DibujarImagen(imgDinero, 0.77f, 0.05f, 0.15f, {
			TextoOverlay(txtDinero, 100.0f, 27.0f, ImVec4(0.0f, 0.0f, 0.0f, 1.0f))
			});
		*/
		// Icono Ronda
		DibujarImagen(imgRonda, 0.50f, 0.05f, 0.28f, {
			TextoOverlay(txtRonda, 100.0f, 8.0f, ImVec4(0.0f, 0.0f, 0.0f, 1.0f))
			});

		ImGui::SetWindowFontScale(1.0f);


		ImGuiWindowFlags btnFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoBackground;

		// Posicionamiento esquina superior derecha
		ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x + viewport->WorkSize.x / 2 - 160.0f, viewport->WorkPos.y + 12.0f), ImGuiCond_Always);

		// Fondo invisible
		ImGui::SetNextWindowBgAlpha(0.0f);

		
	}
	ImGui::End();

	
}




//Sub-menu de prueba para control de los parametros del renderizado de sombras
void menuShadows()
{
	const ImGuiViewport* viewport = ImGui::GetMainViewport();

	// Configuración del Submenú
	ImVec2 size(300, 300);
	// Posición a la izquierda del panel de pruebas
	ImGui::SetNextWindowPos(ImVec2(viewport->WorkSize.x - 550.0f, (viewport->WorkSize.y - size.y) * 0.5f));
	ImGui::SetNextWindowSize(size);

	// Estilo Panel (Igual que Ajustes)
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.15f, 0.15f, 0.17f, 0.9f)); // Gris oscuro
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.85f, 0.55f, 0.25f, 1.0f)); // Cobre
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 5.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 3.0f);

	if (ImGui::Begin("Sombras", &show_submenu_shadows, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize))
	{
		ImGui::Text("Parametros RenderShadows");
		ImGui::Separator();
		ImGui::Spacing();

		cambiarEstiloSlider(); // Estilo para los inputs

		// 1. Vector Direccion Luz (3 parametros)
		ImGui::Text("Direccion Luz (x, y, z)");
		ImGui::PushItemWidth(250);
		ImGui::DragFloat3("##lightDir", debug_lightDir);
		ImGui::PopItemWidth();
		ImGui::Spacing();

		// 2. Box Size
		ImGui::Text("Box Size");
		ImGui::PushItemWidth(250);
		ImGui::DragFloat("##boxSize", &debug_boxSize, 1.0f, 10.0f);
		ImGui::PopItemWidth();
		ImGui::Spacing();

		// 3. Near Plane
		ImGui::Text("Near Plane");
		ImGui::PushItemWidth(250);
		ImGui::DragFloat("##near", &debug_nearPlane, 0.1f, 1.0f);
		ImGui::PopItemWidth();
		ImGui::Spacing();

		// 4. Far Plane
		ImGui::Text("Far Plane");
		ImGui::PushItemWidth(250);
		ImGui::DragFloat("##far", &debug_farPlane, 1.0f, 10.0f);
		ImGui::PopItemWidth();

		regresarEstiloSlider();

		ImGui::Spacing();
		if (ImGui::Button("Cerrar")) show_submenu_shadows = false;
	}
	ImGui::End();

	ImGui::PopStyleVar(2);
	ImGui::PopStyleColor(2);
}

//Sub-menu de prueba para control de los parametros del renderizado de luces
void menuLight()
{
	const ImGuiViewport* viewport = ImGui::GetMainViewport();

	// Configuración del Submenú
	ImVec2 size(300, 300);
	// Posición a la izquierda del panel de pruebas
	ImGui::SetNextWindowPos(ImVec2(viewport->WorkSize.x - 550.0f, (viewport->WorkSize.y - size.y) * 0.5f));
	ImGui::SetNextWindowSize(size);

	// Estilo Panel (Igual que Ajustes)
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.15f, 0.15f, 0.17f, 0.9f)); // Gris oscuro
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.85f, 0.55f, 0.25f, 1.0f)); // Cobre
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 5.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 3.0f);

	if (ImGui::Begin("Luces", &show_submenu_light, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize))
	{
		ImGui::Text("Parametros RenderGame");
		ImGui::Separator();
		ImGui::Spacing();

		cambiarEstiloSlider(); // Estilo para los inputs

		// 
		ImGui::Text("Ambient Intensity");
		ImGui::PushItemWidth(250);
		ImGui::DragFloat("##near", &debug_ambientIntensity, 0.1f, 1.0f);
		ImGui::PopItemWidth();
		ImGui::Spacing();

		// 1. Vector Color luz (3 parametros)
		ImGui::Text("Color luz (r, g, b)");
		ImGui::PushItemWidth(250);
		ImGui::ColorEdit3("##lightColor", debug_lightColor);
		ImGui::PopItemWidth();
		ImGui::Spacing();


		regresarEstiloSlider();

		ImGui::Spacing();
		if (ImGui::Button("Cerrar")) show_submenu_light = false;
	}
	ImGui::End();

	ImGui::PopStyleVar(2);
	ImGui::PopStyleColor(2);
}


//--------------------------------------------------------------//
//					Funciones de diseño							//
//--------------------------------------------------------------//

// Función para el efecto de oscurecer pantalla
// Crea una ventana falsa que no se puede interactuar con ella
void aplicarEfectoBrillo()
{
	// Si el brillo es máximo, no dibujamos nada para ahorrar recursos
	if (nivelBrillo >= 0.99f) return;

	const ImGuiViewport* viewport = ImGui::GetMainViewport();

	// Posicionamos una ventana invisible sobre toda la pantalla
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);

	// Calculamos la opacidad negra: 
	// Brillo 1.0 -> Alpha 0.0 (Transparente)
	// Brillo 0.1 -> Alpha 0.9 (Casi negro)
	ImGui::SetNextWindowBgAlpha(1.0f - nivelBrillo);

	// Fondo negro (evita niebla blanca en temas claros)
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 1));

	// Flags importantes: NoInputs permite que los clicks pasen a través de esta capa oscura al juego
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoResize;

	if (ImGui::Begin("OverlayBrillo", nullptr, flags))
	{
		// Ventana vacía, solo sirve para tintar
	}
	ImGui::End();

	ImGui::PopStyleColor();
}

// Posicionar botones centrados en porcentaje relativo a la ventana
ImVec2 colocarBoton(float porX, float porY)
{
	// Tamaño del botón
	ImVec2 button_size(200, 50);

	// Obtener tamaño de la ventana actual
	ImVec2 window_size = ImGui::GetWindowSize();

	// Calcular posición absoluta
	float posX = (window_size.x * porX) - (button_size.x * 0.5f);
	float posY = (window_size.y * porY) - (button_size.y * 0.5f);

	// Posición del cursor
	ImGui::SetCursorPos(ImVec2(posX, posY));

	return button_size;
}

//Establece el estilo de fabrica de los botones
void cambiarEstiloBotones()
{
	// --- COLORES ---

	// Botón Normal: Plateado Oscuro
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.40f, 0.42f, 0.45f, 1.0f));
	// Botón Hover (Ratón encima): Plateado más claro
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.55f, 0.57f, 0.60f, 1.0f));
	// Botón Activo (Click): Gris oscuro 
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.25f, 0.27f, 0.30f, 1.0f));
	// Borde: Color cobre 
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.85f, 0.55f, 0.25f, 1.0f));

	// --- VARIABLES ---
	// Grosor del borde
	ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 4.0f);
	// Redondeo de esquinas
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
}

//Devuelve el estilo previo de los botones
void regresarEstiloBotones()
{
	// Sacamos los 4 colores
	ImGui::PopStyleColor(4);
	// Sacamos las 2 variables
	ImGui::PopStyleVar(2);
}

//Establece el estilo fabrica de los sliders
void cambiarEstiloSlider()
{
	// Fondo del slider (Canal): Gris Oscuro
	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.20f, 0.20f, 0.20f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.30f, 0.30f, 0.30f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.25f, 0.25f, 0.25f, 1.0f));

	// Agarradera (Knob): Color Cobre 
	ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImVec4(0.85f, 0.55f, 0.25f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, ImVec4(0.95f, 0.65f, 0.35f, 1.0f));

	// Borde del slider: Cobre
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.85f, 0.55f, 0.25f, 1.0f));

	// Variables estéticas
	// AQUI SE AGREGA EL BORDE AL SLIDER (MANTENIDO 2.0f para visibilidad):
	ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_GrabRounding, 3.0f);
}

//Devuelve el estilo de previo de los sliders
void regresarEstiloSlider()
{
	// Sacamos los 6 colores
	ImGui::PopStyleColor(6);
	// Sacamos las 3 variables
	ImGui::PopStyleVar(3);
}

