#include "Engine/GameLevel.h"

#include <string>
#include <fstream>
#include <sstream>

#include "Engine/GameObject.h"
#include "Core/ResourceManager.h"

using namespace Engine;

void GameLevel::Load(
    const char *file, 
    unsigned int levelWidth, 
    unsigned int levelHeight)
{
    // clear old data
    this->Bricks.clear();
    // load from file
    unsigned int tileCode;
    std::string line;
    std::ifstream fstream(file);
    std::vector<std::vector<unsigned int>> tileData;
    if (!fstream) return;

    while (std::getline(fstream, line)) // read each line from level file
    {
        std::istringstream sstream(line);
        std::vector<unsigned int> row;
        while (sstream >> tileCode) // reac each word separated by spaces
        {
            row.push_back(tileCode);
        }
        tileData.push_back(row);
    }

    if (tileData.size() > 0)
    {
        this->init(tileData, levelWidth, levelHeight);
    }
}

void GameLevel::Draw(Core::SpriteRenderer &renderer)
{
    for (GameObject &tile : this->Bricks)
        if (!tile.Destroyed)
            tile.Draw(renderer);
}

bool GameLevel::IsCompleted()
{
    for (GameObject &tile : this->Bricks)
        if (!tile.IsSolid && !tile.Destroyed)
            return false;
    return true;
}

void GameLevel::init(std::vector<std::vector<unsigned int>> tileData,
        unsigned int levelWidth, unsigned int levelHeight)
{
    // calculate dimenstions
    unsigned int height = tileData.size();
    unsigned int width = tileData[0].size();
    float unit_width = levelWidth / static_cast<float>(width);
    float unit_height = levelHeight / height;
    // initialize level tiles based on tileData
    for (unsigned int y = 0; y < height; ++y)
    {
        for (unsigned int x = 0; x < width; ++x)
        {
            unsigned int tile = tileData[y][x];
            // check block type from level data (2D level array)
            if (tile == 1) // solid
            {
                glm::vec2 pos(unit_width * x, unit_height * y);
                glm::vec2 size(unit_width, unit_height);
                GameObject obj(pos, size,
                    Core::ResourceManager::GetTexture("block_solid"),
                    glm::vec3(0.8f, 0.8f, 0.7f) // light beige
                );
                obj.IsSolid = true;
                this->Bricks.push_back(obj);
            } 
            else if (tile > 1)
            {
                glm::vec3 color = glm::vec3(1.0f); // original: white
                if (tile == 2)
                    color = glm::vec3(0.2f, 0.6f, 1.0f); // sky blue
                else if (tile == 3)
                    color = glm::vec3(0.0f, 0.7f, 0.0f); // light green
                else if (tile == 4)
                    color = glm::vec3(0.8f, 0.8f, 0.4f); // olive yellow
                else if (tile == 5)
                    color = glm::vec3(1.0f, 0.5f, 0.0f); // orange

                glm::vec2 pos(unit_width * x, unit_height * y);
                glm::vec2 size(unit_width, unit_height);
                this->Bricks.push_back(
                    GameObject(pos, size, 
                        Core::ResourceManager::GetTexture("block"), color)
                );
            }
        }
    }
}

