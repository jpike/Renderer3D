#pragma once

#include <array>
#include <vector>
#include "Graphics/Camera.h"
#include "Graphics/Color.h"
#include "Graphics/Gui/Text.h"
#include "Graphics/Light.h"
#include "Graphics/Object3D.h"
#include "Graphics/RenderTarget.h"
#include "Graphics/Scene.h"
#include "Graphics/Triangle.h"

namespace GRAPHICS
{
    /// A software renderer.
    class Renderer
    {
    public:
        // RENDERING.
        void Render(const GUI::Text& text, RenderTarget& render_target) const;
        void Render(const Scene& scene, const Camera& camera, RenderTarget& render_target) const;
        void Render(const Object3D& object_3D, const std::vector<Light>& lights, const Camera& camera, RenderTarget& render_target) const;

    private:
        // RENDERING.
        void Render(const Triangle& triangle, const std::array<GRAPHICS::Color, Triangle::VERTEX_COUNT>& triangle_vertex_colors, RenderTarget& render_target) const;

        void DrawLine(
            const float start_x,
            const float start_y,
            const float end_x,
            const float end_y,
            const Color& color,
            RenderTarget& render_target) const;
        void DrawLineWithInterpolatedColor(
            const float start_x,
            const float start_y,
            const float end_x,
            const float end_y,
            const Color& start_color,
            const Color& end_color,
            RenderTarget& render_target) const;
    };
}
