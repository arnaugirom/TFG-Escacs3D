#pragma once
#include <vector>
#include <string>
#include "Turret.h"
#include "player.h"

// -----------------------------------------------------------
//            Variables para el control de menus
// -----------------------------------------------------------

extern bool show_menu_inicio;
extern bool show_menu_ajustes;
extern bool show_jugar;
extern bool show_menu_creditos;
extern bool juego_pausado;
extern bool show_menu_pruebas;
extern bool show_menu_construccion;
extern bool show_menu_mejora;
extern bool show_menu_muerte;
extern bool enConstruccion;
extern bool debugMode;
extern bool reiniciar;

extern int idTipoTorreta;
extern int idTorretaSeleccionada;
extern bool torretaComprada;
extern int idStat;
extern bool mejoraComprada;
extern Turret *tur;
extern Player *pl;

// -----------------------------------------------------------
//        Variables para el control de menus de prueba
// -----------------------------------------------------------

extern bool debug_detener_tiempo;	// Controla si el timer corre
extern bool debug_resetear_todo;	// Orden de matar y reiniciar
extern int debug_id_enemigo_spawn;  // Id del enemigo
extern int debug_num_enemigo_spawn; // Cantidad de enemigos
extern bool debug_solicitar_spawn;  // La orden de spawnear
extern bool show_submenu_shadows;	// Booleano para ventana de sombras
extern bool show_submenu_light;		// Booleano para ventana de luces
extern float debug_speedMult;		// Booleano para ventana de sombras

// -----------------------------------------------------------
//    Variables para el control de menus de prueba de luces
// -----------------------------------------------------------
extern float debug_lightDir[3];		 // Vector direccion luz
extern float debug_boxSize;			 // Tamano caja sombras
extern float debug_nearPlane;		 // Plano cercano
extern float debug_farPlane;		 // Plano lejano
extern float debug_ambientIntensity; // Intensidad de los colores
extern float debug_lightColor[3];	 // Vector RGB color
extern int debug_renderMode;		 // Modo de renderizado 




// ----------- Funciones menus -----------
void menu(bool& salir);
void iniciarPartida(bool& salir);
void menuPausa(bool& salir, const ImGuiViewport* viewport);
void menuAjustes();
void menuCreditos();
void menuConstruccion();
void menuMejora();
void menuMuerte();
void imprimirFotoTorreta(ImVec2 winSize, int idT, float porX, float porY, float escalaImg, char tDmg[16], char tVel[16], char tRng[16], char tPrecio[16]);


// ----------- Funciones para pruebas -----------
void menuPruebas(bool& salir);
void menuShadows();
void menuLight();

void aplicarEfectoBrillo();
void cambiarEstiloBotones();
void regresarEstiloBotones();
ImVec2 colocarBoton(float porX, float porY);
void cambiarEstiloSlider();
void regresarEstiloSlider();


// =========================================================
//  GESTIÓN DE IMÁGENES 
// =========================================================


// Estructura que agrupa todos los parametros de la imagen (ID y tamaño)
struct ImagenData {
    GLuint id = 0;
    int ancho = 0;
    int alto = 0;
};

// Estructura para definir texto superpuesto
struct TextoOverlay {
    std::string texto;
    float offsetX; // Posición X relativa a la esquina de la imagen
    float offsetY; // Posición Y relativa a la esquina de la imagen
    ImVec4 color;  // Color del texto

    // Constructor rápido
    TextoOverlay(std::string t, float x, float y, ImVec4 c = ImVec4(1, 1, 1, 1))
        : texto(t), offsetX(x), offsetY(y), color(c) {
    }
};

// --- VARIABLES GLOBALES DE IMÁGENES ---
extern ImagenData imgVida;
extern ImagenData imgDinero;
extern ImagenData imgRonda;
extern ImagenData imgTorretas[5]; // Array para las 5 torretas
extern ImagenData imgTorretasMejora[5]; // Array para las 5 torretas menu mejoras
extern ImagenData imgMuerte; //Imagen de la pantalla de muerte
extern ImagenData imgTitulo; //Imagen con el titulo del juego

// --- FUNCIONES ---

// Carga todas las imágenes del juego (Llamar una vez al inicio)
void InicializarGestorImagenes();

// Dibuja una imagen en una posición relativa (0.0 a 1.0) de la pantalla
// Permite pasar una lista de textos para poner encima.
// escala: 1.0f = Tamaño original, 0.5f = Mitad de tamaño, etc.
void DibujarImagen(const ImagenData& img, float porX, float porY, float escala, const std::vector<TextoOverlay>& textos = {});
