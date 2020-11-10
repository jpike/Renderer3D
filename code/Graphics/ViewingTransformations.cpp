#include "Graphics/ViewingTransformations.h"

namespace GRAPHICS
{
    /// Creates viewing transformations for the specified parameters.
    /// @param[in]  camera - The camera used for viewing.
    /// @param[in]  output_plane - The 2D plane onto which the final image will be viewed.
    ViewingTransformations::ViewingTransformations(const Camera& camera, const Bitmap& output_plane)
    {
        // INITIALIZE PROPERTIES FROM THE CAMERA.
        CameraViewTransform = camera.ViewTransform();
        CameraProjectionTransform = camera.ProjectionTransform();
        CameraNearClipPlaneViewDistance = camera.NearClipPlaneViewDistance;
        CameraFarClipPlaneViewDistance = camera.FarClipPlaneViewDistance;

        // INITIALIZE THE SCREEN TRANSFORM.
        MATH::Matrix4x4f flip_y_transform = MATH::Matrix4x4f::Scale(MATH::Vector3f(1.0f, -1.0f, 1.0f));
        MATH::Matrix4x4f scale_to_screen_transform = MATH::Matrix4x4f::Scale(MATH::Vector3f(
            static_cast<float>(output_plane.GetWidthInPixels()) / 2.0f,
            static_cast<float>(output_plane.GetHeightInPixels()) / 2.0f,
            1.0f));
        MATH::Matrix4x4f translate_to_screen_center_transform = MATH::Matrix4x4f::Translation(MATH::Vector3f(
            static_cast<float>(output_plane.GetWidthInPixels()) / 2.0f,
            static_cast<float>(output_plane.GetHeightInPixels()) / 2.0f,
            0.0f));
        ScreenTransform = translate_to_screen_center_transform * scale_to_screen_transform * flip_y_transform;
    }

    /// Applies the viewing transformations to transform a triangle from world space to screen space.
    /// @param[in]  world_triangle - The world triangle to transform.
    /// @return The screen-space triangle, if within view; null otherwise.
    std::optional<ScreenSpaceTriangle> ViewingTransformations::Apply(const Triangle& world_triangle) const
    {
        // CREATE THE INITIAL SCREEN SPACE TRIANGLE.
        // Vertex colors will be populated later.
        ScreenSpaceTriangle screen_space_triangle =
        {
            .Material = world_triangle.Material,
            .VertexPositions = world_triangle.Vertices,
            .VertexColors = {}
        };

        // TRANSFORM EACH VERTEX.
        std::size_t triangle_vertex_count = world_triangle.Vertices.size();
        for (std::size_t vertex_index = 0; vertex_index < triangle_vertex_count; ++vertex_index)
        {
            // TRANSFORM THE WORLD VERTEX INTO VIEW OF THE CAMERA.
            const MATH::Vector3f& world_vertex = world_triangle.Vertices[vertex_index];
            MATH::Vector4f world_homogeneous_vertex = MATH::Vector4f::HomogeneousPositionVector(world_vertex);
            MATH::Vector4f view_vertex = CameraViewTransform * world_homogeneous_vertex;

            // MAKE SURE THE VERTEX FALLS WITHIN CLIP PLANES.
            // If not, we could get some odd projections (divide by zero, flipping, etc.) for triangles behind the camera.
            // This also saves on rendering budgets for triangles out-of-view.
            float near_z_boundary = -CameraNearClipPlaneViewDistance;
            float far_z_boundary = -CameraFarClipPlaneViewDistance;
            // "Direction" of >= comparisons is reversed due to being along negative Z axis.
            bool current_vertex_within_near_far_clip_planes = (near_z_boundary >= view_vertex.Z && view_vertex.Z >= far_z_boundary);
            if (!current_vertex_within_near_far_clip_planes)
            {
                // The triangle falls outside of the clipping range.
                return std::nullopt;
            }

            // PROJECT THE VERTEX.
            MATH::Vector4f projected_vertex = CameraProjectionTransform * view_vertex;
            // The vertex must be de-homogenized.
            MATH::Vector4f transformed_vertex = MATH::Vector4f::Scale(1.0f / projected_vertex.W, projected_vertex);

            // TRANSFORM THE VERTEX INTO SCREEN SPACE.
            MATH::Vector4f screen_space_vertex = ScreenTransform * transformed_vertex;
            screen_space_triangle.VertexPositions[vertex_index] = MATH::Vector3f(screen_space_vertex.X, screen_space_vertex.Y, screen_space_vertex.Z);
        }

        // RETURN THE SCREEN SPACE TRIANGLE.
        // If we didn't already return early above, then the triangle should be visible on screen.
        return screen_space_triangle;
    }
}
