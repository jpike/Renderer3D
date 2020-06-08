#pragma once

#include <optional>
#include "Graphics/Material.h"
#include "Graphics/RayTracing/Ray.h"
#include "Graphics/RayTracing/RayObjectIntersection.h"

namespace GRAPHICS
{
namespace RAY_TRACING
{
    /// An interface for ray-traceable objects in a 3D scene.
    class IObject3D
    {
    public:
        /// Defaulted destructor to allow instantiation in unique pointers.
        virtual ~IObject3D() = default;

        /// Computes the surface normal of the object at given point.
        /// @param[in]  surface_point - The point on the object's surface at which to compute a normal.
        /// @return The unit surface normal at the specified point.
        virtual MATH::Vector3f SurfaceNormal(const MATH::Vector3f& surface_point) const = 0;

        /// Gets the material defining surface properties of the object.
        /// @return The material for the object; null if no material exists.
        virtual const Material* GetMaterial() const = 0;

        /// Checks for an intersection between a ray and the object.
        /// Intended to be implemented in derived classes so that each
        /// derived class can handle its own specific intersection logic.
        /// @param[in]  ray - The ray to check for intersection.
        /// @return A ray-object intersection, if one occurred; std::nullopt otherwise.
        virtual std::optional<RayObjectIntersection> Intersect(const Ray& ray) const = 0;
    };
}
}