#include "Engine/BallObject.h"

using namespace Engine;

glm::vec2 BallObject::Move(float dt, unsigned int window_width)
{
    if (this->Stuck)
    { return this->Position; }

    // move the ball
    this->Position += this->Velocity * dt;
    // check if outside window bounds; 
    // if so, reverse velocity and restore at correct position
    if (this->Position.x <= 0.0f)
    {
        this->Velocity.x = -this->Velocity.x;
        this->Position.x = 0.0f;
    }
    else if (this->Position.x + this->Size.x >= window_width)
    {
        this->Velocity.x = -this->Velocity.x;
        this->Position.x = window_width - this->Size.x;
    }
    if (this->Position.y <= 0.0f)
    {
        this->Velocity.y = -this->Velocity.y;
        this->Position.y = 0.0f;
    }

    return this->Position;
}