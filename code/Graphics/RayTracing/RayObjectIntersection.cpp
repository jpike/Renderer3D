#include "Graphics/RayTracing/RayObjectIntersection.h"

namespace GRAPHICS
{
namespace RAY_TRACING
{
    /// Computes the intersection point.
    /// @return The intersection point.
    MATH::Vector3f RayObjectIntersection::IntersectionPoint() const
    {
        // The ray starts at its origin and travels some distance along
        // its direction until it intersects an object.
        MATH::Vector3f directional_distance_traveled = MATH::Vector3f::Scale(
            DistanceFromRayToObject,
            Ray->Direction);
        MATH::Vector3f intersection_point = Ray->Origin + directional_distance_traveled;

        return intersection_point;
    }
}
}
