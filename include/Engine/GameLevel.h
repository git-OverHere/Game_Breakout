#include "Engine/GameObject.h"
#include "Core/SpriteRenderer.h"

namespace Engine
{
    class GameLevel
    {
    public:
        // level state
        std::vector<GameObject> Bricks;
        // constructor
        GameLevel() { }
        // loads level from file
        void Load(const char *file, unsigned int levelWidth, unsigned int levelHeight);
        // render level
        void Draw(Core::SpriteRenderer &renderer);
        // check if the level is completed (all non-solid tiles are destroyed)
        bool IsCompleted();
    private:
        // initialize level from tile data
        void init(  std::vector<std::vector<unsigned int>> tileData,
                    unsigned int levelWidth, unsigned int levelHeight);
    };
};