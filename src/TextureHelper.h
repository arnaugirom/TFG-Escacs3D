#pragma once
#include <string>
#include <iostream>
#include "ImGui/imgui.h"
#include <GL/glew.h> // O glad/glfw dependiendo de tu setup
#include "stb_image.h" // Asegúrate de tener esto incluido

// Esta estructura maneja todo el trabajo sucio de OpenGL
struct SmartTexture {
    GLuint id = 0;
    int width = 0;
    int height = 0;
    std::string path;

    // Constructor automático
    SmartTexture(const std::string& filePath) {
        Load(filePath);
    }

    // Constructor vacío por si quieres cargar luego
    SmartTexture() {}

    void Load(const std::string& filePath) {
        path = filePath;
        unsigned char* data = stbi_load(path.c_str(), &width, &height, NULL, 4);
        if (!data) {
            std::cerr << "ERROR: No se pudo cargar imagen: " << path << std::endl;
            return;
        }

        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);

        // Configuración estándar para pixel art o UI (Linear filtering)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
    }

    // --- LA MAGIA: Función que devuelve el ID listo para ImGui ---
    void* GetImGuiID() const {
        return (void*)(intptr_t)id;
    }

    // Devuelve el tamaño como ImVec2 para usar directo en botones
    ImVec2 GetSize() const {
        return ImVec2((float)width, (float)height);
    }
};