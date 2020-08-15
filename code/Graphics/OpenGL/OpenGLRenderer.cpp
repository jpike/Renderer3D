#include <gl/GL.h>
#include <gl/GLU.h>
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
        else if (ProjectionType::PERSPECTIVE == camera.Projection)
        {
            const MATH::Angle<float>::Degrees VERTICAL_FIELD_OF_VIEW_IN_DEGREES(90.0f);
            const float ASPECT_RATIO_WIDTH_OVER_HEIGHT = 1.0f;
            const float NEAR_Z_WORLD_BOUNDARY = 1.0f;
            const float FAR_Z_WORLD_BOUNDARY = 500.0f;
            gluPerspective(VERTICAL_FIELD_OF_VIEW_IN_DEGREES.Value, ASPECT_RATIO_WIDTH_OVER_HEIGHT, NEAR_Z_WORLD_BOUNDARY, FAR_Z_WORLD_BOUNDARY);
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
            for (std::size_t vertex_index = 0; vertex_index < triangle.Vertices.size(); ++vertex_index)
            {
                const auto& vertex = triangle.Vertices[vertex_index];

                switch (triangle.Material->Shading)
                {
                    case ShadingType::WIREFRAME:
                    {
                        glColor3f(
                            triangle.Material->WireframeColor.Red,
                            triangle.Material->WireframeColor.Green,
                            triangle.Material->WireframeColor.Blue);
                        break;
                    }
                    case ShadingType::WIREFRAME_VERTEX_COLOR_INTERPOLATION:
                    {
                        const Color& vertex_color = triangle.Material->VertexWireframeColors[vertex_index];
                        glColor3f(
                            vertex_color.Red,
                            vertex_color.Green,
                            vertex_color.Blue);
                        break;
                    }
                    case ShadingType::FLAT:
                    {
                        glColor3f(
                            triangle.Material->FaceColor.Red,
                            triangle.Material->FaceColor.Green,
                            triangle.Material->FaceColor.Blue);
                        break;
                    }
                    case ShadingType::FACE_VERTEX_COLOR_INTERPOLATION:
                    {
                        const Color& vertex_color = triangle.Material->VertexFaceColors[vertex_index];
                        glColor3f(
                            vertex_color.Red,
                            vertex_color.Green,
                            vertex_color.Blue);
                        break;
                    }
                    case ShadingType::GOURAUD:
                    {
                        const Color& vertex_color = triangle.Material->VertexColors[vertex_index];
                        glColor3f(
                            vertex_color.Red,
                            vertex_color.Green,
                            vertex_color.Blue);
                        break;
                    }
                    case ShadingType::TEXTURED:
                    {
                        /// @todo
                        glColor3f(1.0f, 1.0f, 1.0f);
                        break;
                    }
                    case ShadingType::MATERIAL:
                    {
                        /// @todo
                        glColor3f(0.5f, 0.5f, 0.5f);
                        break;
                    }
                }

                glVertex3f(vertex.X, vertex.Y, vertex.Z);
            }
        }
        glEnd();
    }
}
