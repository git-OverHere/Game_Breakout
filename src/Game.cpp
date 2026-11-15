#include "Game.h"

#include "Core/ResourceManager.h"
#include "Core/SpriteRenderer.h"
#include "Engine/GameObject.h"
#include "Engine/BallObject.h"
#include "Engine/cc.h"
#include "Engine/ParticleGenerator.h"
#include "Core/PostProcessor.h"

#include <vector>
#include <algorithm>


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
// define the post processor
Core::PostProcessor *Effects;
float ShakeTime = 0.0f;

Game::Game(unsigned int width, unsigned int height)
    : State(GAME_ACTIVE), Keys(), Width(width), Height(height)
{}

Game::~Game()
{
    delete Renderer;
    delete Player;
    delete Ball;
    delete Particles;
    delete Effects;
}

void Game::Init()
{
    // load shaders
    Core::ResourceManager::LoadShader("assets/shaders/vertex/sprite_renderer.vs",
        "assets/shaders/fragment/sprite_renderer.fs", "sprite");
    Core::ResourceManager::LoadShader("assets/shaders/vertex/particle_renderer.vs",
        "assets/shaders/fragment/particle_renderer.fs", "particle");
    Core::ResourceManager::LoadShader("assets/shaders/vertex/post_processor.vs",
        "assets/shaders/fragment/post_processor.fs", "postprocessing");
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
    Core::ResourceManager::LoadTexture("assets/textures/powerup_speed.png", true, "powerup_speed");
    Core::ResourceManager::LoadTexture("assets/textures/powerup_sticky.png", true, "powerup_sticky");
    Core::ResourceManager::LoadTexture("assets/textures/powerup_increase.png", true, "powerup_increase");
    Core::ResourceManager::LoadTexture("assets/textures/powerup_confuse.png", true, "powerup_confuse");
    Core::ResourceManager::LoadTexture("assets/textures/powerup_chaos.png", true, "powerup_chaos");
    Core::ResourceManager::LoadTexture("assets/textures/powerup_passthrough.png", true, "powerup_passthrough");
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
    // load effects
    Effects = new Core::PostProcessor(Core::ResourceManager::GetShader("postprocessing"), this->Width, this->Height);
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

bool ShouldSpawn(unsigned int chance)
{
    unsigned int random = rand() % chance;
    return random == 0;
}

void ActivatePowerUp(Engine::PowerUp &powerUp)
{
    if (powerUp.Type == "speed")
    {
        Ball->Velocity *= 1.2;
    }
    else if (powerUp.Type == "sticky")
    {
        Ball->Sticky = true;
        
        Player->Color = glm::vec3(1.0f, 0.5f, 1.0f);
    }
    else if (powerUp.Type == "pass-through")
    {
        Ball->PassThrough = true;
        Ball->Color = glm::vec3(1.0f, 0.5f, 0.5f);
    }
    else if (powerUp.Type == "pad-size-increase")
    {
        Player->Size.x += 50;
    }
    else if (powerUp.Type == "confuse")
    {   // only activate if chaos wasn't already active
        if (!Effects->Chaos) Effects->Confuse = true;
    }
    else if (powerUp.Type == "chaos")
    {
        if (!Effects->Confuse) Effects->Chaos = true;
    }
}

bool isOtherPowerUpActive(std::vector<Engine::PowerUp> &powerUps, std::string type)
{
    for (const Engine::PowerUp &powerUp : powerUps)
    {
        if (powerUp.Activated && powerUp.Type == type) return true;
    }
    return false;
}


void Game::Update(float dt)
{
    Ball->Move(dt, this->Width);
    this->DoCollisions();
    Particles->Update(dt, *Ball, 2, glm::vec2(Ball->Radius / 2.0f));
    this->UpdatePowerUps(dt);
    if (Ball->Position.y >= this->Height)
    {
        this->ResetLevel();
        this->ResetPlayer();
    }
    if (ShakeTime > 0.0f)
    {
        ShakeTime -= dt;
        if (ShakeTime <= 0.0f)
        {
            Effects->Shake = false;
        }
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
        Effects->BeginRender();
        // draw background
        Core::Texture2D background = Core::ResourceManager::GetTexture("background");
        Renderer->DrawSprite(background, glm::vec2(0.0f, 0.0f), 
            glm::vec2(this->Width, this->Height), 0.0f);
        // draw level
        this->Levels[this->Level].Draw(*Renderer);
        // draw player paddle
        Player->Draw(*Renderer);
        // draw powerups
        for (Engine::PowerUp &powerUp : this->PowerUps)
        {
            if (!powerUp.Destroyed) powerUp.Draw(*Renderer);
        }
        // draw ball
        Particles->Draw();
        Ball->Draw(*Renderer);
        Effects->EndRender();
        Effects->Render(glfwGetTime());
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
            this->SpawnPowerUps(box);
        }
        else
        {   // if block is solid, enable shake effect
            ShakeTime = 0.05f;
            Effects->Shake = true;
        }
        // collision resolution
        Direction dir = std::get<1>(collision);
        glm::vec2 diff_vector = std::get<2>(collision);
        if (!Ball->PassThrough || box.IsSolid)
        {
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
    }
    // check player collision
    cc::Collision result = cc::CheckCollision(*Ball, *Player);
    if (!Ball->Stuck && std::get<0>(result)) 
    {
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
        Ball->Stuck = Ball->Sticky;
    }
    // check powerup collision
    for (Engine::PowerUp &powerUp : this->PowerUps)
    {
        if (powerUp.Destroyed) continue;
        if (powerUp.Position.y >= this->Height)
        {
            powerUp.Destroyed = true;
        }
        if (cc::CheckCollision(*Player, powerUp))
        {   // collided with player, now activate powerup
            ActivatePowerUp(powerUp);
            powerUp.Destroyed = true;
            powerUp.Activated = true;
        }
    }
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

void Game::SpawnPowerUps(Engine::GameObject &block)
{
    if (ShouldSpawn(75)) // 1 in 75 chance
    {
        this->PowerUps.push_back(Engine::PowerUp("speed", glm::vec3(0.5f, 0.5f, 1.0f), 0.0f, block.Position, Core::ResourceManager::GetTexture("powerup_speed")));
    }
    if (ShouldSpawn(75))
    {
        this->PowerUps.push_back(Engine::PowerUp("sticky", glm::vec3(1.0f, 0.5f, 1.0f), 20.0f, block.Position, Core::ResourceManager::GetTexture("powerup_sticky")));
    }
    if (ShouldSpawn(75))
    {
        this->PowerUps.push_back(Engine::PowerUp("pass-through", glm::vec3(0.5f, 1.0f, 0.5f), 10.0f, block.Position, Core::ResourceManager::GetTexture("powerup_passthrough")));
    }
    if (ShouldSpawn(75))
    {
        this->PowerUps.push_back(Engine::PowerUp("pad-size-increase", glm::vec3(1.0f, 0.6f, 0.4f), 0.0f, block.Position, Core::ResourceManager::GetTexture("powerup_increase")));
    }
    if (ShouldSpawn(15)) // Negative powerups should spawn more often
    {
        this->PowerUps.push_back(Engine::PowerUp("confuse", glm::vec3(1.0f, 0.3f, 0.3f), 15.0f, block.Position, Core::ResourceManager::GetTexture("powerup_confuse")));    
    }
    if (ShouldSpawn(15))
    {
        this->PowerUps.push_back(Engine::PowerUp("chaos", glm::vec3(0.9f, 0.25f, 0.25f), 15.0f, block.Position, Core::ResourceManager::GetTexture("powerup_chaos")));
    }
}

void Game::UpdatePowerUps(float dt)
{
    for (Engine::PowerUp &powerUp : this->PowerUps)
    {
        powerUp.Position += powerUp.Velocity * dt;
        if (!powerUp.Activated) continue;
        powerUp.Duration -= dt;
        if (powerUp.Duration > 0.0f) continue;
        // remove powerup from list (will later be removed)
        powerUp.Activated = false;
        // deactivate effects
        if (powerUp.Type == "sticky")
        {
            if (!isOtherPowerUpActive(this->PowerUps, "sticky"))
            { // only reset if no other PowerUp of type sticky is active
                Ball->Sticky = false;
                Player->Color = glm::vec3(1.0f);
            }
        }
        else if (powerUp.Type == "pass-through")
        {
            if (!isOtherPowerUpActive(this->PowerUps, "pass-through"))
            {   // only reset if no other PowerUp of type pass-through is active
                Ball->PassThrough = false;
                Ball->Color = glm::vec3(1.0f);
            }
        }
        else if (powerUp.Type == "confuse")
        {
            if (!isOtherPowerUpActive(this->PowerUps, "confuse"))
            {   // only reset if no other PowerUp of type confuse is active
                Effects->Confuse = false;
            }
        }
        else if (powerUp.Type == "chaos")
        {
            if (!isOtherPowerUpActive(this->PowerUps, "chaos"))
            {   // only reset if no other PowerUp of type chaos is active
                Effects->Chaos = false;
            }
        }
    }
    this->PowerUps.erase(std::remove_if(this->PowerUps.begin(), this->PowerUps.end(),
        [](const Engine::PowerUp &powerUp) { return powerUp.Destroyed && !powerUp.Activated; }
    ), this->PowerUps.end());
}
