#include "Game.h"

#include "Core/ResourceManager.h"
#include "Core/SpriteRenderer.h"
#include "Engine/GameObject.h"
#include "Engine/BallObject.h"
#include "Engine/cc.h"
#include "Engine/ParticleGenerator.h"

#include <vector>


Core::SpriteRenderer *Renderer;

// define player
const glm::vec2 PLAYER_SIZE(100.0f, 20.0f);
const float PLAYER_VELOCITY(500.0f);
Engine::GameObject *Player;
// define the ball
const glm::vec2 INITIAL_BALL_VELOCITY(100.0f, -350.0f);
const float BALL_RADIUS = 12.5f;
Engine::BallObject *Ball;
// define the particle
Engine::ParticleGenerator *Particles;

Game::Game(unsigned int width, unsigned int height)
    : State(GAME_ACTIVE), Keys(), Width(width), Height(height)
{}

Game::~Game()
{}

void Game::Init()
{
    // load shaders
    Core::ResourceManager::LoadShader("assets/shaders/vertex/sprite_renderer.vs",
        "assets/shaders/fragment/sprite_renderer.fs", "sprite");
    Core::ResourceManager::LoadShader("assets/shaders/vertex/particle_renderer.vs",
        "assets/shaders/fragment/particle_renderer.fs", "particle");
    // configure shaders
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width),
        static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);
    Core::ResourceManager::GetShader("sprite").use().setInt("image", 0);
    Core::ResourceManager::GetShader("sprite").setMat4("projection", 1, false, projection);
    Core::ResourceManager::GetShader("particle").use().setMat4("projection", 1, false, projection);
    // set render-specific controls
    Core::Shader shader = Core::ResourceManager::GetShader("sprite");
    Renderer = new Core::SpriteRenderer(shader);
    //load textures
    Core::ResourceManager::LoadTexture("assets/textures/awesomeface.png", true, "face");
    Core::ResourceManager::LoadTexture("assets/textures/background.jpg", false, "background");
    Core::ResourceManager::LoadTexture("assets/textures/block.png", false, "block");
    Core::ResourceManager::LoadTexture("assets/textures/block_solid.png", false, "block_solid");
    Core::ResourceManager::LoadTexture("assets/textures/paddle.png", true, "paddle");
    Core::ResourceManager::LoadTexture("assets/textures/particle.png", true, "particle");
    // load player
    glm::vec2 playerPos = glm::vec2(
        this->Width / 2.0f - PLAYER_SIZE.x / 2.0f,
        this->Height - PLAYER_SIZE.y
    );
    Core::Texture2D paddle = Core::ResourceManager::GetTexture("paddle");
    Player = new Engine::GameObject(playerPos, PLAYER_SIZE, paddle);
    // load ball
    // place the ball on top of player in the center
    glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS,
                                                -BALL_RADIUS * 2.0f);
    Ball = new Engine::BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY,
                                    Core::ResourceManager::GetTexture("face"));                                            
    // load particle    
    Particles = new Engine::ParticleGenerator(
        Core::ResourceManager::GetShader("particle"),
        Core::ResourceManager::GetTexture("particle"),
        500
    );
    // load levels
    Engine::GameLevel one; one.Load("assets/levels/standard.lvl", this->Width, this->Height / 2);
    Engine::GameLevel two; two.Load("assets/levels/small_gaps.lvl", this->Width, this->Height / 2);
    Engine::GameLevel three; three.Load("assets/levels/space_invader.lvl", this->Width, this->Height / 2);
    Engine::GameLevel four; four.Load("assets/levels/bounce_galore.lvl", this->Width, this->Height / 2);
    this->Levels.push_back(one);
    this->Levels.push_back(two);
    this->Levels.push_back(three);
    this->Levels.push_back(four);
    this->Level = 3;
}

void Game::Update(float dt)
{
    Ball->Move(dt, this->Width);
    this->DoCollisions();
    Particles->Update(dt, *Ball, 2, glm::vec2(Ball->Radius / 2.0f));
    if (Ball->Position.y >= this->Height)
    {
        this->ResetLevel();
        this->ResetPlayer();
    }
}

