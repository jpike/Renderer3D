#pragma once

#include "Graphics/Color.h"
#include "Graphics/Object3D.h"

namespace GRAPHICS::OPEN_GL
{
    /// A renderer using OpenGL.
    class OpenGLRenderer
    {
    public:
        void ClearScreen(const Color& color);
        void Render(const Object3D& object_3D);
    };
}
