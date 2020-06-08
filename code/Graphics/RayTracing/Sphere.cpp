#include <cmath>
#include "Graphics/RayTracing/Sphere.h"

namespace GRAPHICS
{
namespace RAY_TRACING
{
    /// Computes the surface normal of the sphere at given point.
    /// @param[in]  surface_point - The point on the sphere's surface at which to compute a normal.
    /// @return The unit surface normal at the specified point.
    MATH::Vector3f Sphere::SurfaceNormal(const MATH::Vector3f& surface_point) const
    {
        // Sphere normals always point outward in a direction from the center to the surface.
        MATH::Vector3f surface_normal;
        surface_normal.X = surface_point.X - CenterPosition.X;
        surface_normal.Y = surface_point.Y - CenterPosition.Y;
        surface_normal.Z = surface_point.Z - CenterPosition.Z;

        MATH::Vector3f normalized_surface_normal = MATH::Vector3f::Normalize(surface_normal);
        return normalized_surface_normal;
    }

    /// Gets the material defining surface properties of the object.
    /// @return The material for the object.
    const Material* Sphere::GetMaterial() const
    {
        return Material.get();
    }

    /// Checks for an intersection between a ray and the object.
    /// @param[in]  ray - The ray to check for intersection.
    /// @return A ray-object intersection, if one occurred; std::nullopt otherwise.
    std::optional<RayObjectIntersection> Sphere::Intersect(const Ray& ray) const
    {
        // A sphere can be modeled by an implicit surface equation like:
        //      (Point.X - CenterPosition.X)^2 + (Point.Y - CenterPosition.Y)^2 + (Point.Z - CenterPosition.Z)^2 - Radius^2 = 0
        // Here, any point that lies on the surface of the sphere would result in 0.
        // The above equation can be written in dot product form as (* means dot product where applicable):
        //      (Point - CenterPosition) * (Point - CenterPosition) - Radius^2 = 0
        // If we are looking for points from the ray, we can plug in the ray as:
        //      (Ray.Origin + DistanceFromRayToObject*Ray.Direction - CenterPosition) * (Ray.Origin + DistanceFromRayToObject*Ray.Direction - CenterPosition) - Radius^2 = 0
        // If we attempt to solve this equation, we get the following:
        //      Ray.Origin*(Ray.Origin + DistanceFromRayToObject*Ray.Direction - CenterPosition) +
        //      DistanceFromRayToObject*Ray.Direction*(Ray.Origin + DistanceFromRayToObject*Ray.Direction - CenterPosition) -
        //      CenterPosition*(Ray.Origin + DistanceFromRayToObject*Ray.Direction - CenterPosition) -
        //      Radius^2
        //      = 0
        // And then (distributive property):
        //      Ray.Origin*Ray.Origin + Ray.Origin*DistanceFromRayToObject*Ray.Direction - Ray.Origin*CenterPosition +
        //      DistanceFromRayToObject*Ray.Direction*Ray.Origin + DistanceFromRayToObject^2*Ray.Direction*Ray.Direction - DistanceFromRayToObject*Ray.Direction*CenterPosition +
        //      -CenterPosition*Ray.Origin - CenterPosition*DistanceFromRayToObject*Ray.Direction + CenterPosition*CenterPosition -
        //      Radius^2
        //      = 0
        // Re-arranging terms:
        //      (Ray.Direction*Ray.Direction)*DistanceFromRayToObject^2 + 
        //      Ray.Origin*DistanceFromRayToObject*Ray.Direction + DistanceFromRayToObject*Ray.Direction*Ray.Origin - DistanceFromRayToObject*Ray.Direction*CenterPosition - CenterPosition*DistanceFromRayToObject*Ray.Direction +
        //      Ray.Origin*Ray.Origin - Ray.Origin*CenterPosition - CenterPosition*Ray.Origin + CenterPosition*CenterPosition -
        //      Radius^2
        //      = 0
        // Re-arranging again:
        //      (Ray.Direction*Ray.Direction)*DistanceFromRayToObject^2 + 
        //      2*Ray.Direction*(Ray.Origin - CenterPosition)*DistanceFromRayToObject +
        //      (Ray.Origin - CenterPosition)*(Ray.Origin - CenterPosition) -
        //      Radius^2
        //      = 0
        // This is just a standard quadratic equation of the form at^2 + bt + c = 0 where:
        //      a = (Ray.Direction*Ray.Direction)
        //      b = 2*Ray.Direction*(Ray.Origin - CenterPosition)
        //      c = (Ray.Origin - CenterPosition)*(Ray.Origin - CenterPosition) - Radius^2
        // Therefore, we can solve it with the standard quadratic formula:
        //      t = (-b +- sqrt(b^2 - 4ac)) / 2a

        // CALCULATE THE 3 MAIN COMPONENTS OF THE QUADRATIC FORMULA.
        float a = MATH::Vector3f::DotProduct(ray.Direction, ray.Direction);
        MATH::Vector3f vector_from_sphere_center_to_ray = (ray.Origin - CenterPosition);
        float half_b = MATH::Vector3f::DotProduct(ray.Direction, vector_from_sphere_center_to_ray);
        float b = 2.0f * half_b;
        float c_without_radius = MATH::Vector3f::DotProduct(vector_from_sphere_center_to_ray, vector_from_sphere_center_to_ray);
        float c = c_without_radius - (Radius * Radius);

        // CALCULATE THE DISCRIMINANT.
        // If discriminant is:
        // - Positive = 2 real solutions
        // - Zero = 1 real solutions
        // - Negative = 0 real solutions
        float discriminant = (b * b) - (4 * a * c);
        bool intersections_exist = (discriminant >= 0.0f);
        if (!intersections_exist)
        {
            // INDICATE THAT NO INTERSECTION OCCURRED.
            return std::nullopt;
        }

        // CALCULATE THE TWO POSSIBLE INTERSECTION DISTANCES.
        float first_intersection_distance = ((-1.0f * b) + std::sqrtf(discriminant)) / (2.0f * a);
        float second_intersection_distance = ((-1.0f * b) - std::sqrtf(discriminant)) / (2.0f * a);

        // CHECK IF THE INTERSECTIONS ARE BEHIND VIEW OF THE RAY.
        bool intersects_behind_view = (first_intersection_distance < 0.0f) && (second_intersection_distance < 0.0f);
        if (intersects_behind_view)
        {
            // INDICATE THAT NO VIEWABLE INTERSECTION OCCURRED.
            return std::nullopt;
        }

        // CHECK IF ONLY THE FIRST INTERSECTION IS IN VIEW.
        bool only_first_intersection_is_in_front = (first_intersection_distance >= 0.0f) && (second_intersection_distance < 0.0f);
        if (only_first_intersection_is_in_front)
        {
            // RETURN THE FIRST INTERSECTION.
            RayObjectIntersection intersection;
            intersection.Ray = &ray;
            intersection.DistanceFromRayToObject = first_intersection_distance;
            intersection.Object = this;
            return intersection;
        }

        // CHECK IF ONLY THE SECOND INTERSECTION IS IN VIEW.
        bool only_second_intersection_is_in_front = (second_intersection_distance >= 0.0f) && (first_intersection_distance < 0.0f);
        if (only_second_intersection_is_in_front)
        {
            // RETURN THE SECOND INTERSECTION.
            RayObjectIntersection intersection;
            intersection.Ray = &ray;
            intersection.DistanceFromRayToObject = second_intersection_distance;
            intersection.Object = this;
            return intersection;
        }

        // CHECK IF BOTH INTERSECTIONS ARE IN FRONT.
        bool both_intersections_in_front = (first_intersection_distance >= 0.0f) && (second_intersection_distance >= 0.0f);
        if (both_intersections_in_front)
        {
            // TRACK COMMON INFORMATION REGARDING THE INTERSECTION.
            RayObjectIntersection intersection;
            intersection.Ray = &ray;
            intersection.Object = this;

            // CHOOSE THE EARLIEST INTERSECTION.
            bool first_intersection_is_earliest = (first_intersection_distance < second_intersection_distance);
            if (first_intersection_is_earliest)
            {
                intersection.DistanceFromRayToObject = first_intersection_distance;
            }
            else
            {
                intersection.DistanceFromRayToObject = second_intersection_distance;
            }

            return intersection;
        }

        // INDICATE THAT NO INTERSECTION OCCURRED.
        return std::nullopt;
    }
}
}
