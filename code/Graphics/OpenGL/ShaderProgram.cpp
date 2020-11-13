#include <GL/gl3w.h>
#include "Graphics/OpenGL/ShaderProgram.h"

namespace GRAPHICS::OPEN_GL
{
    void CheckShaderCompilation(const GLuint shader_id)
    {
        GLint shader_compile_status;
        glGetShaderiv(shader_id, GL_COMPILE_STATUS, &shader_compile_status);
        bool shader_compiled = (GL_TRUE == shader_compile_status);
        if (!shader_compiled)
        {
            // GET THE SHADER COMPILER ERROR.
            const GLsizei LEGNTH_OF_LOG_NOT_NEEDED = NULL;
            char shader_compile_log_buffer[512];
            glGetShaderInfoLog(
                shader_id,
                sizeof(shader_compile_log_buffer) / sizeof(shader_compile_log_buffer[0]),
                LEGNTH_OF_LOG_NOT_NEEDED,
                shader_compile_log_buffer);
            /// @todo   Log via a better mechanism.
            OutputDebugString("Shader compile error: ");
            OutputDebugString(shader_compile_log_buffer);
        }
    }

    /// Attempts to build a shader program.
    /// @param[in]  vertex_shader_code - The vertex shader code.
    /// @param[in]  fragment_shader_code - The fragment shader code.
    /// @return The built shader program; null if an error occurs.
    /// @todo   Error handling for failures?
    std::shared_ptr<ShaderProgram> ShaderProgram::Build(
        const char* const vertex_shader_code,
        const char* const fragment_shader_code)
    {
        // COMPILE THE VERTEX SHADER.
        GLuint vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
        const GLsizei SINGLE_SOURCE_CODE_STRING = 1;
        const GLint* const SOURCE_CODE_IS_NULL_TERMINATED = NULL;
        glShaderSource(vertex_shader_id, SINGLE_SOURCE_CODE_STRING, &vertex_shader_code, SOURCE_CODE_IS_NULL_TERMINATED);
        glCompileShader(vertex_shader_id);
        CheckShaderCompilation(vertex_shader_id);

        // COMPILE THE FRAGMENT SHADER.
        GLuint fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment_shader_id, SINGLE_SOURCE_CODE_STRING, &fragment_shader_code, SOURCE_CODE_IS_NULL_TERMINATED);
        glCompileShader(fragment_shader_id);
        CheckShaderCompilation(fragment_shader_id);

        // CREATE THE COMBINED PROGRAM.
        auto shader_program = std::make_shared<ShaderProgram>();
        shader_program->Id = glCreateProgram();
        glAttachShader(shader_program->Id, vertex_shader_id);
        glAttachShader(shader_program->Id, fragment_shader_id);
        glLinkProgram(shader_program->Id);

        // FREE RESOURCES NO LONGER NEEDED.
        // They're stored with the shader program now.
        glDeleteShader(fragment_shader_id);
        glDeleteShader(vertex_shader_id);

        return shader_program;
    }

    /// Destructor to free shader program resources.
    /// @todo   Should this really be done automatically as opposed to
    ///     explicitly via the graphics device?
    ShaderProgram::~ShaderProgram()
    {
        glDeleteProgram(Id);
    }
}
