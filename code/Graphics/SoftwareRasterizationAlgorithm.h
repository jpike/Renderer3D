#pragma once

#include <optional>
#include <vector>
#include "Graphics/Bitmap.h"
#include "Graphics/Camera.h"
#include "Graphics/Gui/Text.h"
#include "Graphics/Light.h"
#include "Graphics/Scene.h"
#include "Graphics/ScreenSpaceTriangle.h"

namespace GRAPHICS
{
    /// A software (non-graphics hardware) rasterization algorithm
    /// (http://en.wikipedia.org/wiki/Rasterisation) for rendering.
    struct SoftwareRasterizationAlgorithm
    {
        static void Render(const GUI::Text& text, Bitmap& render_target);

        static void Render(const Scene& scene, const Camera& camera, const bool cull_backfaces, Bitmap& output_bitmap);
        static void Render(const Object3D& object_3D, const std::vector<Light>& lights, const Camera& camera, const bool cull_backfaces, Bitmap& output_bitmap);

        static Triangle TransformLocalToWorld(const Triangle& local_triangle, const MATH::Matrix4x4f& world_transform);
        static std::optional<ScreenSpaceTriangle> TransformWorldToScreen(
            const Triangle& world_triangle,
            const Camera& camera,
            const MATH::Matrix4x4f& camera_view_transform,
            const MATH::Matrix4x4f& camera_projection_transform,
            const MATH::Matrix4x4f& screen_transform);

        static Color ComputeLighting(
            const MATH::Vector3f& world_vertex,
            const MATH::Vector3f& unit_vertex_normal,
            const Material& material,
            const Camera& camera,
            const std::vector<Light>& lights);

        static void Render(const ScreenSpaceTriangle& triangle, Bitmap& render_target);

        static void DrawLine(
            const float start_x,
            const float start_y,
            const float end_x,
            const float end_y,
            const Color& color,
            Bitmap& render_target);
        static void DrawLineWithInterpolatedColor(
            const float start_x,
            const float start_y,
            const float end_x,
            const float end_y,
            const Color& start_color,
            const Color& end_color,
            Bitmap& render_target);
    };
}
