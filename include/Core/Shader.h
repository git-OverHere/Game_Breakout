#pragma once

#include <iostream>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Core
{
    class Shader
    {
        public:
            struct Path
            {
                const char *value;
            };
            struct Code
            {
                const char *value;
            };
            unsigned int ID;
            // Constructor
            Shader() { }
            // set current shader as active
            Shader &use();
            // compiles shader from given source code
            void Compile(const char *vertexSource, const char *fragmentSource);
            // Uniform Util
            void setBool(const std::string &name, bool value) const;
            void setInt(const std::string &name, int value) const;
            void setFloat(const std::string &name, float value) const;
            void setFloat3(const std::string &name, float x, float y, float z) const;
            void setFloat4(const std::string &name, float x, float y, float z, float w) const;
            void setMat4(const std::string &name, GLsizei count, GLboolean transpose, const glm::mat4 &trans) const;
        private:
            void checkCompileErrors(unsigned int object, std::string type);
    };
};

/*
How to use:

Core::Shader newShader = Core::Shader(vertex, fragment)
newShader.use()

*/