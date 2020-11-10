#pragma once

#include <optional>
#include <vector>
#include "Graphics/Color.h"
#include "Graphics/Light.h"
#include "Graphics/Material.h"
#include "Math/Vector3.h"

namespace GRAPHICS
{
    /// Encapsulates domain knowledge for shading algorithms.
    class Shading
    {
    public:
        static Color Compute(
            const MATH::Vector3f& world_vertex,
            const MATH::Vector3f& unit_vertex_normal,
            const Color& base_vertex_color,
            const Material& material,
            const MATH::Vector3f& viewing_world_position,
            const std::optional<std::vector<Light>>& lights);
    };
}
