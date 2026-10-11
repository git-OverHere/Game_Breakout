#include "Game.h"

#include "Core/ResourceManager.h"
#include "Core/SpriteRenderer.h"

Core::SpriteRenderer *Renderer;

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
    }
}
