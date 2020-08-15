#include <gl/GL.h>
#include "Graphics/OpenGL/OpenGLRenderer.h"

namespace GRAPHICS::OPEN_GL
{
    /// Renders an entire 3D scene.
    /// @param[in]  scene - The scene to render.
    /// @param[in]  camera - The camera to use to view the scene.
    void OpenGLRenderer::Render(const Scene& scene, const Camera& camera) const
    {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        if (ProjectionType::ORTHOGRAPHIC == camera.Projection)
        {
            /// @todo   Centralize screen dimensions!
            glOrtho(
                camera.WorldPosition.X - 200.0f, 
                camera.WorldPosition.X + 200.0f,
                camera.WorldPosition.Y - 200.0f,
                camera.WorldPosition.Y + 200.0f,
                -1.0f,
                1.0f);
        }
        /// @todo   Perspective!
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        MATH::Matrix4x4f view_matrix = camera.ViewTransform();
        glLoadMatrixf(view_matrix.Elements.ValuesInColumnMajorOrder().data());

        ClearScreen(scene.BackgroundColor);

        // RENDER EACH OBJECT IN THE SCENE.
        for (const auto& object_3D : scene.Objects)
        {
            glPushMatrix();
            glLoadIdentity();
            MATH::Matrix4x4f world_matrix = object_3D.WorldTransform();
            glLoadMatrixf(world_matrix.Elements.ValuesInColumnMajorOrder().data());
            
            Render(object_3D);

            glPopMatrix();
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
