#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image/stb_image.h>

#include <iostream>

namespace Core
{
  // Defaults to only using texture address 0
  class Texture2D
  {
  public:
    // holds the opengl ID of the texture object
    unsigned int ID;
    // texture dimensions and format
    unsigned int Width;
    unsigned int Height;
    unsigned int Internal_Format;
    unsigned int Image_Format;
    // texture configuration
    unsigned int Wrap_S; // wrapping mode on S axis (like X axis)
    unsigned int Wrap_T; // wrapping mode on T axis (like Y axis)
    unsigned int Filter_Min; // filtering mode for texture px < screen px
    unsigned int Filter_Max; // filtering mode for texture px > screen px

    // Default constructor (Change default texture config here)
    Texture2D();

    void Generate(unsigned int width, unsigned int height, unsigned char* data, unsigned int nrChannels, bool flip = false);
    // binds the texture as the current active GL_TEXTURE_2D
    void Bind() const;

  private:
    // get the image type/extension from its name
    std::string imageFormat(const char *imagePath);
  };
}

/*
How to use:

Core::Texture newTexture = Texture();
int width, height, nrChannels;
unsigned char* data = stbi_load(file, &width, &height, &nrChannels, 0);
newTexture.Generate(width, height, data);

newTexture.Bind();

*/