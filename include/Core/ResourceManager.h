#pragma once

#include <map>
#include <string>

#include "Core/Shader.h"
#include "Core/Texture2D.h"

namespace Core
{
    // A static singleton ResourceManager class that hosts several
    // functions to load Textures and Shaders. Each loaded texture
    // and/or shader is also stored for future reference by string
    // handles. All functions and resources are static and no 
    // public constructor is defined.
    class ResourceManager
    {
    public:
        // resource storage
        static std::map<std::string, Shader> Shaders;
        static std::map<std::string, Texture2D> Textures;
        // loads (and generates) a shader program from file loading vertex, fragment shader's source code.
        static Shader LoadShader(const char *vShaderFile, const char *fShaderFile, std::string name);
        static Shader LoadShaderFromCode(const char *vShaderSource, const char * fShaderSource, std::string name);
        // retrieves a stored shader
        static Shader GetShader(std::string name);
        // loads (and generates) a texture from file
        static Texture2D LoadTexture(const char *file, bool alpha, std::string name);
        // retrieves a stored texture
        static Texture2D GetTexture(std::string name);
        // properly deallocates all loaded resources
        static void Clear();
    private:
        // private constructor, that is we do not want any actual resource manager objects. Its members and functions should be publicly available (static).
        ResourceManager() { }
        // loads and generates a shader from file
        static Shader loadShaderFromFile(const char *vShaderFile, const char *fShaderFile);
        // loads a single texture from file
        static Texture2D loadTextureFromFile(const char *file, bool alpha);
        // get image format from file name
        static std::string getImageFormat(const char *file);
    };
};

/*
How to use:

Shader shader = ResourceManager::LoadShader("vertex.vs", "fragment.vs", "test");
// then use it
shader.Use();
// or
ResourceManager::GetShader("test").Use();
*/