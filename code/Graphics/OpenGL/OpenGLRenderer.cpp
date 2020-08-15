#include <gl/GL.h>
#include "Graphics/OpenGL/OpenGLRenderer.h"

namespace GRAPHICS::OPEN_GL
{
    /// Renders an entire 3D scene.
    /// @param[in]  scene - The scene to render.
    /// @param[in]  camera - The camera to use to view the scene.
    void OpenGLRenderer::Render(const Scene& scene, const Camera& camera) const
    {
        ClearScreen(scene.BackgroundColor);

        // RENDER EACH OBJECT IN THE SCENE.
        for (const auto& object_3D : scene.Objects)
        {
            /// @todo
            camera;
            Render(object_3D);
        }
    }

    /// Clears the screen to the specified color.
    /// @param[in]  color - The color to clear to.
    void OpenGLRenderer::ClearScreen(const Color& color) const
    {
        glClearColor(color.Red, color.Green, color.Blue, color.Alpha);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    /// Renders the specified object.
    /// @param[in]  object_3D - The 3D object to render.
    void OpenGLRenderer::Render(const Object3D& object_3D) const
    {
        glBegin(GL_TRIANGLES);
        for (const auto& triangle : object_3D.Triangles)
        {
            glColor3f(1.0f, 1.0f, 1.0f);
            for (const auto& vertex : triangle.Vertices)
            {
                glVertex3f(vertex.X, vertex.Y, vertex.Z);
            }
        }
        glEnd();
    }
}
