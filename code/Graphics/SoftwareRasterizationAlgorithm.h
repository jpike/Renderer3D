#pragma once

#include <vector>
#include "Graphics/Bitmap.h"
#include "Graphics/Camera.h"
#include "Graphics/Gui/Text.h"
#include "Graphics/Light.h"
#include "Graphics/Scene.h"

namespace GRAPHICS
{
    /// A software (non-graphics hardware) rasterization algorithm
    /// (http://en.wikipedia.org/wiki/Rasterisation) for rendering.
    struct SoftwareRasterizationAlgorithm
    {
        static void Render(const GUI::Text& text, Bitmap& render_target);

        static void Render(const Scene& scene, const Camera& camera, Bitmap& output_bitmap);
        static void Render(const Object3D& object_3D, const std::vector<Light>& lights, const Camera& camera, Bitmap& output_bitmap);

        static void Render(const Triangle& triangle, const std::array<GRAPHICS::Color, Triangle::VERTEX_COUNT>& triangle_vertex_colors, Bitmap& render_target);

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
