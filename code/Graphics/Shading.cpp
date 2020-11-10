#include "Graphics/Lighting.h"
#include "Graphics/Shading.h"

namespace GRAPHICS
{
    /// Computes shading for a vertex.
    /// @param[in]  world_vertex - The world space vertex for which to compute lighting.
    /// @param[in]  unit_vertex_normal - The unit surface normal for the vertex.
    /// @param[in]  base_vertex_color - The base color of the vertex.
    /// @param[in]  material - The material for the vertex.
    /// @param[in]  viewing_world_position - The world position from where the vertex is being viewed.
    /// @param[in]  lights - The lights potentially shining on the vertex.
    ///     An unpopulated optional means lighting shouldn't be computed for shading
    ///     (shading just comes from vertex colors).
    ///     A populated optional with an empty lists means to compute lighting as if
    ///     there are no lights in the scene.
    /// @return The computed shading for the vertex.
    Color Shading::Compute(
        const MATH::Vector3f& world_vertex,
        const MATH::Vector3f& unit_vertex_normal,
        const Color& base_vertex_color,
        const Material& material,
        const MATH::Vector3f& viewing_world_position,
        const std::optional<std::vector<Light>>& lights)
    {
        // COMPUTE COLOR FROM LIGHTS IF APPLICABLE.
        Color light_color = Color::WHITE;
        if (lights)
        {
            light_color = Lighting::Compute(
                world_vertex,
                unit_vertex_normal,
                material,
                viewing_world_position,
                *lights);
        }

        // COMBINE LIGHTING WITH THE BASE VERTEX COLOR.
        Color final_vertex_color = Color::ComponentMultiplyRedGreenBlue(base_vertex_color, light_color);
        final_vertex_color.Clamp();
        return final_vertex_color;
    }
}
