#include "Graphics/SoftwareRasterizationAlgorithm.h"

namespace GRAPHICS
{
    /// Renders an entire 3D scene.
    /// @param[in]  scene - The scene to render.
    /// @param[in]  camera - The camera to use to view the scene.
    /// @param[in,out]  output_bitmap - The bitmap to render to.
    void SoftwareRasterizationAlgorithm::Render(const Scene& scene, const Camera& camera, Bitmap& output_bitmap) const
    {
        // CLEAR THE BACKGROUND.
        output_bitmap.FillPixels(scene.BackgroundColor);

        // RENDER EACH OBJECT IN THE SCENE.
        for (const auto& object_3D : scene.Objects)
        {
            Render(object_3D, scene.PointLights, camera, output_bitmap);
        }
    }

    /// Renders a 3D object to the render target.
    /// @param[in]  object_3D - The object to render.
    /// @param[in]  lights - Any lights that should illuminate the object.
    /// @param[in]  camera - The camera to use to view the object.
    /// @param[in,out]  output_bitmap - The bitmap to render to.
    void SoftwareRasterizationAlgorithm::Render(const Object3D& object_3D, const std::vector<Light>& lights, const Camera& camera, Bitmap& output_bitmap) const
    {
        // GET THE OBJECT'S WORLD TRANSFORMATION MATRIX.
        // This is done before the loop to avoid performance hits for repeatedly calculating it.
        MATH::Matrix4x4f object_world_transform = object_3D.WorldTransform();

        // RENDER EACH TRIANGLE OF THE OBJECT.
        for (const auto& local_triangle : object_3D.Triangles)
        {
            // TRANSFORM THE TRIANGLE INTO WORLD SPACE.
            Triangle world_space_triangle = local_triangle;
            std::size_t triangle_vertex_count = world_space_triangle.Vertices.size();
            for (std::size_t vertex_index = 0; vertex_index < triangle_vertex_count; ++vertex_index)
            {
                // TRANFORM THE CURRENT LOCAL VERTEX INTO WORLD SPACE.
                const MATH::Vector3f& local_vertex = local_triangle.Vertices[vertex_index];
                MATH::Vector4f local_homogeneous_vertex = MATH::Vector4f::HomogeneousPositionVector(local_vertex);

                /// @todo   More unit testing!
                MATH::Vector4f world_homogeneous_vertex = object_world_transform * local_homogeneous_vertex;
                MATH::Vector3f& world_vertex = world_space_triangle.Vertices[vertex_index];
                world_vertex = MATH::Vector3f(world_homogeneous_vertex.X, world_homogeneous_vertex.Y, world_homogeneous_vertex.Z);
            }

            /// @todo   Clipping against camera z-boundaries!

            /// @todo   Surface normal!

            // TRANSFORM THE TRIANGLE FOR PROPER CAMERA VIEWING.

            /// @todo   Render screen-space triangle!
        }
    }
}