void Game::ProcessInput(float dt)
{
    if (this->State == GAME_ACTIVE)
    {
        float velocity = PLAYER_VELOCITY * dt;
        // move playerboard
        if (this->Keys[GLFW_KEY_A] || this->Keys[GLFW_KEY_LEFT])
        {
            if (Player->Position.x >= 0.0f)
            {
                Player->Position.x -= velocity;
                if (Ball->Stuck)
                    Ball->Position.x -= velocity;
            }
        }
        if (this->Keys[GLFW_KEY_D] || this->Keys[GLFW_KEY_RIGHT])
        {
            if (Player->Position.x <= this->Width - Player->Size.x)
            {
                Player->Position.x += velocity;
                if (Ball->Stuck)
                    Ball->Position.x += velocity;
            }
        }
        // Unstuck ball to player
        if (this->Keys[GLFW_KEY_SPACE])
            Ball->Stuck = false;
    }
}

void Game::Render()
{
    if (this->State == GAME_ACTIVE)
    {
        // draw background
        Core::Texture2D background = Core::ResourceManager::GetTexture("background");
        Renderer->DrawSprite(background, glm::vec2(0.0f, 0.0f), 
            glm::vec2(this->Width, this->Height), 0.0f);
        // draw level
        this->Levels[this->Level].Draw(*Renderer);
        // draw player paddle
        Player->Draw(*Renderer);
        Particles->Draw();
        Ball->Draw(*Renderer);
    }
}

void Game::DoCollisions()
{
    for (Engine::GameObject &box : this->Levels[this->Level].Bricks)
    {
        if (box.Destroyed) continue;
        cc::Collision collision = cc::CheckCollision(*Ball, box);
        if (!std::get<0>(collision)) continue;
        if (!box.IsSolid) 
        {
            box.Destroyed = true;
        }
        // collision resolution
        Direction dir = std::get<1>(collision);
        glm::vec2 diff_vector = std::get<2>(collision);
        if (dir == LEFT || dir == RIGHT)
        {
            Ball->Velocity.x = -Ball->Velocity.x;
            // relocate outside AABB
            float penetration = Ball->Radius - std::abs(diff_vector.x);
            if (dir == LEFT)
                Ball->Position.x += penetration;
            else
                Ball->Position.x -= penetration;
        }
        else
        {
            Ball->Velocity.y = -Ball->Velocity.y;
            float penetration = Ball->Radius - std::abs(diff_vector.y);
            if (dir == UP)
                Ball->Position.y -= penetration;
            else
                Ball->Position.y += penetration;
        }      
    }
    // check player collision
    cc::Collision result = cc::CheckCollision(*Ball, *Player);
    if (Ball->Stuck || !std::get<0>(result)) return;
    // hitting farther from the center must make it move stronger
    float centerBoard = Player->Position.x + Player->Size.x / 2.0f;
    float distance = (Ball->Position.x + Ball->Radius) - centerBoard;
    float percentage = distance / (Player->Size.x / 2.0f);
    // then move accordingly
    float strength = 2.0f;
    glm::vec2 oldVelocity = Ball->Velocity;
    Ball->Velocity.x = INITIAL_BALL_VELOCITY.x * percentage * strength;
    Ball->Velocity.y = -1.0f * abs(Ball->Velocity.y);
    Ball->Velocity = glm::normalize(Ball->Velocity) * glm::length(oldVelocity);
}

void Game::ResetLevel()
{
    if (this->Level == 0)
        this->Levels[0].Load("assets/levels/standard.lvl", this->Width, this->Height / 2);
    else if (this->Level == 1)
        this->Levels[1].Load("assets/levels/small_gaps.lvl", this->Width, this->Height / 2);
    else if (this->Level == 2)
        this->Levels[2].Load("assets/levels/space_invader.lvl",  this->Width, this->Height / 2);
    else if (this->Level == 3)
        this->Levels[3].Load("assets/levels/bounce_galore.lvl", this->Width, this->Height / 2);
}

void Game::ResetPlayer()
{
    // reset player/ball states
    Player->Size = PLAYER_SIZE;
    Player->Position = glm::vec2(this->Width / 2.0f - PLAYER_SIZE.x / 2.0f, this->Height - PLAYER_SIZE.y);
    Ball->Reset(Player->Position + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -(BALL_RADIUS * 2.0f)), INITIAL_BALL_VELOCITY);
}

