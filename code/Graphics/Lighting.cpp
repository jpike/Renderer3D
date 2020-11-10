#include "Graphics/Lighting.h"

namespace GRAPHICS
{
    /// Computes lighting for a vertex.
    /// @param[in]  world_vertex - The world space vertex for which to compute lighting.
    /// @param[in]  unit_vertex_normal - The unit surface normal for the vertex.
    /// @param[in]  material - The material for the vertex.
    /// @param[in]  viewing_world_position - The world position from where the vertex is being viewed.
    /// @param[in]  lights - The lights potentially shining on the vertex.
    /// @return The computed light color.
    Color Lighting::Compute(
        const MATH::Vector3f& world_vertex,
        const MATH::Vector3f& unit_vertex_normal,
        const Material& material,
        const MATH::Vector3f& viewing_world_position,
        const std::vector<Light>& lights)
    {
        Color light_total_color = Color::BLACK;
        for (const Light& light : lights)
        {
            // COMPUTE SHADING BASED ON TYPE OF LIGHT.
            if (LightType::AMBIENT == light.Type)
            {
                if (ShadingType::MATERIAL == material.Shading)
                {
                    light_total_color += Color::ComponentMultiplyRedGreenBlue(light.Color, material.AmbientColor);
                }
                else
                {
                    light_total_color += light.Color;
                }
            }
            else
            {
                // GET THE DIRECTION OF THE LIGHT.
                MATH::Vector3f current_world_vertex = MATH::Vector3f(world_vertex.X, world_vertex.Y, world_vertex.Z);
                MATH::Vector3f direction_from_vertex_to_light;
                if (LightType::DIRECTIONAL == light.Type)
                {
                    // The computations are based on the opposite direction.
                    direction_from_vertex_to_light = MATH::Vector3f::Scale(-1.0f, light.DirectionalLightDirection);
                }
                else if (LightType::POINT == light.Type)
                {
                    direction_from_vertex_to_light = light.PointLightWorldPosition - current_world_vertex;
                }

                // ADD DIFFUSE COLOR FROM THE CURRENT LIGHT.
                // This is based on the Lambertian shading model.
                // An object is maximally illuminated when facing toward the light.
                // An object tangent to the light direction or facing away receives no illumination.
                // In-between, the amount of illumination is proportional to the cosine of the angle between
                // the light and surface normal (where the cosine can be computed via the dot product).
                MATH::Vector3f unit_direction_from_point_to_light = MATH::Vector3f::Normalize(direction_from_vertex_to_light);
                constexpr float NO_ILLUMINATION = 0.0f;
                float illumination_proportion = MATH::Vector3f::DotProduct(unit_vertex_normal, unit_direction_from_point_to_light);
                illumination_proportion = std::max(NO_ILLUMINATION, illumination_proportion);
                Color current_light_color = Color::ScaleRedGreenBlue(illumination_proportion, light.Color);
                if (ShadingType::MATERIAL == material.Shading)
                {
                    light_total_color += Color::ComponentMultiplyRedGreenBlue(current_light_color, material.DiffuseColor);
                }
                else
                {
                    light_total_color += current_light_color;
                }

                // ADD SPECULAR COLOR FROM THE CURRENT LIGHT.
                /// @todo   Is this how we want to handle specularity?
                if (material.SpecularPower > 1.0f)
                {
                    MATH::Vector3f reflected_light_along_surface_normal = MATH::Vector3f::Scale(2.0f * illumination_proportion, unit_vertex_normal);
                    MATH::Vector3f reflected_light_direction = reflected_light_along_surface_normal - unit_direction_from_point_to_light;
                    MATH::Vector3f unit_reflected_light_direction = MATH::Vector3f::Normalize(reflected_light_direction);

                    MATH::Vector3f ray_from_vertex_to_camera = viewing_world_position - current_world_vertex;
                    MATH::Vector3f normalized_ray_from_vertex_to_camera = MATH::Vector3f::Normalize(ray_from_vertex_to_camera);
                    float specular_proportion = MATH::Vector3f::DotProduct(normalized_ray_from_vertex_to_camera, unit_reflected_light_direction);
                    specular_proportion = std::max(NO_ILLUMINATION, specular_proportion);
                    specular_proportion = std::pow(specular_proportion, material.SpecularPower);

                    Color current_light_specular_color = Color::ScaleRedGreenBlue(specular_proportion, light.Color);

                    if (ShadingType::MATERIAL == material.Shading)
                    {
                        light_total_color += Color::ComponentMultiplyRedGreenBlue(current_light_specular_color, material.SpecularColor);
                    }
                    else
                    {
                        light_total_color += current_light_specular_color;
                    }
                }
            }
        }

        // RETURN THE COMPUTED LIGHTING COLOR.
        return light_total_color;
    }
}
