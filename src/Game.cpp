#include "Game.h"

#include "Core/ResourceManager.h"
#include "Core/SpriteRenderer.h"
#include "Engine/GameObject.h"

Core::SpriteRenderer *Renderer;

// define player
const glm::vec2 PLAYER_SIZE(100.0f, 20.0f);
const float PLAYER_VELOCITY(500.0f);
Engine::GameObject *Player;

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
    // configure shaders
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width),
        static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);
    Core::ResourceManager::GetShader("sprite").use().setInt("image", 0);
    Core::ResourceManager::GetShader("sprite").setMat4("projection", 1, false, projection);
    // set render-specific controls
    Core::Shader shader = Core::ResourceManager::GetShader("sprite");
    Renderer = new Core::SpriteRenderer(shader);
    //load textures
    Core::ResourceManager::LoadTexture("assets/textures/awesomeface.png", true, "face");
    Core::ResourceManager::LoadTexture("assets/textures/background.jpg", false, "background");
    Core::ResourceManager::LoadTexture("assets/textures/block.png", false, "block");
    Core::ResourceManager::LoadTexture("assets/textures/block_solid.png", false, "block_solid");
    Core::ResourceManager::LoadTexture("assets/textures/paddle.png", true, "paddle");
    // load player
    glm::vec2 playerPos = glm::vec2(
        this->Width / 2.0f - PLAYER_SIZE.x / 2.0f,
        this->Height - PLAYER_SIZE.y
    );
    Core::Texture2D paddle = Core::ResourceManager::GetTexture("paddle");
    Player = new Engine::GameObject(playerPos, PLAYER_SIZE, paddle);
    // load levels
    Engine::GameLevel one; one.Load("assets/levels/standard.lvl", this->Width, this->Height / 2);
    Engine::GameLevel two; two.Load("assets/levels/small_gaps.lvl", this->Width, this->Height / 2);
    Engine::GameLevel three; three.Load("assets/levels/space_invader.lvl", this->Width, this->Height / 2);
    Engine::GameLevel four; four.Load("assets/textures/bloc_solid.png", this->Width, this->Height / 2);
    this->Levels.push_back(one);
    this->Levels.push_back(two);
    this->Levels.push_back(three);
    this->Levels.push_back(four);
    this->Level = 0;
}

void Game::Update(float dt)
{

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
                Player->Position.x -= velocity;
        }
        if (this->Keys[GLFW_KEY_D] || this->Keys[GLFW_KEY_RIGHT])
        {
            if (Player->Position.x <= this->Width - Player->Size.x)
                Player->Position.x += velocity;
        }
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
    }
}
