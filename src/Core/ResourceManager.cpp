#include "Core/ResourceManager.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>

#include "stb_image/stb_image.h"

using namespace Core;

// Instantiate static variables
std::map<std::string, Texture2D>    ResourceManager::Textures;
std::map<std::string, Shader>       ResourceManager::Shaders;

Shader ResourceManager::LoadShader(const char *vShaderFile, const char *fShaderFile, std::string name)
{
    Shaders[name] = loadShaderFromFile(vShaderFile, fShaderFile);
    return Shaders[name];
}

Shader ResourceManager::LoadShaderFromCode(const char *vShaderSource, const char * fShaderSource, std::string name)
{
    Shader shader;
    shader.Compile(vShaderSource, fShaderSource);
    Shaders[name] = shader;
    return Shaders[name];
}

Shader ResourceManager::GetShader(std::string name)
{
    return Shaders[name];
}

Texture2D ResourceManager::LoadTexture(const char *file, bool alpha, std::string name)
{
    Textures[name] = loadTextureFromFile(file, alpha);
    return Textures[name];
}

Texture2D ResourceManager::GetTexture(std::string name)
{
    return Textures[name];
}

void ResourceManager::Clear() 
{
    // (properly) delete all shaders
    for (auto iter : Shaders)
        glDeleteProgram(iter.second.ID);
    // (properly) delete all textures
    for (auto iter : Textures)
        glDeleteTextures(1, &iter.second.ID);
}

Shader ResourceManager::loadShaderFromFile(const char *vShaderFile, const char *fShaderFile)
{
    // retrieve vertex/fragment source code from file path
    std::string vertexCode;
    std::string fragmentCode;
    try
    {
        // open files
        std::ifstream vertexShaderFile(vShaderFile);
        std::ifstream fragmentShaderFile(fShaderFile);
        std::stringstream vShaderStream, fShaderStream;
        // read file buffer contents into streams
        vShaderStream << vertexShaderFile.rdbuf();
        fShaderStream << fragmentShaderFile.rdbuf();
        // close the handlers
        vertexShaderFile.close();
        fragmentShaderFile.close();
        // convert stream into string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch(const std::exception& e)
    {
        std::cerr << "ERROR::SHADER: Failed to read shader files" << std::endl;
    }
    const char *vShaderCode = vertexCode.c_str();
    const char *fShaderCode = fragmentCode.c_str();
    // now create shader object from source code;
    Shader shader;
    shader.Compile(vShaderCode, fShaderCode);
    return shader;
}

Texture2D ResourceManager::loadTextureFromFile(const char *file, bool alpha)
{
    // create texture object
    Texture2D texture;
    if (alpha)
    {
        texture.Internal_Format = GL_RGBA;
        texture.Image_Format = GL_RGBA;
    }
    // load image
    int width, height, nrChannels;
    unsigned char * data = stbi_load(file, &width, &height, &nrChannels, 0);
    // determine if image needs to flip
    bool flip = false;
    std::string fileExt = ResourceManager::getImageFormat(file);
    if (fileExt != "jpeg" && fileExt != "jpg")
    {
        flip = true;
    }
    // now generate texture
    texture.Generate(width, height, data, nrChannels, flip);
    // and finally free image data
    stbi_image_free(data);
    return texture;
}

std::string ResourceManager::getImageFormat(const char *file)
{
    if (!file )
        return "";

    std::string strPath(file);
    auto dotPos = strPath.find_last_of(".");
    if (dotPos == std::string::npos)
        return "";

    std::string ext = strPath.substr(dotPos + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    return ext;
}