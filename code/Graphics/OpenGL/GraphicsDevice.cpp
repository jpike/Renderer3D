#include "Graphics/OpenGL/GraphicsDevice.h"

namespace GRAPHICS
{
namespace OPEN_GL
{
    /// Attempts to crete a graphics device using OpenGL for the provided window.
    /// @param[in]  device_context - The handle to the device context for which the graphics device
    ///     should be created.
    /// @return The graphics device, if successfully created; null otherwise.
    std::shared_ptr<GraphicsDevice> GraphicsDevice::Create(const HDC device_context)
    {
        // CREATE THE OPEN GL RENDERING CONTEXT.
        const HGLRC NO_CONTEXT_TO_SHARE_WITH = nullptr;
        const int context_attribute_list[] =
        {
            WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
            WGL_CONTEXT_MINOR_VERSION_ARB, 2,
            WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB,
            WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
            ATTRIBUTE_LIST_TERMINATOR
        };
        HGLRC open_gl_render_context = wglCreateContextAttribsARB(device_context, NO_CONTEXT_TO_SHARE_WITH, context_attribute_list);
        BOOL open_gl_context_made_current = wglMakeCurrent(device_context, open_gl_render_context);
        if (!open_gl_context_made_current)
        {
            return nullptr;
        }

        MessageBoxA(0, (char*)glGetString(GL_VERSION), "OPENGL VERSION", 0);
        
        // CREATE THE GRAPHICS DEVICE.
        std::shared_ptr<GraphicsDevice> graphics_device = std::make_shared<GraphicsDevice>(
            device_context,
            open_gl_render_context);
        return graphics_device;
    }

    /// Constructor.
    /// @param[in]  device_context - The regular Windows device context.
    /// @param[in]  open_gl_render_context - The OpenGL rendering context.
    GraphicsDevice::GraphicsDevice(const HDC device_context, const HGLRC open_gl_render_context) :
        DeviceContext(device_context),
        OpenGLRenderContext(open_gl_render_context)
    {}

    /// Destructor that deletes resources and the OpenGL rendering context for the device.
    GraphicsDevice::~GraphicsDevice()
    {
        // DELETE THE RENDERING CONTEXT.
        wglDeleteContext(OpenGLRenderContext);
    }
}
}
