#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

#include "Engine/GameObject.h"
#include "Core/Shader.h"
#include "Core/Texture2D.h"

namespace Engine
{
    struct Particle {
        glm::vec2 Position, Velocity;
        glm::vec4 Color;
        float Life;

        Particle()
            : Position(0.0f), Velocity(0.0f), Color(1.0f), Life(0.0f) { }
    };

    class ParticleGenerator
    {
        public:
            ParticleGenerator(Core::Shader shader, Core::Texture2D texture, unsigned int amount);
            // update all particles
            void Update(float dt, Engine::GameObject &object, unsigned int newParticles, glm::vec2 offset = glm::vec2(0.0f, 0.0f));
            // render all particles
            void Draw();

        private:
            std::vector<Particle> particles;
            unsigned int amount;
            // render state
            Core::Shader shader;
            Core::Texture2D texture;
            unsigned int VAO;
            void init();
            // returns the first Particle index that has no life
            unsigned int firstUnusedParticle();
            // gives particle a random pos, color and 1-second life
            void respawnParticle(Particle &particle, Engine::GameObject &object, glm::vec2 offset = glm::vec2(0.0f, 0.0f));
    };
};