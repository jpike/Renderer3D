#include "Graphics/Light.h"

namespace GRAPHICS
{
    /// Computes the direction from the specified world position to this point light.
    /// This method assumes that the light is a point light.
    /// @param[in]  other_world_position - The other world position to compute the direction from.
    /// @return The direction from the specified point to this light's position; not normalized to unit length.
    MATH::Vector3f Light::PointLightDirectionFrom(const MATH::Vector3f& other_world_position) const
    {
        MATH::Vector3f direction_from_other_point_to_light = PointLightWorldPosition - other_world_position;
        return direction_from_other_point_to_light;
    }
}
