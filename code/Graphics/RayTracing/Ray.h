#pragma once

#include "Math/Vector3.h"

namespace GRAPHICS
{
namespace RAY_TRACING
{
    /// A ray that can be shot out from a location in the world in a given
    /// direction for the purposes of ray tracing.
    class Ray
    {
    public:
        // CONSTRUCTION.
        explicit Ray(const MATH::Vector3f& origin, const MATH::Vector3f& direction);

        // PUBLIC MEMBER VARIABLES FOR EASY ACCESS.
        /// The origin of the ray.
        MATH::Vector3f Origin = MATH::Vector3f(0.0f, 0.0f, 0.0f);
        /// The direction of the ray.
        MATH::Vector3f Direction = MATH::Vector3f(0.0f, 0.0f, 0.0f);
    };
}
}