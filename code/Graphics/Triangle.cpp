#include "Graphics/Triangle.h"

namespace GRAPHICS
{
    /// Creates an equilateral triangle with the specified material.
    /// @param[in]  material - The material of the triangle.
    /// @return An equilateral triangle of the specified material.
    Triangle Triangle::CreateEquilateral(const std::shared_ptr<GRAPHICS::Material>& material)
    {
        // DEFINE THE OBJECT-SPACE POSITIONS OF THE TRIANGLE VERTICES.
        // The Pythagorean theorem is used to compute the coordinates of the vertices:
        //      /|\
        //     /_|_\
        // height^2 + half_width^2 = side_length^2
        // The efficiency of these calculations has not been measured.
        float side_length = 1.0f;
        float side_length_squared = side_length * side_length;
        float half_width = side_length / 2.0f;
        float half_width_squared = half_width * half_width;
        float height_squared = side_length_squared - half_width_squared;
        float height = sqrt(height_squared);
        float half_height = height / 2.0f;
        float top_x = 0.0f;
        float top_y = half_height;
        float bottom_y = -half_height;
        float left_x = -half_width;
        float right_x = half_width;
        float z = 0.0f;

        // SET THE VERTICES IN A TRIANGLE.
        Triangle triangle;
        triangle.Material = material;
        triangle.Vertices =
        {
            MATH::Vector3f(top_x, top_y, z),
            MATH::Vector3f(left_x, bottom_y, z),
            MATH::Vector3f(right_x, bottom_y, z)
        };
        return triangle;
    }

    /// Constructs a triangle with the given material and vertices.
    /// @param[in]  material - The material for the triangle.
    /// @param[in]  vertices - The vertices of the triangle.
    Triangle::Triangle(const std::shared_ptr<GRAPHICS::Material>& material, const std::array<MATH::Vector3f, Triangle::VERTEX_COUNT>& vertices):
        Material(material),
        Vertices(vertices)
    {}

    /// Computes the surface normal of the triangle.
    /// @return The unit surface normal.
    MATH::Vector3f Triangle::SurfaceNormal() const
    {
        // The edges are calculated relative to the first vertex.
        // Since they're in a counter-clockwise order, the vertex for
        // the "first" edge should be the first component of the cross
        // product to get an outward-facing normal.
        MATH::Vector3f first_edge = Vertices[1] - Vertices[0];
        MATH::Vector3f second_edge = Vertices[2] - Vertices[0];
        MATH::Vector3f surface_normal = MATH::Vector3f::CrossProduct(first_edge, second_edge);
        MATH::Vector3f normalized_surface_normal = MATH::Vector3f::Normalize(surface_normal);
        return normalized_surface_normal;
    }

    /// Computes the surface normal of the triangle at given point.
    /// Same at all places for a triangle.
    /// @param[in]  surface_point - The point on the triangle's surface at which to compute a normal.
    /// @return The unit surface normal at the specified point.
    MATH::Vector3f Triangle::SurfaceNormal(const MATH::Vector3f& surface_point) const
    {
        // This parameter is unneeded for triangles since they have consistent normals.
        surface_point;

        MATH::Vector3f normalized_surface_normal = SurfaceNormal();
        return normalized_surface_normal;
    }

    /// Gets the material defining surface properties of the object.
    /// @return The material for the object.
    const Material* Triangle::GetMaterial() const
    {
        return Material.get();
    }

    /// Checks for an intersection between a ray and the object.
    /// @param[in]  ray - The ray to check for intersection.
    /// @return A ray-object intersection, if one occurred; std::nullopt otherwise.
    std::optional<RAY_TRACING::RayObjectIntersection> Triangle::Intersect(const RAY_TRACING::Ray& ray) const
    {
        // GET THE TRIANGLE'S SURFACE NORMAL.
        MATH::Vector3f surface_normal = SurfaceNormal();

        // GET EACH OF THE TRIANGLES EDGES IN COUNTER-CLOCKWISE ORDER.
        MATH::Vector3f edge_a = Vertices[1] - Vertices[0];
        MATH::Vector3f edge_b = Vertices[2] - Vertices[1];
        MATH::Vector3f edge_c = Vertices[0] - Vertices[2];

        // CHECK FOR INTERSECTION WITH THE PLANE.
        float distance_from_ray_to_object = MATH::Vector3f::DotProduct(surface_normal, Vertices[0]);
        distance_from_ray_to_object -= MATH::Vector3f::DotProduct(surface_normal, ray.Origin);
        distance_from_ray_to_object /= MATH::Vector3f::DotProduct(surface_normal, ray.Direction);
        bool intersection_in_front_of_current_view = (distance_from_ray_to_object >= 0.0f);
        if (!intersection_in_front_of_current_view)
        {
            // INDICATE THAT NO INTERSECTION OCCURRED.
            return std::nullopt;
        }

        // CHECK FOR INTERSECTION WITHIN THE TRIANGLE.
        MATH::Vector3f intersection_point = ray.Origin + MATH::Vector3f::Scale(distance_from_ray_to_object, ray.Direction);
        MATH::Vector3f edge_a_for_point = intersection_point - Vertices[0];
        MATH::Vector3f edge_b_for_point = intersection_point - Vertices[1];
        MATH::Vector3f edge_c_for_point = intersection_point - Vertices[2];

        float dot_product_for_edge_a = MATH::Vector3f::DotProduct(surface_normal, MATH::Vector3f::CrossProduct(edge_a, edge_a_for_point));
        float dot_product_for_edge_b = MATH::Vector3f::DotProduct(surface_normal, MATH::Vector3f::CrossProduct(edge_b, edge_b_for_point));
        float dot_product_for_edge_c = MATH::Vector3f::DotProduct(surface_normal, MATH::Vector3f::CrossProduct(edge_c, edge_c_for_point));

        bool intersects_triangle = (dot_product_for_edge_a >= 0.0f) && (dot_product_for_edge_b >= 0.0f) && (dot_product_for_edge_c >= 0.0f);
        if (!intersects_triangle)
        {
            // INDICATE THAT NO INTERSECTION OCCURRED.
            return std::nullopt;
        }

        // RETURN INFORMATION ABOUT THE INTERSECTION.
        RAY_TRACING::RayObjectIntersection intersection;
        intersection.Ray = &ray;
        intersection.DistanceFromRayToObject = distance_from_ray_to_object;
        intersection.Object = this;
        return intersection;
    }
}
