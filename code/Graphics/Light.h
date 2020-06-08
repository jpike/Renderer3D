#pragma once

#include "Graphics/Color.h"
#include "Math/Vector3.h"

namespace GRAPHICS
{
    /// The type of light.
    enum class LightType
    {
        /// Ambient light that basically illuminates an entire scene.
        AMBIENT = 0,
        /// Directional light that has a consistent direction and appears infinitely far away.
        DIRECTIONAL,
        /// A point light with a position in space and provides illumination in all directions
        POINT
        /// @todo   Spotlight for advanced functionality?
    };

    /// A light that an provide illumination in a 3D scene.
    class Light
    {
    public:
        MATH::Vector3f PointLightDirectionFrom(const MATH::Vector3f& other_world_position) const;

        /// The type of the light.
        LightType Type = LightType::AMBIENT;
        /// The color of the light.
        GRAPHICS::Color Color = GRAPHICS::Color::BLACK;
        /// The direction for a directional light.
        MATH::Vector3f DirectionalLightDirection = MATH::Vector3f();
        /// The world position for a point light.
        MATH::Vector3f PointLightWorldPosition = MATH::Vector3f();
        
    };
}
