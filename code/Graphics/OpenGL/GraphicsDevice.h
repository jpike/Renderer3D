#pragma once

#include <memory>
#include <vector>
#include <gl/GL.h>
#include <Windows.h>
#include "Graphics/OpenGL/OpenGL.h"

namespace GRAPHICS
{
namespace OPEN_GL
{
    /// Represents a device for rendering graphics using OpenGL.
    /// @note   Currently only supports up to OpenGL 4.2.0.
    class GraphicsDevice
    {
    public:
        // CONSTRUCTION.
        static std::shared_ptr<GraphicsDevice> Create(const HDC device_context);
        explicit GraphicsDevice(const HDC device_context, const HGLRC open_gl_render_context);
        ~GraphicsDevice();

        // PUBLIC MEMBER VARIABLES FOR EASY ACCESS.
        /// The regular Windows device context.
        HDC DeviceContext;

    private:
        // MEMBER VARIABLES.
        /// The OpenGL rendering context.
        HGLRC OpenGLRenderContext;
    };
}
}
