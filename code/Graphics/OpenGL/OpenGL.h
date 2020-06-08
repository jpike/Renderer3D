#pragma once

#include <cstddef>
#include <memory>
#include <gl/GL.h>
#undef GL_VERSION_1_1
#include <GL/gl3w.h>
#include <GL/wglext.h>
#include <Windows.h>

namespace GRAPHICS
{
/// Holds code related to the OpenGL graphics API.
namespace OPEN_GL
{
    /// The value that marks the end of an OpenGL attribute list.
    constexpr int ATTRIBUTE_LIST_TERMINATOR = 0;

    /// The Initialize function should be called once to load OpenGL functions.
    bool Initialize(const HDC device_context);

    extern PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB;
    extern PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;
}
}
