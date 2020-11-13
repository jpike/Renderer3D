#pragma once

#include <memory>
#include <gl/GL.h>

namespace GRAPHICS::OPEN_GL
{
    /// An OpenGL shader program.
    class ShaderProgram
    {
    public:
        // CONSTRUCTION/DESTRUCTION.
        static std::shared_ptr<ShaderProgram> Build(
            const char* const vertex_shader_code,
            const char* const fragment_shader_code);
        ~ShaderProgram();

        // PUBLIC MEMBER VARIABLES FOR EASY ACCESS.
        /// The OpenGL ID for the program (if it's an OpenGL program).
        GLuint Id = 0;
    };
}
