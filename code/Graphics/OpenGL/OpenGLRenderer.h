#pragma once

#include "Graphics/Camera.h"
#include "Graphics/Color.h"
#include "Graphics/Object3D.h"
#include "Graphics/Scene.h"

namespace GRAPHICS::OPEN_GL
{
    /// A renderer using OpenGL.
    class OpenGLRenderer
    {
    public:
        void Render(const Scene& scene, const Camera& camera, int primitive_type, unsigned int first_primitive_offset, unsigned int primitive_count) const;

        void ClearScreen(const Color& color) const;
        void Render(const Object3D& object_3D, int primitive_type, unsigned int first_primitive_offset, unsigned int primitive_count) const;
    };
}
