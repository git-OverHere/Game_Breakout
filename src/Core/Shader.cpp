#include "Core/Shader.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

using namespace Core;

Shader &Shader::use()
{
  glUseProgram(this->ID);
  return *this;
}

void Shader::Compile(const char *vertexSource, const char *fragmentSource)
{
  // Vertex Shader
  unsigned int vShader, fShader;
  vShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vShader, 1, &vertexSource, NULL);
  glCompileShader(vShader);
  checkCompileErrors(vShader, "VERTEX");

  // Fragment Shader
  fShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fShader, 1, &fragmentSource, NULL);
  glCompileShader(fShader);
  checkCompileErrors(fShader, "FRAGMENT");

  // Shader Program
  this->ID = glCreateProgram();
  glAttachShader(this->ID, vShader);
  glAttachShader(this->ID, fShader);
  glLinkProgram(this->ID);
  checkCompileErrors(this->ID, "PROGRAM");

  // Delete shaders after linking
  glDeleteShader(vShader);
  glDeleteShader(fShader);
}

void Shader::setBool(const std::string &name, bool value) const
{
  glUniform1i(glGetUniformLocation(Shader::ID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string &name, int value) const
{
  glUniform1i(glGetUniformLocation(Shader::ID, name.c_str()), value);
}

void Shader::setFloat(const std::string &name, float value) const
{
  glUniform1f(glGetUniformLocation(Shader::ID, name.c_str()), value);
}

void Shader::setFloat3(const std::string &name, float x, float y, float z) const
{
  glUniform3f(glGetUniformLocation(Shader::ID, name.c_str()), x, y, z);
}

void Shader::setFloat4(const std::string &name, float x, float y, float z, float w) const
{
  glUniform4f(glGetUniformLocation(Shader::ID, name.c_str()), x, y, z, w);
}

void Shader::setMat4(const std::string &name, GLsizei count, GLboolean transpose, const glm::mat4 &trans) const
{
  glUniformMatrix4fv(glGetUniformLocation(Shader::ID, name.c_str()), count, transpose, glm::value_ptr(trans));
}

void Shader::checkCompileErrors(unsigned int object, std::string type)
{
    int success;
    char infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(object, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(object, 1024, NULL, infoLog);
            std::cout << "| ERROR::CORE::Shader: Compile-time error: Type: " << type << "\n"
                << infoLog << "\n -- --------------------------------------------------- -- "
                << std::endl;
        }
    }
    else
    {
        glGetProgramiv(object, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(object, 1024, NULL, infoLog);
            std::cout << "| ERROR::CORE::Shader: Link-time error: Type: " << type << "\n"
                << infoLog << "\n -- --------------------------------------------------- -- "
                << std::endl;
        }
    }
}