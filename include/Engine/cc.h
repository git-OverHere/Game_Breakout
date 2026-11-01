#pragma once

#include "Engine/GameObject.h"
#include "Engine/BallObject.h"
#include "Game.h"

#include <tuple>

namespace cc
{
    typedef std::tuple<bool, Direction, glm::vec2> Collision;
    // AABB - AABB collision
    bool CheckCollision(Engine::GameObject &one, Engine::GameObject &two);
    // AABB - Circle collision
    Collision CheckCollision(Engine::BallObject &one, Engine::GameObject &two);

    // get direction the ball penetrated
    Direction VectorDirection(glm::vec2 target);

}