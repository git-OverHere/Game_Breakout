#include "Core/Texture2D.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image/stb_image.h>

#include <iostream>
#include <string>
#include <algorithm>

using namespace Core;

// Create 2D Texture. Has default repeat wrapper.
Texture2D::Texture2D()
  : Width(0), Height(0),
  Internal_Format(GL_RGB), Image_Format(GL_RGB),
  Wrap_S(GL_REPEAT), Wrap_T(GL_REPEAT),
  Filter_Min(GL_LINEAR_MIPMAP_LINEAR), Filter_Max(GL_NEAREST)
{
  glGenTextures(1, &(this->ID));
  if (this->ID == 0)
  {
    std::cerr << "ERROR::TEXTURE_GENERATION: Error generating texture." << std::endl;
  }
}

void Texture2D::Generate(unsigned int width, unsigned int height, unsigned char* data, unsigned int nrChannels, bool flip)
{
  this->Width = width;
  this->Height = height;
  if (flip)
  {
    stbi_set_flip_vertically_on_load(true);
  }
  if (data == nullptr)
  {
    std::cerr << "ERROR::TEXTURE_GENERATION: Invalid data received during generation." << std::endl;
  }

  // set Texture wrap and filter modes
  glBindTexture(GL_TEXTURE_2D, this->ID);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, this->Wrap_S);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, this->Wrap_T);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->Filter_Min);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, this->Filter_Max);


  if (nrChannels == 1)
  {
    this->Internal_Format = GL_RED;
    this->Image_Format = GL_RED;
    glTexImage2D(GL_TEXTURE_2D, 0, this->Internal_Format, this->Width, this->Height, 0, this->Image_Format, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_RED);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_RED);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_RED);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_RED);
  }
  else if (nrChannels == 3)
  {
    this->Internal_Format = GL_RGB;
    this->Image_Format = GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, this->Internal_Format, this->Width, this->Height, 0, this->Image_Format, GL_UNSIGNED_BYTE, data);
  }
  else if (nrChannels == 4)
  {
    this->Internal_Format = GL_RGBA;
    this->Image_Format = GL_RGBA;
    glTexImage2D(GL_TEXTURE_2D, 0, this->Internal_Format, this->Width, this->Height, 0, this->Image_Format, GL_UNSIGNED_BYTE, data);
  }

  glGenerateMipmap(GL_TEXTURE_2D);
}

void Texture2D::Bind() const
{
  glBindTexture(GL_TEXTURE_2D, this->ID);
}

std::string Texture2D::imageFormat(const char *imagePath)
{
  if (!imagePath)
    return "";

  std::string strPath(imagePath);
  auto dotPos = strPath.find_last_of(".");
  if (dotPos == std::string::npos)
    return "";

  std::string ext = strPath.substr(dotPos + 1);
  std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

  return ext;
}
