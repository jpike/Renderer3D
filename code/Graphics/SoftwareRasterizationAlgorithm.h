#pragma once

#include <optional>
#include <vector>
#include "Graphics/Bitmap.h"
#include "Graphics/Camera.h"
#include "Graphics/DepthBuffer.h"
#include "Graphics/Gui/Text.h"
#include "Graphics/Light.h"
#include "Graphics/Scene.h"
#include "Graphics/ScreenSpaceTriangle.h"

namespace GRAPHICS
{
    /// A software (non-graphics hardware) rasterization algorithm
    /// (http://en.wikipedia.org/wiki/Rasterisation) for rendering.
    /// @todo   Alpha blending
    class SoftwareRasterizationAlgorithm
    {
    public:
        static void Render(const GUI::Text& text, Bitmap& render_target);

        static void Render(
            const Scene& scene, 
            const Camera& camera, 
            const bool cull_backfaces, 
            Bitmap& output_bitmap,
            DepthBuffer* depth_buffer);
        static void Render(
            const Object3D& object_3D, 
            const std::optional<std::vector<Light>>& lights, 
            const Camera& camera, 
            const bool cull_backfaces, 
            Bitmap& output_bitmap,
            DepthBuffer* depth_buffer);

        static Triangle TransformLocalToWorld(const Triangle& local_triangle, const MATH::Matrix4x4f& world_transform);

        static void Render(
            const ScreenSpaceTriangle& triangle, 
            Bitmap& render_target,
            DepthBuffer* depth_buffer);

        static void DrawLine(
            const MATH::Vector3f& start_vertex,
            const MATH::Vector3f& end_vertex,
            const Color& color,
            Bitmap& render_target,
            DepthBuffer* depth_buffer);
        static void DrawLineWithInterpolatedColor(
            const MATH::Vector3f& start_vertex,
            const MATH::Vector3f& end_vertex,
            const Color& start_color,
            const Color& end_color,
            Bitmap& render_target,
            DepthBuffer* depth_buffer);
    };
}
