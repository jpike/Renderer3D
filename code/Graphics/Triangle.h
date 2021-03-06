#pragma once

#include <array>
#include <cstddef>
#include <memory>
#include <optional>
#include "Graphics/Material.h"
#include "Graphics/RayTracing/Ray.h"
#include "Graphics/RayTracing/RayObjectIntersection.h"
#include "Math/Vector3.h"

namespace GRAPHICS
{
    /// A triangle that can be rendered.
    class Triangle
    {
    public:
        // STATIC CONSTANTS.
        /// The number of vertices in a triangle.
        static constexpr std::size_t VERTEX_COUNT = 3;

        // CONSTRUCTION.
        static Triangle CreateEquilateral(const std::shared_ptr<Material>& material);
        explicit Triangle() = default;
        explicit Triangle(const std::shared_ptr<Material>& material, const std::array<MATH::Vector3f, VERTEX_COUNT>& vertices);

        // OTHER METHODS.
        MATH::Vector3f SurfaceNormal() const;
        std::optional<RAY_TRACING::RayObjectIntersection> Intersect(const RAY_TRACING::Ray& ray) const;

        // PUBLIC MEMBER VARIABLES FOR EASY ACCESS.
        /// The material of the triangle.
        std::shared_ptr<Material> Material = nullptr;
        /// The vertices of the triangle.
        /// Should be in counter-clockwise order.
        std::array<MATH::Vector3f, VERTEX_COUNT> Vertices = {};
    };
}
