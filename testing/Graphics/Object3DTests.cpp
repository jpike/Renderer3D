#include "Graphics/Object3D.h"
#include "ThirdParty/Catch/catch.hpp"

TEST_CASE("World transform can translate a vector.", "[Object3D][WorldTransform]")
{
    // CREATE A 3D OBJECT.
    GRAPHICS::Triangle triangle;
    triangle.Vertices =
    {
        MATH::Vector3f(0.0f, 1.0f, 0.0f),
        MATH::Vector3f(-1.0f, -1.0f, 0.0f),
        MATH::Vector3f(1.0f, -1.0f, 0.0f)
    };

    GRAPHICS::Object3D test_object_3D;
    test_object_3D.Triangles = { triangle };
    test_object_3D.WorldPosition = MATH::Vector3f(1.0f, 3.0f, -5.0f);

    // TRANSFORM THE 3D OBJECT TO WORLD SPACE.
    std::vector<MATH::Vector4f> world_vertices;
    MATH::Matrix4x4f world_transform = test_object_3D.WorldTransform();
    for (const auto& vertex : triangle.Vertices)
    {
        MATH::Vector4f homogeneous_vertex = MATH::Vector4f::HomogeneousPositionVector(vertex);
        MATH::Vector4f world_vertex = world_transform * homogeneous_vertex;
        world_vertices.push_back(world_vertex);
    }

    // VERIFY THE TRANSFORMATION OF THE 3D OBJECT.
    const MATH::Vector4f EXPECTED_TOP_WORLD_VERTEX(1.0f, 4.0f, -5.0f, 1.0f);
    const MATH::Vector4f& actual_top_world_vertex = world_vertices[0];
    REQUIRE(EXPECTED_TOP_WORLD_VERTEX.X == actual_top_world_vertex.X);
    REQUIRE(EXPECTED_TOP_WORLD_VERTEX.Y == actual_top_world_vertex.Y);
    REQUIRE(EXPECTED_TOP_WORLD_VERTEX.Z == actual_top_world_vertex.Z);
    REQUIRE(EXPECTED_TOP_WORLD_VERTEX.W == actual_top_world_vertex.W);

    const MATH::Vector4f EXPECTED_LEFT_WORLD_VERTEX(0.0f, 2.0f, -5.0f, 1.0f);
    const MATH::Vector4f& actual_left_world_vertex = world_vertices[1];
    REQUIRE(EXPECTED_LEFT_WORLD_VERTEX.X == actual_left_world_vertex.X);
    REQUIRE(EXPECTED_LEFT_WORLD_VERTEX.Y == actual_left_world_vertex.Y);
    REQUIRE(EXPECTED_LEFT_WORLD_VERTEX.Z == actual_left_world_vertex.Z);
    REQUIRE(EXPECTED_LEFT_WORLD_VERTEX.W == actual_left_world_vertex.W);

    const MATH::Vector4f EXPECTED_RIGHT_WORLD_VERTEX(2.0f, 2.0f, -5.0f, 1.0f);
    const MATH::Vector4f& actual_right_world_vertex = world_vertices[2];
    REQUIRE(EXPECTED_RIGHT_WORLD_VERTEX.X == actual_right_world_vertex.X);
    REQUIRE(EXPECTED_RIGHT_WORLD_VERTEX.Y == actual_right_world_vertex.Y);
    REQUIRE(EXPECTED_RIGHT_WORLD_VERTEX.Z == actual_right_world_vertex.Z);
    REQUIRE(EXPECTED_RIGHT_WORLD_VERTEX.W == actual_right_world_vertex.W);
}
