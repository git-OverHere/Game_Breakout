#pragma once

#include "Core/Shader.h"
#include "Core/Texture2D.h"

// PostProcessor hosts all PostProcessing effects. 
// It renders the game on a textured quad after which one can
// enable specific effects by enabling either the Confuse, Chaos or 
// Shake boolean. 
// It is required to call BeginRender() before rendering the game
// and EndRender() after rendering the game for the class to work.
namespace Core
{
    class PostProcessor
    {
    public:
        // state
        Core::Shader PostProcessingShader;
        Core::Texture2D Texture;
        unsigned int Width, Height;
        // options
        bool Confuse, Chaos, Shake;
        // constructor
        PostProcessor(Core::Shader shader, unsigned int width, unsigned int height);
        // prepares the postprocessor's framebuffer operations before rendering the game
        void BeginRender();
        // should be called after rendering the game, so it stores all the rendered data into a texture object
        void EndRender();
        // renders the PostProcessor texxture quad (as a screen-encompassing large sprite)
        void Render(float time);
    private:
        // render state
        unsigned int MSFBO, FBO; // MSFBO = Multisampled FBO. FBO is regular, used for blitting MS color-buffer to texture
        unsigned int RBO; // RBO is used for multisampled color buffer
        unsigned int VAO;
        // initialize quad for rendering postprocessing texture
        void initRenderData();
    };
};