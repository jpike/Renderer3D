#pragma once

#include <array>
#include <cstddef>
#include <memory>
#include "Graphics/Color.h"
#include "Graphics/Material.h"
#include "Math/Vector3.h"

namespace GRAPHICS
{
    /// A triangle that has been transformed into screen-space and is ready to be
    /// rasterized directly to a 2D pixel grid.  This separate representation of
    /// a triangle helps simplify passing of data to 2D rasterization algorithms.
    struct ScreenSpaceTriangle
    {
        // STATIC CONSTANTS.
        /// The number of vertices in a triangle.
        static constexpr std::size_t VERTEX_COUNT = 3;

        // PUBLIC MEMBER VARIABLES FOR EASY ACCESS.
        /// The material of the triangle.
        std::shared_ptr<Material> Material = nullptr;
        /// The vertices of the triangle.  Should be in counter-clockwise order.
        /// A z-coordinate is included to support depth-testing.
        std::array<MATH::Vector3f, VERTEX_COUNT> VertexPositions = {};
        /// The colors of each vertex (same order as vertex positions).
        std::array<GRAPHICS::Color, VERTEX_COUNT> VertexColors = {};
    };
}
