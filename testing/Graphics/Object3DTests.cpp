#include <cmath>
#include <map>
#include "Graphics/Object3D.h"
#include "ThirdParty/Catch/catch.hpp"

constexpr float ONE_ROTATED_30_DEGREES = 0.86603f;
constexpr float ONE_ROTATED_45_DEGREES = 0.70711f;
constexpr float ONE_ROTATED_60_DEGREES = 0.5f;

TEST_CASE("World transform can translate a vector.", "[Object3D][WorldTransform][Translation]")
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

TEST_CASE("World transform can rotate a vector around the z-axis by common locations on unit circle.", "[Object3D][WorldTransform][Rotation]")
{
    // DEFINE THE BASE TRIANGLE TO ROTATE.
    GRAPHICS::Triangle triangle;
    triangle.Vertices =
    {
        MATH::Vector3f(0.0f, 1.0f, 0.0f), // top
        MATH::Vector3f(-1.0f, -1.0f, 0.0f), // left
        MATH::Vector3f(1.0f, -1.0f, 0.0f) // right
    };

    // DEFINE THE DIFFERENT ROTATION ANGLES AND EXPECTED TRIANGLES.
    const std::shared_ptr<GRAPHICS::Material> NO_MATERIAL = nullptr;
    std::map<float, GRAPHICS::Triangle> rotation_angles_in_degrees_and_expected_triangles =
    {
        {
            30.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    MATH::Vector3f(
                        -0.5f, // -1.0f / 2.0f,
                        0.86603f, // std::sqrt(3.0f) / 2.0f, 
                        0.0f),
                    MATH::Vector3f(
                        -0.36603f, //-std::sqrt(3.0f)/2.0f + 0.5f, // -1*cos(30) - -1*sin(30) = -1*sqrt(3)/2 + 1*1/2 = -sqrt(3)/2 + 1/2
                        -1.36603f, // -0.5f - std::sqrt(3.0f)/2.0f, // -1*sin(30) + -1*cos(30) = -1*1/2 -1*sqrt(3)/2 = -1/2 - sqrt(3)/2
                        0.0f),
                    MATH::Vector3f(
                        1.36603f, // std::sqrt(3.0f)/2.0f + 0.5f, // 1*cos(30) - -1*sin(30) = 1*sqrt(3)/2 - -1*1/2 = sqrt(3)/2 + 1/2
                        -0.36603f, //0.5f - std::sqrt(3.0f)/2.0f, // 1*sin(30) + -1*cos(30) = 1*1/2 + -1*sqrt(3)/2 = 1/2 - sqrt(3)/2
                        0.0f),
                })
        },
        {
            45.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    MATH::Vector3f(
                        -0.70711f, // -sqrt(2) / 2
                        0.70711f, // sqrt(2) / 2 
                        0.0f),
                    MATH::Vector3f(
                         0.0f, // -1*cos(45) - -1*sin(45) = -sqrt(2)/2 + sqrt(2)/2
                        -1.41421f, // -1*sin(45) + -1*cos(45) = -sqrt(2)/2 + -sqrt(2)/2 = -sqrt(2)
                        0.0f),
                    MATH::Vector3f(
                        1.41421f, // 1*cos(45) - -1*sin(45) = sqrt(2)/2 + sqrt(2)/2 = sqrt(2)
                        0.0f, // 1*sin(45) + -1*cos(45) = sqrt(2)/2 - sqrt(2)/2
                        0.0f),
                })
        },
        {
            60.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    MATH::Vector3f(
                        -0.86603f, // -sqrt(3)/2
                        0.5f, // 0*sin(60) + 1*cos(60) = 1/2
                        0.0f),
                    MATH::Vector3f(
                        0.36603f, // -1*cos(60) - -1*sin(60) = -1/2 + sqrt(3)/2
                        -1.36603f, // -1*sin(60) + -1*cos(60) = -sqrt(3)/2 - 1/2 
                        0.0f),
                    MATH::Vector3f(
                        1.36603f, // 1*cos(60) - -1*sin(60) = 1/2 + sqrt(3)/2
                        0.36603f, // 1*sin(60) + -1*cos(60) = sqrt(3)/2 - 1/2
                        0.0f),
                })
        },
        {
            90.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    MATH::Vector3f(
                        -1.0f,
                        0.0f,
                        0.0f),
                    MATH::Vector3f(
                        1.0f, // -1*cos(90) - -1*sin(90) = 0 + 1
                        -1.0f, // -1*sin(90) + -1*cos(90) = -1 + 0
                        0.0f),
                    MATH::Vector3f(
                        1.0f, // 1*cos(90) - -1*sin(90) = 0 + 1
                        1.0f, // 1*sin(90) + -1*cos(90) = 1 + 0
                        0.0f),
                })
        },
        {
            120.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    MATH::Vector3f(
                        -0.86603f, // 0*cos(120) - 1*sin(120) = -sqrt(3)/2
                        -0.5f, // 0*sin(120) + 1*cos(120) = -1/2
                        0.0f),
                    MATH::Vector3f(
                        1.36603f, // -1*cos(120) - -1*sin(120) = 1/2 + sqrt(3)/2
                        -0.36603f, // -1*sin(120) + -1*cos(120) = -sqrt(3)/2 + 1/2
                        0.0f),
                    MATH::Vector3f(
                        0.36603f, // 1*cos(120) - -1*sin(120) = -1/2 + sqrt(3)/2
                        1.36603f, // 1*sin(120) + -1*cos(120) = sqrt(3)/2 + 1/2
                        0.0f),
                })
        },
        {
            135.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    MATH::Vector3f(
                        -0.70711f, // 0*cos(135) - 1*sin(135) = -sqrt(2)/2
                        -0.70711f, // 0*sin(135) + 1*cos(135) = -sqrt(2)/2
                        0.0f),
                    MATH::Vector3f(
                        1.41421f, // -1*cos(135) - -1*sin(135) = sqrt(2)/2 + sqrt(2)/2 = sqrt(2)
                        0.0f, // -1*sin(135) + -1*cos(135) = -sqrt(2)/2 + sqrt(2)/2 = 0
                        0.0f),
                    MATH::Vector3f(
                        0.0f, // 1*cos(135) - -1*sin(135) = -sqrt(2)/2 + sqrt(2)/2 = 0
                        1.41421f, // 1*sin(135) + -1*cos(135) = sqrt(2)/2 + sqrt(2)/2 = sqrt(2)
                        0.0f),
                })
        },
        {
            150.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    MATH::Vector3f(
                        -0.5f, // 0*cos(150) - 1*sin(150) = -1/2
                        -0.86603f, // 0*sin(150) + 1*cos(150) = -sqrt(3)/2
                        0.0f),
                    MATH::Vector3f(
                        1.36603f, // -1*cos(150) - -1*sin(150) = sqrt(3)/2 + 1/2
                        0.36603f, // -1*sin(150) + -1*cos(150) = -1/2 + sqrt(3)/2
                        0.0f),
                    MATH::Vector3f(
                        -0.36603f, // 1*cos(150) - -1*sin(150) = -sqrt(3)/2 + 1/2
                        1.36603f, // 1*sin(150) + -1*cos(150) = 1/2 + sqrt(3)/2
                        0.0f),
                })
        },
        {
            180.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    MATH::Vector3f(
                        0.0f, // 0*cos(180) - 1*sin(180) = 0
                        -1.0f, // 0*sin(180) + 1*cos(180) = -1
                        0.0f),
                    MATH::Vector3f(
                        1.0f, // -1*cos(180) - -1*sin(180) = 1
                        1.0f, // -1*sin(180) + -1*cos(180) = 1
                        0.0f),
                    MATH::Vector3f(
                        -1.0f, // 1*cos(180) - -1*sin(180) = -1
                        1.0f, // 1*sin(180) + -1*cos(180) = 1
                        0.0f),
                })
        },
        {
            210.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    MATH::Vector3f(
                        0.5f, // 0*cos(210) - 1*sin(210) = 1/2
                        -0.86603f, // 0*sin(210) + 1*cos(210) = -sqrt(3)/2
                        0.0f),
                    MATH::Vector3f(
                        0.36603f, // -1*cos(210) - -1*sin(210) = sqrt(3)/2 - 1/2
                        1.36603f, // -1*sin(210) + -1*cos(210) = 1/2 + sqrt(3)/2
                        0.0f),
                    MATH::Vector3f(
                        -1.36603f, // 1*cos(210) - -1*sin(210) = -sqrt(3)/2 - 1/2
                        0.36603f, // 1*sin(210) + -1*cos(210) = -1/2 + sqrt(3)/2
                        0.0f),
                })
        },
        {
            225.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    MATH::Vector3f(
                        0.70711f, // 0*cos(225) - 1*sin(225) = sqrt(2)/2
                        -0.70711f, // 0*sin(225) + 1*cos(225) = -sqrt(2)/2
                        0.0f),
                    MATH::Vector3f(
                        0.0f, // -1*cos(225) - -1*sin(225) = sqrt(2)/2 - sqrt(2)2
                        1.41421f, // -1*sin(225) + -1*cos(225) = sqrt(2)/2 + sqrt(2)/2 = sqrt(2)
                        0.0f),
                    MATH::Vector3f(
                        -1.41421f, // 1*cos(225) - -1*sin(225) = -sqrt(2)/2 - sqrt(2)/2 = -sqrt(2)
                        0.0f, // 1*sin(225) + -1*cos(225) = -sqrt(2)/2 + sqrt(2)/2
                        0.0f),
                })
        },
        {
            240.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    MATH::Vector3f(
                        0.86603f, // 0*cos(240) - 1*sin(240) = sqrt(3)/2
                        -0.5f, // 0*sin(240) + 1*cos(240) = -1/2
                        0.0f),
                    MATH::Vector3f(
                        -0.36603f, // -1*cos(240) - -1*sin(240) = 1/2 - sqrt(3)/2
                        1.36603f, // -1*sin(240) + -1*cos(240) = sqrt(3)/2 + 1/2
                        0.0f),
                    MATH::Vector3f(
                        -1.36603f, // 1*cos(240) - -1*sin(240) = -1/2 - sqrt(3)/2
                        -0.36603f, // 1*sin(240) + -1*cos(240) = -sqrt(3)/2 + 1/2
                        0.0f),
                })
        },
        {
            270.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    MATH::Vector3f(
                        1.0f, // 0*cos(270) - 1*sin(270) = 1
                        0.0f, // 0*sin(270) + 1*cos(270) = 0
                        0.0f),
                    MATH::Vector3f(
                        -1.0f, // -1*cos(270) - -1*sin(270) = 0 - 1
                        1.0f, // -1*sin(270) + -1*cos(270) = 1
                        0.0f),
                    MATH::Vector3f(
                        -1.0f, // 1*cos(270) - -1*sin(270) = 0 - 1
                        -1.0f, // 1*sin(270) + -1*cos(270) = -1
                        0.0f),
                })
        },
        {
            300.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    MATH::Vector3f(
                        0.86603f, // 0*cos(300) - 1*sin(300) = sqrt(3)/2
                        0.5f, // 0*sin(300) + 1*cos(300) = 1/2
                        0.0f),
                    MATH::Vector3f(
                        -1.36603f, // -1*cos(300) - -1*sin(300) = -1/2 - sqrt(3)/2
                        0.36603f, // -1*sin(300) + -1*cos(300) = sqrt(3)/2 - 1/2
                        0.0f),
                    MATH::Vector3f(
                        -0.36603f, // 1*cos(300) - -1*sin(300) = 1/2 - sqrt(3)/2
                        -1.36603f, // 1*sin(300) + -1*cos(300) = -sqrt(3)/2 - 1/2
                        0.0f),
                })
        },
        {
            315.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    MATH::Vector3f(
                        0.70711f, // 0*cos(315) - 1*sin(315) = sqrt(2)/2
                        0.70711f, // 0*sin(315) + 1*cos(315) = sqrt(2)/2
                        0.0f),
                    MATH::Vector3f(
                        -1.41421f, // -1*cos(315) - -1*sin(315) = -sqrt(2)/2 - sqrt(2)/2 = -sqrt(2)
                        0.0f, // -1*sin(315) + -1*cos(315) = sqrt(2)/2 - sqrt(2)/2 = 0
                        0.0f),
                    MATH::Vector3f(
                        0.0f, // 1*cos(315) - -1*sin(315) = sqrt(2)/2 - sqrt(2)/2 = 0
                        -1.41421f, // 1*sin(315) + -1*cos(315) = -sqrt(2)/2 - sqrt(2)/2 = -sqrt(2)
                        0.0f),
                })
        },
        {
            330.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    MATH::Vector3f(
                        0.5f, // 0*cos(330) - 1*sin(330) = 1/2
                        0.86603f, // 0*sin(330) + 1*cos(330) = sqrt(3)/2
                        0.0f),
                    MATH::Vector3f(
                        -1.36603f, // -1*cos(330) - -1*sin(330) = -sqrt(3)/2 - 1/2
                        -0.36603f, // -1*sin(330) + -1*cos(330) = 1/2 - sqrt(3)/2
                        0.0f),
                    MATH::Vector3f(
                        0.36603f, // 1*cos(330) - -1*sin(330) = sqrt(3)/2 - 1/2
                        -1.36603f, // 1*sin(330) + -1*cos(330) = -1/2 - sqrt(3)/2
                        0.0f),
                })
        },
        {
            360.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                triangle.Vertices)
        },
        {
            -30.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    MATH::Vector3f(
                        0.5f, // 0*cos(330) - 1*sin(330) = 1/2
                        0.86603f, // 0*sin(330) + 1*cos(330) = sqrt(3)/2
                        0.0f),
                    MATH::Vector3f(
                        -1.36603f, // -1*cos(330) - -1*sin(330) = -sqrt(3)/2 - 1/2
                        -0.36603f, // -1*sin(330) + -1*cos(330) = 1/2 - sqrt(3)/2
                        0.0f),
                    MATH::Vector3f(
                        0.36603f, // 1*cos(330) - -1*sin(330) = sqrt(3)/2 - 1/2
                        -1.36603f, // 1*sin(330) + -1*cos(330) = -1/2 - sqrt(3)/2
                        0.0f),
                })
        },
        {
            -45.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    MATH::Vector3f(
                        0.70711f, // 0*cos(315) - 1*sin(315) = sqrt(2)/2
                        0.70711f, // 0*sin(315) + 1*cos(315) = sqrt(2)/2
                        0.0f),
                    MATH::Vector3f(
                        -1.41421f, // -1*cos(315) - -1*sin(315) = -sqrt(2)/2 - sqrt(2)/2 = -sqrt(2)
                        0.0f, // -1*sin(315) + -1*cos(315) = sqrt(2)/2 - sqrt(2)/2 = 0
                        0.0f),
                    MATH::Vector3f(
                        0.0f, // 1*cos(315) - -1*sin(315) = sqrt(2)/2 - sqrt(2)/2 = 0
                        -1.41421f, // 1*sin(315) + -1*cos(315) = -sqrt(2)/2 - sqrt(2)/2 = -sqrt(2)
                        0.0f),
                })
        },
        {
            -60.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    MATH::Vector3f(
                        0.86603f, // 0*cos(300) - 1*sin(300) = sqrt(3)/2
                        0.5f, // 0*sin(300) + 1*cos(300) = 1/2
                        0.0f),
                    MATH::Vector3f(
                        -1.36603f, // -1*cos(300) - -1*sin(300) = -1/2 - sqrt(3)/2
                        0.36603f, // -1*sin(300) + -1*cos(300) = sqrt(3)/2 - 1/2
                        0.0f),
                    MATH::Vector3f(
                        -0.36603f, // 1*cos(300) - -1*sin(300) = 1/2 - sqrt(3)/2
                        -1.36603f, // 1*sin(300) + -1*cos(300) = -sqrt(3)/2 - 1/2
                        0.0f),
                })
        },
        {
            -90.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    MATH::Vector3f(
                        1.0f, // 0*cos(270) - 1*sin(270) = 1
                        0.0f, // 0*sin(270) + 1*cos(270) = 0
                        0.0f),
                    MATH::Vector3f(
                        -1.0f, // -1*cos(270) - -1*sin(270) = 0 - 1
                        1.0f, // -1*sin(270) + -1*cos(270) = 1
                        0.0f),
                    MATH::Vector3f(
                        -1.0f, // 1*cos(270) - -1*sin(270) = 0 - 1
                        -1.0f, // 1*sin(270) + -1*cos(270) = -1
                        0.0f),
                })
        },
        {
            -120.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    MATH::Vector3f(
                        0.86603f, // 0*cos(240) - 1*sin(240) = sqrt(3)/2
                        -0.5f, // 0*sin(240) + 1*cos(240) = -1/2
                        0.0f),
                    MATH::Vector3f(
                        -0.36603f, // -1*cos(240) - -1*sin(240) = 1/2 - sqrt(3)/2
                        1.36603f, // -1*sin(240) + -1*cos(240) = sqrt(3)/2 + 1/2
                        0.0f),
                    MATH::Vector3f(
                        -1.36603f, // 1*cos(240) - -1*sin(240) = -1/2 - sqrt(3)/2
                        -0.36603f, // 1*sin(240) + -1*cos(240) = -sqrt(3)/2 + 1/2
                        0.0f),
                })
        },
        {
            -135.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    MATH::Vector3f(
                        0.70711f, // 0*cos(225) - 1*sin(225) = sqrt(2)/2
                        -0.70711f, // 0*sin(225) + 1*cos(225) = -sqrt(2)/2
                        0.0f),
                    MATH::Vector3f(
                        0.0f, // -1*cos(225) - -1*sin(225) = sqrt(2)/2 - sqrt(2)2
                        1.41421f, // -1*sin(225) + -1*cos(225) = sqrt(2)/2 + sqrt(2)/2 = sqrt(2)
                        0.0f),
                    MATH::Vector3f(
                        -1.41421f, // 1*cos(225) - -1*sin(225) = -sqrt(2)/2 - sqrt(2)/2 = -sqrt(2)
                        0.0f, // 1*sin(225) + -1*cos(225) = -sqrt(2)/2 + sqrt(2)/2
                        0.0f),
                })
        },
        {
            -150.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    MATH::Vector3f(
                        0.5f, // 0*cos(210) - 1*sin(210) = 1/2
                        -0.86603f, // 0*sin(210) + 1*cos(210) = -sqrt(3)/2
                        0.0f),
                    MATH::Vector3f(
                        0.36603f, // -1*cos(210) - -1*sin(210) = sqrt(3)/2 - 1/2
                        1.36603f, // -1*sin(210) + -1*cos(210) = 1/2 + sqrt(3)/2
                        0.0f),
                    MATH::Vector3f(
                        -1.36603f, // 1*cos(210) - -1*sin(210) = -sqrt(3)/2 - 1/2
                        0.36603f, // 1*sin(210) + -1*cos(210) = -1/2 + sqrt(3)/2
                        0.0f),
                })
        },
        {
            -180.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    MATH::Vector3f(
                        0.0f, // 0*cos(180) - 1*sin(180) = 0
                        -1.0f, // 0*sin(180) + 1*cos(180) = -1
                        0.0f),
                    MATH::Vector3f(
                        1.0f, // -1*cos(180) - -1*sin(180) = 1
                        1.0f, // -1*sin(180) + -1*cos(180) = 1
                        0.0f),
                    MATH::Vector3f(
                        -1.0f, // 1*cos(180) - -1*sin(180) = -1
                        1.0f, // 1*sin(180) + -1*cos(180) = 1
                        0.0f),
                })
        },
        {
            -210.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    MATH::Vector3f(
                        -0.5f, // 0*cos(150) - 1*sin(150) = -1/2
                        -0.86603f, // 0*sin(150) + 1*cos(150) = -sqrt(3)/2
                        0.0f),
                    MATH::Vector3f(
                        1.36603f, // -1*cos(150) - -1*sin(150) = sqrt(3)/2 + 1/2
                        0.36603f, // -1*sin(150) + -1*cos(150) = -1/2 + sqrt(3)/2
                        0.0f),
                    MATH::Vector3f(
                        -0.36603f, // 1*cos(150) - -1*sin(150) = -sqrt(3)/2 + 1/2
                        1.36603f, // 1*sin(150) + -1*cos(150) = 1/2 + sqrt(3)/2
                        0.0f),
                })
        },
        {
            -225.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    MATH::Vector3f(
                        -0.70711f, // 0*cos(135) - 1*sin(135) = -sqrt(2)/2
                        -0.70711f, // 0*sin(135) + 1*cos(135) = -sqrt(2)/2
                        0.0f),
                    MATH::Vector3f(
                        1.41421f, // -1*cos(135) - -1*sin(135) = sqrt(2)/2 + sqrt(2)/2 = sqrt(2)
                        0.0f, // -1*sin(135) + -1*cos(135) = -sqrt(2)/2 + sqrt(2)/2 = 0
                        0.0f),
                    MATH::Vector3f(
                        0.0f, // 1*cos(135) - -1*sin(135) = -sqrt(2)/2 + sqrt(2)/2 = 0
                        1.41421f, // 1*sin(135) + -1*cos(135) = sqrt(2)/2 + sqrt(2)/2 = sqrt(2)
                        0.0f),
                })
        },
        {
            -240.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    MATH::Vector3f(
                        -0.86603f, // 0*cos(120) - 1*sin(120) = -sqrt(3)/2
                        -0.5f, // 0*sin(120) + 1*cos(120) = -1/2
                        0.0f),
                    MATH::Vector3f(
                        1.36603f, // -1*cos(120) - -1*sin(120) = 1/2 + sqrt(3)/2
                        -0.36603f, // -1*sin(120) + -1*cos(120) = -sqrt(3)/2 + 1/2
                        0.0f),
                    MATH::Vector3f(
                        0.36603f, // 1*cos(120) - -1*sin(120) = -1/2 + sqrt(3)/2
                        1.36603f, // 1*sin(120) + -1*cos(120) = sqrt(3)/2 + 1/2
                        0.0f),
                })
        },
        {
            -270.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    MATH::Vector3f(
                        -1.0f,
                        0.0f,
                        0.0f),
                    MATH::Vector3f(
                        1.0f, // -1*cos(90) - -1*sin(90) = 0 + 1
                        -1.0f, // -1*sin(90) + -1*cos(90) = -1 + 0
                        0.0f),
                    MATH::Vector3f(
                        1.0f, // 1*cos(90) - -1*sin(90) = 0 + 1
                        1.0f, // 1*sin(90) + -1*cos(90) = 1 + 0
                        0.0f),
                })
        },
        {
            -300.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    MATH::Vector3f(
                        -0.86603f, // -sqrt(3)/2
                        0.5f, // 0*sin(60) + 1*cos(60) = 1/2
                        0.0f),
                    MATH::Vector3f(
                        0.36603f, // -1*cos(60) - -1*sin(60) = -1/2 + sqrt(3)/2
                        -1.36603f, // -1*sin(60) + -1*cos(60) = -sqrt(3)/2 - 1/2 
                        0.0f),
                    MATH::Vector3f(
                        1.36603f, // 1*cos(60) - -1*sin(60) = 1/2 + sqrt(3)/2
                        0.36603f, // 1*sin(60) + -1*cos(60) = sqrt(3)/2 - 1/2
                        0.0f),
                })
        },
        {
            -315.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    MATH::Vector3f(
                        -0.70711f, // -sqrt(2) / 2
                        0.70711f, // sqrt(2) / 2 
                        0.0f),
                    MATH::Vector3f(
                         0.0f, // -1*cos(45) - -1*sin(45) = -sqrt(2)/2 + sqrt(2)/2
                        -1.41421f, // -1*sin(45) + -1*cos(45) = -sqrt(2)/2 + -sqrt(2)/2 = -sqrt(2)
                        0.0f),
                    MATH::Vector3f(
                        1.41421f, // 1*cos(45) - -1*sin(45) = sqrt(2)/2 + sqrt(2)/2 = sqrt(2)
                        0.0f, // 1*sin(45) + -1*cos(45) = sqrt(2)/2 - sqrt(2)/2
                        0.0f),
                })
        },
        {
            -330.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    MATH::Vector3f(
                        -0.5f, // -1.0f / 2.0f,
                        0.86603f, // std::sqrt(3.0f) / 2.0f, 
                        0.0f),
                    MATH::Vector3f(
                        -0.36603f, //-std::sqrt(3.0f)/2.0f + 0.5f, // -1*cos(30) - -1*sin(30) = -1*sqrt(3)/2 + 1*1/2 = -sqrt(3)/2 + 1/2
                        -1.36603f, // -0.5f - std::sqrt(3.0f)/2.0f, // -1*sin(30) + -1*cos(30) = -1*1/2 -1*sqrt(3)/2 = -1/2 - sqrt(3)/2
                        0.0f),
                    MATH::Vector3f(
                        1.36603f, // std::sqrt(3.0f)/2.0f + 0.5f, // 1*cos(30) - -1*sin(30) = 1*sqrt(3)/2 - -1*1/2 = sqrt(3)/2 + 1/2
                        -0.36603f, //0.5f - std::sqrt(3.0f)/2.0f, // 1*sin(30) + -1*cos(30) = 1*1/2 + -1*sqrt(3)/2 = 1/2 - sqrt(3)/2
                        0.0f),
                })
        },
        {
            -360.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                triangle.Vertices)
        },
    };

    // TEST EACH ROTATION.
    for (const auto& [rotation_angle_in_degrees, expected_triangle] : rotation_angles_in_degrees_and_expected_triangles)
    {
        // PROVIDE VISIBILITY INTO THE CURRENT TEST CASE.
        INFO("Testing rotation degrees: " + std::to_string(rotation_angle_in_degrees));

        // CREATE A 3D OBJECT.
        GRAPHICS::Object3D test_object_3D;
        test_object_3D.Triangles = { triangle };
        test_object_3D.RotationInRadians = MATH::Vector3<MATH::Angle<float>::Radians>(
            MATH::Angle<float>::Radians(0.0f),
            MATH::Angle<float>::Radians(0.0f),
            MATH::Angle<float>::DegreesToRadians(MATH::Angle<float>::Degrees(rotation_angle_in_degrees)));

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
        constexpr unsigned int TOP_VERTEX_INDEX = 0;
        const MATH::Vector4f EXPECTED_TOP_WORLD_VERTEX = MATH::Vector4f::HomogeneousPositionVector(expected_triangle.Vertices[TOP_VERTEX_INDEX]);
        const MATH::Vector4f& actual_top_world_vertex = world_vertices[0];
        // Due to numerical precision issues, the margin is modified.
        constexpr float APPROXIMATION_ALLOWED_ABSOLUTE_MARGIN = 0.0001f;
        REQUIRE(EXPECTED_TOP_WORLD_VERTEX.X == Approx(actual_top_world_vertex.X).margin(APPROXIMATION_ALLOWED_ABSOLUTE_MARGIN));
        REQUIRE(EXPECTED_TOP_WORLD_VERTEX.Y == Approx(actual_top_world_vertex.Y).margin(APPROXIMATION_ALLOWED_ABSOLUTE_MARGIN));
        REQUIRE(EXPECTED_TOP_WORLD_VERTEX.Z == Approx(actual_top_world_vertex.Z));
        REQUIRE(EXPECTED_TOP_WORLD_VERTEX.W == Approx(actual_top_world_vertex.W));

        constexpr unsigned int LEFT_VERTEX_INDEX = 1;
        const MATH::Vector4f EXPECTED_LEFT_WORLD_VERTEX = MATH::Vector4f::HomogeneousPositionVector(expected_triangle.Vertices[LEFT_VERTEX_INDEX]);
        const MATH::Vector4f& actual_left_world_vertex = world_vertices[1];
        REQUIRE(EXPECTED_LEFT_WORLD_VERTEX.X == Approx(actual_left_world_vertex.X).margin(APPROXIMATION_ALLOWED_ABSOLUTE_MARGIN));
        REQUIRE(EXPECTED_LEFT_WORLD_VERTEX.Y == Approx(actual_left_world_vertex.Y).margin(APPROXIMATION_ALLOWED_ABSOLUTE_MARGIN));
        REQUIRE(EXPECTED_LEFT_WORLD_VERTEX.Z == Approx(actual_left_world_vertex.Z));
        REQUIRE(EXPECTED_LEFT_WORLD_VERTEX.W == Approx(actual_left_world_vertex.W));

        constexpr unsigned int RIGHT_VERTEX_INDEX = 2;
        const MATH::Vector4f EXPECTED_RIGHT_WORLD_VERTEX = MATH::Vector4f::HomogeneousPositionVector(expected_triangle.Vertices[RIGHT_VERTEX_INDEX]);;
        const MATH::Vector4f& actual_right_world_vertex = world_vertices[2];
        REQUIRE(EXPECTED_RIGHT_WORLD_VERTEX.X == Approx(actual_right_world_vertex.X).margin(APPROXIMATION_ALLOWED_ABSOLUTE_MARGIN));
        REQUIRE(EXPECTED_RIGHT_WORLD_VERTEX.Y == Approx(actual_right_world_vertex.Y).margin(APPROXIMATION_ALLOWED_ABSOLUTE_MARGIN));
        REQUIRE(EXPECTED_RIGHT_WORLD_VERTEX.Z == Approx(actual_right_world_vertex.Z));
        REQUIRE(EXPECTED_RIGHT_WORLD_VERTEX.W == Approx(actual_right_world_vertex.W));
    }
}

TEST_CASE("World transform can rotate a vector around the x-axis by common locations on unit circle.", "[Object3D][WorldTransform][Rotation]")
{
    // DEFINE THE BASE TRIANGLE TO ROTATE.
    GRAPHICS::Triangle triangle;
    triangle.Vertices =
    {
        MATH::Vector3f(0.0f, 1.0f, 0.0f), // top
        MATH::Vector3f(-1.0f, -1.0f, 0.0f), // left
        MATH::Vector3f(1.0f, -1.0f, 0.0f) // right
    };

    // DEFINE THE DIFFERENT ROTATION ANGLES AND EXPECTED TRIANGLES.
    const std::shared_ptr<GRAPHICS::Material> NO_MATERIAL = nullptr;
    std::map<float, GRAPHICS::Triangle> rotation_angles_in_degrees_and_expected_triangles =
    {
        {
            30.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    MATH::Vector3f(0.0f, ONE_ROTATED_30_DEGREES, ONE_ROTATED_60_DEGREES),
                    MATH::Vector3f(-1.0f, -ONE_ROTATED_30_DEGREES, -ONE_ROTATED_60_DEGREES),
                    MATH::Vector3f(1.0f, -ONE_ROTATED_30_DEGREES, -ONE_ROTATED_60_DEGREES),
                })
        },
        {
            45.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    MATH::Vector3f(0.0f, ONE_ROTATED_45_DEGREES, ONE_ROTATED_45_DEGREES),
                    MATH::Vector3f(-1.0f, -ONE_ROTATED_45_DEGREES, -ONE_ROTATED_45_DEGREES),
                    MATH::Vector3f( 1.0f, -ONE_ROTATED_45_DEGREES, -ONE_ROTATED_45_DEGREES),
                })
        },
        {
            60.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    MATH::Vector3f(0.0f, ONE_ROTATED_60_DEGREES, ONE_ROTATED_30_DEGREES),
                    MATH::Vector3f(-1.0f, -ONE_ROTATED_60_DEGREES, -ONE_ROTATED_30_DEGREES),
                    MATH::Vector3f(1.0f, -ONE_ROTATED_60_DEGREES, -ONE_ROTATED_30_DEGREES),
                })
        },
        {
            90.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    MATH::Vector3f(0.0f, 0.0f, 1.0f),
                    MATH::Vector3f(-1.0f, 0.0f, -1.0f),
                    MATH::Vector3f(1.0f, 0.0f, -1.0f),
                })
        },
        {
            120.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    MATH::Vector3f(0.0f, -ONE_ROTATED_60_DEGREES, ONE_ROTATED_30_DEGREES),
                    MATH::Vector3f(-1.0f, ONE_ROTATED_60_DEGREES, -ONE_ROTATED_30_DEGREES),
                    MATH::Vector3f(1.0f, ONE_ROTATED_60_DEGREES, -ONE_ROTATED_30_DEGREES),
                })
        },
        {
            135.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    MATH::Vector3f(0.0f, -ONE_ROTATED_45_DEGREES, ONE_ROTATED_45_DEGREES),
                    MATH::Vector3f(-1.0f, ONE_ROTATED_45_DEGREES, -ONE_ROTATED_45_DEGREES),
                    MATH::Vector3f(1.0f, ONE_ROTATED_45_DEGREES, -ONE_ROTATED_45_DEGREES),
                })
        },
        {
            150.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    MATH::Vector3f(0.0f, -ONE_ROTATED_30_DEGREES, ONE_ROTATED_60_DEGREES),
                    MATH::Vector3f(-1.0f, ONE_ROTATED_30_DEGREES, -ONE_ROTATED_60_DEGREES),
                    MATH::Vector3f(1.0f, ONE_ROTATED_30_DEGREES, -ONE_ROTATED_60_DEGREES),
                })
        },
        {
            180.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    MATH::Vector3f(0.0f, -1.0f, 0.0f),
                    MATH::Vector3f(-1.0f, 1.0f, 0.0f),
                    MATH::Vector3f(1.0f, 1.0f, 0.0f),
                })
        },
        {
            210.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    MATH::Vector3f(0.0f, -ONE_ROTATED_30_DEGREES, -ONE_ROTATED_60_DEGREES),
                    MATH::Vector3f(-1.0f, ONE_ROTATED_30_DEGREES, ONE_ROTATED_60_DEGREES),
                    MATH::Vector3f(1.0f, ONE_ROTATED_30_DEGREES, ONE_ROTATED_60_DEGREES),
                })
        },
        {
            225.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    MATH::Vector3f(0.0f, -ONE_ROTATED_45_DEGREES, -ONE_ROTATED_45_DEGREES),
                    MATH::Vector3f(-1.0f, ONE_ROTATED_45_DEGREES, ONE_ROTATED_45_DEGREES),
                    MATH::Vector3f(1.0f, ONE_ROTATED_45_DEGREES, ONE_ROTATED_45_DEGREES),
                })
        },
        {
            240.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    MATH::Vector3f(0.0f, -ONE_ROTATED_60_DEGREES, -ONE_ROTATED_30_DEGREES),
                    MATH::Vector3f(-1.0f, ONE_ROTATED_60_DEGREES, ONE_ROTATED_30_DEGREES),
                    MATH::Vector3f(1.0f, ONE_ROTATED_60_DEGREES, ONE_ROTATED_30_DEGREES),
                })
        },
        {
            270.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    MATH::Vector3f(0.0f, 0.0f, -1.0f),
                    MATH::Vector3f(-1.0f, 0.0f, 1.0f),
                    MATH::Vector3f(1.0f, 0.0f, 1.0f),
                })
        },
        {
            300.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    MATH::Vector3f(0.0f, ONE_ROTATED_60_DEGREES, -ONE_ROTATED_30_DEGREES),
                    MATH::Vector3f(-1.0f, -ONE_ROTATED_60_DEGREES, ONE_ROTATED_30_DEGREES),
                    MATH::Vector3f(1.0f, -ONE_ROTATED_60_DEGREES, ONE_ROTATED_30_DEGREES),
                })
        },
        {
            315.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    MATH::Vector3f(0.0f, ONE_ROTATED_45_DEGREES, -ONE_ROTATED_45_DEGREES),
                    MATH::Vector3f(-1.0f, -ONE_ROTATED_45_DEGREES, ONE_ROTATED_45_DEGREES),
                    MATH::Vector3f(1.0f, -ONE_ROTATED_45_DEGREES, ONE_ROTATED_45_DEGREES),
                })
        },
        {
            330.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    MATH::Vector3f(0.0f, ONE_ROTATED_30_DEGREES, -ONE_ROTATED_60_DEGREES),
                    MATH::Vector3f(-1.0f, -ONE_ROTATED_30_DEGREES, ONE_ROTATED_60_DEGREES),
                    MATH::Vector3f(1.0f, -ONE_ROTATED_30_DEGREES, ONE_ROTATED_60_DEGREES),
                })
        },
        {
            360.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                triangle.Vertices)
        },
        {
            -30.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    MATH::Vector3f(0.0f, ONE_ROTATED_30_DEGREES, -ONE_ROTATED_60_DEGREES),
                    MATH::Vector3f(-1.0f, -ONE_ROTATED_30_DEGREES, ONE_ROTATED_60_DEGREES),
                    MATH::Vector3f(1.0f, -ONE_ROTATED_30_DEGREES, ONE_ROTATED_60_DEGREES),
                })
        },
        {
            -45.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    MATH::Vector3f(0.0f, ONE_ROTATED_45_DEGREES, -ONE_ROTATED_45_DEGREES),
                    MATH::Vector3f(-1.0f, -ONE_ROTATED_45_DEGREES, ONE_ROTATED_45_DEGREES),
                    MATH::Vector3f(1.0f, -ONE_ROTATED_45_DEGREES, ONE_ROTATED_45_DEGREES),
                })
        },
        {
            -60.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    MATH::Vector3f(0.0f, ONE_ROTATED_60_DEGREES, -ONE_ROTATED_30_DEGREES),
                    MATH::Vector3f(-1.0f, -ONE_ROTATED_60_DEGREES, ONE_ROTATED_30_DEGREES),
                    MATH::Vector3f(1.0f, -ONE_ROTATED_60_DEGREES, ONE_ROTATED_30_DEGREES),
                })
        },
        {
            -90.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    MATH::Vector3f(0.0f, 0.0f, -1.0f),
                    MATH::Vector3f(-1.0f, 0.0f, 1.0f),
                    MATH::Vector3f(1.0f, 0.0f, 1.0f),
                })
        },
        {
            -120.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    MATH::Vector3f(0.0f, -ONE_ROTATED_60_DEGREES, -ONE_ROTATED_30_DEGREES),
                    MATH::Vector3f(-1.0f, ONE_ROTATED_60_DEGREES, ONE_ROTATED_30_DEGREES),
                    MATH::Vector3f(1.0f, ONE_ROTATED_60_DEGREES, ONE_ROTATED_30_DEGREES),
                })
        },
        {
            -135.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    MATH::Vector3f(0.0f, -ONE_ROTATED_45_DEGREES, -ONE_ROTATED_45_DEGREES),
                    MATH::Vector3f(-1.0f, ONE_ROTATED_45_DEGREES, ONE_ROTATED_45_DEGREES),
                    MATH::Vector3f(1.0f, ONE_ROTATED_45_DEGREES, ONE_ROTATED_45_DEGREES),
                })
        },
        {
            -150.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    MATH::Vector3f(0.0f, -ONE_ROTATED_30_DEGREES, -ONE_ROTATED_60_DEGREES),
                    MATH::Vector3f(-1.0f, ONE_ROTATED_30_DEGREES, ONE_ROTATED_60_DEGREES),
                    MATH::Vector3f(1.0f, ONE_ROTATED_30_DEGREES, ONE_ROTATED_60_DEGREES),
                })
        },
        {
            -180.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    MATH::Vector3f(0.0f, -1.0f, 0.0f),
                    MATH::Vector3f(-1.0f, 1.0f, 0.0f),
                    MATH::Vector3f(1.0f, 1.0f, 0.0f),
                })
        },
        {
            -210.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    MATH::Vector3f(0.0f, -ONE_ROTATED_30_DEGREES, ONE_ROTATED_60_DEGREES),
                    MATH::Vector3f(-1.0f, ONE_ROTATED_30_DEGREES, -ONE_ROTATED_60_DEGREES),
                    MATH::Vector3f(1.0f, ONE_ROTATED_30_DEGREES, -ONE_ROTATED_60_DEGREES),
                })
        },
        {
            -225.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    MATH::Vector3f(0.0f, -ONE_ROTATED_45_DEGREES, ONE_ROTATED_45_DEGREES),
                    MATH::Vector3f(-1.0f, ONE_ROTATED_45_DEGREES, -ONE_ROTATED_45_DEGREES),
                    MATH::Vector3f(1.0f, ONE_ROTATED_45_DEGREES, -ONE_ROTATED_45_DEGREES),
                })
        },
        {
            -240.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    MATH::Vector3f(0.0f, -ONE_ROTATED_60_DEGREES, ONE_ROTATED_30_DEGREES),
                    MATH::Vector3f(-1.0f, ONE_ROTATED_60_DEGREES, -ONE_ROTATED_30_DEGREES),
                    MATH::Vector3f(1.0f, ONE_ROTATED_60_DEGREES, -ONE_ROTATED_30_DEGREES),
                })
        },
        {
            -270.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    MATH::Vector3f(0.0f, 0.0f, 1.0f),
                    MATH::Vector3f(-1.0f, 0.0f, -1.0f),
                    MATH::Vector3f(1.0f, 0.0f, -1.0f),
                })
        },
        {
            -300.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    MATH::Vector3f(0.0f, ONE_ROTATED_60_DEGREES, ONE_ROTATED_30_DEGREES),
                    MATH::Vector3f(-1.0f, -ONE_ROTATED_60_DEGREES, -ONE_ROTATED_30_DEGREES),
                    MATH::Vector3f(1.0f, -ONE_ROTATED_60_DEGREES, -ONE_ROTATED_30_DEGREES),
                })
        },
        {
            -315.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    MATH::Vector3f(0.0f, ONE_ROTATED_45_DEGREES, ONE_ROTATED_45_DEGREES),
                    MATH::Vector3f(-1.0f, -ONE_ROTATED_45_DEGREES, -ONE_ROTATED_45_DEGREES),
                    MATH::Vector3f(1.0f, -ONE_ROTATED_45_DEGREES, -ONE_ROTATED_45_DEGREES),
                })
        },
        {
            -330.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    MATH::Vector3f(0.0f, ONE_ROTATED_30_DEGREES, ONE_ROTATED_60_DEGREES),
                    MATH::Vector3f(-1.0f, -ONE_ROTATED_30_DEGREES, -ONE_ROTATED_60_DEGREES),
                    MATH::Vector3f(1.0f, -ONE_ROTATED_30_DEGREES, -ONE_ROTATED_60_DEGREES),
                })
        },
        {
            -360.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                triangle.Vertices)
        },
    };

    // TEST EACH ROTATION.
    for (const auto& [rotation_angle_in_degrees, expected_triangle] : rotation_angles_in_degrees_and_expected_triangles)
    {
        // PROVIDE VISIBILITY INTO THE CURRENT TEST CASE.
        INFO("Testing rotation degrees: " + std::to_string(rotation_angle_in_degrees));

        // CREATE A 3D OBJECT.
        GRAPHICS::Object3D test_object_3D;
        test_object_3D.Triangles = { triangle };
        test_object_3D.RotationInRadians = MATH::Vector3<MATH::Angle<float>::Radians>(
            MATH::Angle<float>::DegreesToRadians(MATH::Angle<float>::Degrees(rotation_angle_in_degrees)),
            MATH::Angle<float>::Radians(0.0f),
            MATH::Angle<float>::Radians(0.0f));

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
        constexpr unsigned int TOP_VERTEX_INDEX = 0;
        const MATH::Vector4f EXPECTED_TOP_WORLD_VERTEX = MATH::Vector4f::HomogeneousPositionVector(expected_triangle.Vertices[TOP_VERTEX_INDEX]);
        const MATH::Vector4f& actual_top_world_vertex = world_vertices[0];
        // Due to numerical precision issues, the margin is modified.
        constexpr float APPROXIMATION_ALLOWED_ABSOLUTE_MARGIN = 0.0001f;
        REQUIRE(EXPECTED_TOP_WORLD_VERTEX.X == Approx(actual_top_world_vertex.X));
        REQUIRE(EXPECTED_TOP_WORLD_VERTEX.Y == Approx(actual_top_world_vertex.Y).margin(APPROXIMATION_ALLOWED_ABSOLUTE_MARGIN));
        REQUIRE(EXPECTED_TOP_WORLD_VERTEX.Z == Approx(actual_top_world_vertex.Z).margin(APPROXIMATION_ALLOWED_ABSOLUTE_MARGIN));
        REQUIRE(EXPECTED_TOP_WORLD_VERTEX.W == Approx(actual_top_world_vertex.W));

        constexpr unsigned int LEFT_VERTEX_INDEX = 1;
        const MATH::Vector4f EXPECTED_LEFT_WORLD_VERTEX = MATH::Vector4f::HomogeneousPositionVector(expected_triangle.Vertices[LEFT_VERTEX_INDEX]);
        const MATH::Vector4f& actual_left_world_vertex = world_vertices[1];
        REQUIRE(EXPECTED_LEFT_WORLD_VERTEX.X == Approx(actual_left_world_vertex.X));
        REQUIRE(EXPECTED_LEFT_WORLD_VERTEX.Y == Approx(actual_left_world_vertex.Y).margin(APPROXIMATION_ALLOWED_ABSOLUTE_MARGIN));
        REQUIRE(EXPECTED_LEFT_WORLD_VERTEX.Z == Approx(actual_left_world_vertex.Z).margin(APPROXIMATION_ALLOWED_ABSOLUTE_MARGIN));
        REQUIRE(EXPECTED_LEFT_WORLD_VERTEX.W == Approx(actual_left_world_vertex.W));

        constexpr unsigned int RIGHT_VERTEX_INDEX = 2;
        const MATH::Vector4f EXPECTED_RIGHT_WORLD_VERTEX = MATH::Vector4f::HomogeneousPositionVector(expected_triangle.Vertices[RIGHT_VERTEX_INDEX]);;
        const MATH::Vector4f& actual_right_world_vertex = world_vertices[2];
        REQUIRE(EXPECTED_RIGHT_WORLD_VERTEX.X == Approx(actual_right_world_vertex.X));
        REQUIRE(EXPECTED_RIGHT_WORLD_VERTEX.Y == Approx(actual_right_world_vertex.Y).margin(APPROXIMATION_ALLOWED_ABSOLUTE_MARGIN));
        REQUIRE(EXPECTED_RIGHT_WORLD_VERTEX.Z == Approx(actual_right_world_vertex.Z).margin(APPROXIMATION_ALLOWED_ABSOLUTE_MARGIN));
        REQUIRE(EXPECTED_RIGHT_WORLD_VERTEX.W == Approx(actual_right_world_vertex.W));
    }
}

TEST_CASE("World transform can rotate a vector around the y-axis by common locations on unit circle.", "[Object3D][WorldTransform][Rotation]")
{
    // DEFINE THE BASE TRIANGLE TO ROTATE.
    GRAPHICS::Triangle triangle;
    triangle.Vertices =
    {
        MATH::Vector3f(0.0f, 1.0f, 0.0f), // top
        MATH::Vector3f(-1.0f, -1.0f, 0.0f), // left
        MATH::Vector3f(1.0f, -1.0f, 0.0f) // right
    };

    // DEFINE THE DIFFERENT ROTATION ANGLES AND EXPECTED TRIANGLES.
    const std::shared_ptr<GRAPHICS::Material> NO_MATERIAL = nullptr;
    std::map<float, GRAPHICS::Triangle> rotation_angles_in_degrees_and_expected_triangles =
    {
        {
            30.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    triangle.Vertices[0],
                    MATH::Vector3f(-ONE_ROTATED_30_DEGREES, -1.0f, ONE_ROTATED_60_DEGREES),
                    MATH::Vector3f(ONE_ROTATED_30_DEGREES, -1.0f, -ONE_ROTATED_60_DEGREES),
                })
        },
        {
            45.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    triangle.Vertices[0],
                    MATH::Vector3f(-ONE_ROTATED_45_DEGREES, -1.0f, ONE_ROTATED_45_DEGREES),
                    MATH::Vector3f(ONE_ROTATED_45_DEGREES, -1.0f, -ONE_ROTATED_45_DEGREES),
                })
        },
        {
            60.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    triangle.Vertices[0],
                    MATH::Vector3f(-ONE_ROTATED_60_DEGREES, -1.0f, ONE_ROTATED_30_DEGREES),
                    MATH::Vector3f(ONE_ROTATED_60_DEGREES, -1.0f, -ONE_ROTATED_30_DEGREES),
                })
        },
        {
            90.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    triangle.Vertices[0],
                    MATH::Vector3f(0.0f, -1.0f, 1.0f),
                    MATH::Vector3f(0.0f, -1.0f, -1.0f),
                })
        },
        {
            120.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    triangle.Vertices[0],
                    MATH::Vector3f(ONE_ROTATED_60_DEGREES, -1.0f, ONE_ROTATED_30_DEGREES),
                    MATH::Vector3f(-ONE_ROTATED_60_DEGREES, -1.0f, -ONE_ROTATED_30_DEGREES),
                })
        },
        {
            135.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    triangle.Vertices[0],
                    MATH::Vector3f(ONE_ROTATED_45_DEGREES, -1.0f, ONE_ROTATED_45_DEGREES),
                    MATH::Vector3f(-ONE_ROTATED_45_DEGREES, -1.0f, -ONE_ROTATED_45_DEGREES),
                })
        },
        {
            150.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    triangle.Vertices[0],
                    MATH::Vector3f(ONE_ROTATED_30_DEGREES, -1.0f, ONE_ROTATED_60_DEGREES),
                    MATH::Vector3f(-ONE_ROTATED_30_DEGREES, -1.0f, -ONE_ROTATED_60_DEGREES),
                })
        },
        {
            180.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    triangle.Vertices[0],
                    MATH::Vector3f(1.0f, -1.0f, 0.0f),
                    MATH::Vector3f(-1.0f, -1.0f, 0.0f),
                })
        },
        {
            210.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    triangle.Vertices[0],
                    MATH::Vector3f(ONE_ROTATED_30_DEGREES, -1.0f, -ONE_ROTATED_60_DEGREES),
                    MATH::Vector3f(-ONE_ROTATED_30_DEGREES, -1.0f, ONE_ROTATED_60_DEGREES),
                })
        },
        {
            225.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    triangle.Vertices[0],
                    MATH::Vector3f(ONE_ROTATED_45_DEGREES, -1.0f, -ONE_ROTATED_45_DEGREES),
                    MATH::Vector3f(-ONE_ROTATED_45_DEGREES, -1.0f, ONE_ROTATED_45_DEGREES),
                })
        },
        {
            240.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    triangle.Vertices[0],
                    MATH::Vector3f(ONE_ROTATED_60_DEGREES, -1.0f, -ONE_ROTATED_30_DEGREES),
                    MATH::Vector3f(-ONE_ROTATED_60_DEGREES, -1.0f, ONE_ROTATED_30_DEGREES),
                })
        },
        {
            270.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    triangle.Vertices[0],
                    MATH::Vector3f(0.0f, -1.0f, -1.0f),
                    MATH::Vector3f(0.0f, -1.0f, 1.0f),
                })
        },
        {
            300.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    triangle.Vertices[0],
                    MATH::Vector3f(-ONE_ROTATED_60_DEGREES, -1.0f, -ONE_ROTATED_30_DEGREES),
                    MATH::Vector3f(ONE_ROTATED_60_DEGREES, -1.0f, ONE_ROTATED_30_DEGREES),
                })
        },
        {
            315.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    triangle.Vertices[0],
                    MATH::Vector3f(-ONE_ROTATED_45_DEGREES, -1.0f, -ONE_ROTATED_45_DEGREES),
                    MATH::Vector3f(ONE_ROTATED_45_DEGREES, -1.0f, ONE_ROTATED_45_DEGREES),
                })
        },
        {
            330.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    triangle.Vertices[0],
                    MATH::Vector3f(-ONE_ROTATED_30_DEGREES, -1.0f, -ONE_ROTATED_60_DEGREES),
                    MATH::Vector3f(ONE_ROTATED_30_DEGREES, -1.0f, ONE_ROTATED_60_DEGREES),
                })
        },
        {
            360.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                triangle.Vertices)
        },
        {
            -30.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    triangle.Vertices[0],
                    MATH::Vector3f(-ONE_ROTATED_30_DEGREES, -1.0f, -ONE_ROTATED_60_DEGREES),
                    MATH::Vector3f(ONE_ROTATED_30_DEGREES, -1.0f, ONE_ROTATED_60_DEGREES),
                })
        },
        {
            -45.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    triangle.Vertices[0],
                    MATH::Vector3f(-ONE_ROTATED_45_DEGREES, -1.0f, -ONE_ROTATED_45_DEGREES),
                    MATH::Vector3f(ONE_ROTATED_45_DEGREES, -1.0f, ONE_ROTATED_45_DEGREES),
                })
        },
        {
            -60.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    triangle.Vertices[0],
                    MATH::Vector3f(-ONE_ROTATED_60_DEGREES, -1.0f, -ONE_ROTATED_30_DEGREES),
                    MATH::Vector3f(ONE_ROTATED_60_DEGREES, -1.0f, ONE_ROTATED_30_DEGREES),
                })
        },
        {
            -90.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    triangle.Vertices[0],
                    MATH::Vector3f(0.0f, -1.0f, -1.0f),
                    MATH::Vector3f(0.0f, -1.0f, 1.0f),
                })
        },
        {
            -120.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    triangle.Vertices[0],
                    MATH::Vector3f(ONE_ROTATED_60_DEGREES, -1.0f, -ONE_ROTATED_30_DEGREES),
                    MATH::Vector3f(-ONE_ROTATED_60_DEGREES, -1.0f, ONE_ROTATED_30_DEGREES),
                })
        },
        {
            -135.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    triangle.Vertices[0],
                    MATH::Vector3f(ONE_ROTATED_45_DEGREES, -1.0f, -ONE_ROTATED_45_DEGREES),
                    MATH::Vector3f(-ONE_ROTATED_45_DEGREES, -1.0f, ONE_ROTATED_45_DEGREES),
                })
        },
        {
            -150.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    triangle.Vertices[0],
                    MATH::Vector3f(ONE_ROTATED_30_DEGREES, -1.0f, -ONE_ROTATED_60_DEGREES),
                    MATH::Vector3f(-ONE_ROTATED_30_DEGREES, -1.0f, ONE_ROTATED_60_DEGREES),
                })
        },
        {
            -180.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    triangle.Vertices[0],
                    MATH::Vector3f(1.0f, -1.0f, 0.0f),
                    MATH::Vector3f(-1.0f, -1.0f, 0.0f),
                })
        },
        {
            -210.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    triangle.Vertices[0],
                    MATH::Vector3f(ONE_ROTATED_30_DEGREES, -1.0f, ONE_ROTATED_60_DEGREES),
                    MATH::Vector3f(-ONE_ROTATED_30_DEGREES, -1.0f, -ONE_ROTATED_60_DEGREES),
                })
        },
        {
            -225.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    triangle.Vertices[0],
                    MATH::Vector3f(ONE_ROTATED_45_DEGREES, -1.0f, ONE_ROTATED_45_DEGREES),
                    MATH::Vector3f(-ONE_ROTATED_45_DEGREES, -1.0f, -ONE_ROTATED_45_DEGREES),
                })
        },
        {
            -240.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    triangle.Vertices[0],
                    MATH::Vector3f(ONE_ROTATED_60_DEGREES, -1.0f, ONE_ROTATED_30_DEGREES),
                    MATH::Vector3f(-ONE_ROTATED_60_DEGREES, -1.0f, -ONE_ROTATED_30_DEGREES),
                })
        },
        {
            -270.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    triangle.Vertices[0],
                    MATH::Vector3f(0.0f, -1.0f, 1.0f),
                    MATH::Vector3f(0.0f, -1.0f, -1.0f),
                })
        },
        {
            -300.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    triangle.Vertices[0],
                    MATH::Vector3f(-ONE_ROTATED_60_DEGREES, -1.0f, ONE_ROTATED_30_DEGREES),
                    MATH::Vector3f(ONE_ROTATED_60_DEGREES, -1.0f, -ONE_ROTATED_30_DEGREES),
                })
        },
        {
            -315.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    triangle.Vertices[0],
                    MATH::Vector3f(-ONE_ROTATED_45_DEGREES, -1.0f, ONE_ROTATED_45_DEGREES),
                    MATH::Vector3f(ONE_ROTATED_45_DEGREES, -1.0f, -ONE_ROTATED_45_DEGREES),
                })
        },
        {
            -330.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    triangle.Vertices[0],
                    MATH::Vector3f(-ONE_ROTATED_30_DEGREES, -1.0f, ONE_ROTATED_60_DEGREES),
                    MATH::Vector3f(ONE_ROTATED_30_DEGREES, -1.0f, -ONE_ROTATED_60_DEGREES),
                })
        },
        {
            -360.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                triangle.Vertices)
        },
    };

    // TEST EACH ROTATION.
    for (const auto& [rotation_angle_in_degrees, expected_triangle] : rotation_angles_in_degrees_and_expected_triangles)
    {
        // PROVIDE VISIBILITY INTO THE CURRENT TEST CASE.
        INFO("Testing rotation degrees: " + std::to_string(rotation_angle_in_degrees));

        // CREATE A 3D OBJECT.
        GRAPHICS::Object3D test_object_3D;
        test_object_3D.Triangles = { triangle };
        test_object_3D.RotationInRadians = MATH::Vector3<MATH::Angle<float>::Radians>(
            MATH::Angle<float>::Radians(0.0f),
            MATH::Angle<float>::DegreesToRadians(MATH::Angle<float>::Degrees(rotation_angle_in_degrees)),
            MATH::Angle<float>::Radians(0.0f));

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
        constexpr unsigned int TOP_VERTEX_INDEX = 0;
        const MATH::Vector4f EXPECTED_TOP_WORLD_VERTEX = MATH::Vector4f::HomogeneousPositionVector(expected_triangle.Vertices[TOP_VERTEX_INDEX]);
        const MATH::Vector4f& actual_top_world_vertex = world_vertices[0];
        // Due to numerical precision issues, the margin is modified.
        constexpr float APPROXIMATION_ALLOWED_ABSOLUTE_MARGIN = 0.0001f;
        REQUIRE(EXPECTED_TOP_WORLD_VERTEX.X == Approx(actual_top_world_vertex.X).margin(APPROXIMATION_ALLOWED_ABSOLUTE_MARGIN));
        REQUIRE(EXPECTED_TOP_WORLD_VERTEX.Y == Approx(actual_top_world_vertex.Y).margin(APPROXIMATION_ALLOWED_ABSOLUTE_MARGIN));
        REQUIRE(EXPECTED_TOP_WORLD_VERTEX.Z == Approx(actual_top_world_vertex.Z));
        REQUIRE(EXPECTED_TOP_WORLD_VERTEX.W == Approx(actual_top_world_vertex.W));

        constexpr unsigned int LEFT_VERTEX_INDEX = 1;
        const MATH::Vector4f EXPECTED_LEFT_WORLD_VERTEX = MATH::Vector4f::HomogeneousPositionVector(expected_triangle.Vertices[LEFT_VERTEX_INDEX]);
        const MATH::Vector4f& actual_left_world_vertex = world_vertices[1];
        REQUIRE(EXPECTED_LEFT_WORLD_VERTEX.X == Approx(actual_left_world_vertex.X).margin(APPROXIMATION_ALLOWED_ABSOLUTE_MARGIN));
        REQUIRE(EXPECTED_LEFT_WORLD_VERTEX.Y == Approx(actual_left_world_vertex.Y));
        REQUIRE(EXPECTED_LEFT_WORLD_VERTEX.Z == Approx(actual_left_world_vertex.Z).margin(APPROXIMATION_ALLOWED_ABSOLUTE_MARGIN));
        REQUIRE(EXPECTED_LEFT_WORLD_VERTEX.W == Approx(actual_left_world_vertex.W));

        constexpr unsigned int RIGHT_VERTEX_INDEX = 2;
        const MATH::Vector4f EXPECTED_RIGHT_WORLD_VERTEX = MATH::Vector4f::HomogeneousPositionVector(expected_triangle.Vertices[RIGHT_VERTEX_INDEX]);;
        const MATH::Vector4f& actual_right_world_vertex = world_vertices[2];
        REQUIRE(EXPECTED_RIGHT_WORLD_VERTEX.X == Approx(actual_right_world_vertex.X).margin(APPROXIMATION_ALLOWED_ABSOLUTE_MARGIN));
        REQUIRE(EXPECTED_RIGHT_WORLD_VERTEX.Y == Approx(actual_right_world_vertex.Y));
        REQUIRE(EXPECTED_RIGHT_WORLD_VERTEX.Z == Approx(actual_right_world_vertex.Z).margin(APPROXIMATION_ALLOWED_ABSOLUTE_MARGIN));
        REQUIRE(EXPECTED_RIGHT_WORLD_VERTEX.W == Approx(actual_right_world_vertex.W));
    }
}

TEST_CASE("World transform can rotate a vector around the all three cardinal axes.", "[Object3D][WorldTransform][Rotation]")
{
    // DEFINE A VECTOR TO ROTATE.
    MATH::Vector4f vector_to_rotate(1.0f, 0.5f, -1.0f, 1.0f);

    // DEFINE AN OBJECT WORLD TRANSFORM.
    GRAPHICS::Object3D object;
    object.RotationInRadians = MATH::Vector3<MATH::Angle<float>::Radians>(
        MATH::Angle<float>::DegreesToRadians(MATH::Angle<float>::Degrees(30.0f)),
        MATH::Angle<float>::DegreesToRadians(MATH::Angle<float>::Degrees(-45.0f)),
        MATH::Angle<float>::DegreesToRadians(MATH::Angle<float>::Degrees(60.0f)));

    // TRANSFORM THE VECTOR TO WORLD SPACE.
    MATH::Matrix4x4f world_transform = object.WorldTransform();
    MATH::Vector4f world_vertex = world_transform * vector_to_rotate;

    // VERIFY THE ROTATION.
    // Note that math below is approximate and has some rounding errors.
    // This math assumes an XYZ rotation order.
    // After x axis rotation:
    //      x = x = 1
    //      y = y*cos(30) - z*sin(30) = y*sqrt(3)/2 - z*1/2 = 0.5*sqrt(3)/2 + 1/2 = 0.433013 + 0.5 = 0.933013
    //      z = y*sin(30) + z*cos(30) = y*1/2 + z*sqrt(3)/2 = 0.5/2 + -1*sqrt(3)/2 = 0.25 - 0.86603 = -0.61603
    // After y axis rotation:
    //      x = x*cos(-45) + z*sin(-45) = x*sqrt(2)/2 - z*sqrt(2)/2 = sqrt(2)/2 + 0.61603*sqrt(2)/2 = 1.61603*sqrt(2)/2 = 1.61603*0.70711 = 1.14271
    //      y = y = 0.933013
    //      z = -x*sin(-45) + z*cos(45) = -x*-sqrt(2)/2 + z*sqrt(2)/2 = sqrt(2)/2 - 0.61603*sqrt(2)/2 = 0.38397*sqrt(2)/2 = 0.38397*0.70711 = 0.27151
    // After z axis rotation:
    //      x = x*cos(60) - y*sin(60) = x*1/2 - y*sqrt(3)/2 = 0.57136 - 0.933013*0.86603 = 0.57136 - 0.80814 = -0.23678
    //      y = x*sin(60) + y*cos(60) = x*sqrt(3)/2 + y*1/2 = 1.14271*sqrt(3)/2 + 0.933013/2 = 1.14271*0.86603 + 0.46651 = 0.98962 + 0.46651 = 1.45613
    //      z = z = 0.27151
    constexpr float EXPECTED_X = -0.23666f;
    constexpr float EXPECTED_Y = 1.45612f;
    constexpr float EXPECTED_Z = 0.27151f;
    MATH::Vector4f EXPECTED_ROTATED_VERTEX(
        EXPECTED_X,
        EXPECTED_Y,
        EXPECTED_Z,
        1.0f);

    // Due to numerical precision issues, the margin is modified.
    constexpr float APPROXIMATION_ALLOWED_ABSOLUTE_MARGIN = 0.0001f;
    REQUIRE(EXPECTED_ROTATED_VERTEX.X == Approx(world_vertex.X).margin(APPROXIMATION_ALLOWED_ABSOLUTE_MARGIN));
    REQUIRE(EXPECTED_ROTATED_VERTEX.Y == Approx(world_vertex.Y).margin(APPROXIMATION_ALLOWED_ABSOLUTE_MARGIN));
    REQUIRE(EXPECTED_ROTATED_VERTEX.Z == Approx(world_vertex.Z).margin(APPROXIMATION_ALLOWED_ABSOLUTE_MARGIN));
    REQUIRE(EXPECTED_ROTATED_VERTEX.W == Approx(world_vertex.W));
}

TEST_CASE("World transform can scale, rotate, and translate a vector.", "[Object3D][WorldTransform][Rotation]")
{
    // DEFINE A VECTOR TO TRANSFORM.
    MATH::Vector4f vector_to_rotate(1.0f, 0.5f, -1.0f, 1.0f);

    // DEFINE AN OBJECT WORLD TRANSFORM.
    GRAPHICS::Object3D object;
    object.WorldPosition = MATH::Vector3f(5.0f, -6.0f, 7.0f);
    // A uniform (rather than non-uniform) scale is chosen to avoid having to recompute
    // data for the rotation (and thus re-use the numbers from the previous test case).
    object.Scale = MATH::Vector3f(2.0f, 2.0f, 2.0f);
    object.RotationInRadians = MATH::Vector3<MATH::Angle<float>::Radians>(
        MATH::Angle<float>::DegreesToRadians(MATH::Angle<float>::Degrees(30.0f)),
        MATH::Angle<float>::DegreesToRadians(MATH::Angle<float>::Degrees(-45.0f)),
        MATH::Angle<float>::DegreesToRadians(MATH::Angle<float>::Degrees(60.0f)));

    // TRANSFORM THE VECTOR TO WORLD SPACE.
    MATH::Matrix4x4f world_transform = object.WorldTransform();
    MATH::Vector4f world_vertex = world_transform * vector_to_rotate;

    // VERIFY THE TRANSFORMATION.
    // Rotation numbers come from previous test case.
    // Ordering of numbers below is Scale * Rotation + Translation.
    constexpr float EXPECTED_X = 2.0f * -0.23666f + 5.0f;
    constexpr float EXPECTED_Y = 2.0f * 1.45612f - 6.0f;
    constexpr float EXPECTED_Z = 2.0f * 0.27151f + 7.0f;
    MATH::Vector4f EXPECTED_TRANSFORMED_VERTEX(
        EXPECTED_X,
        EXPECTED_Y,
        EXPECTED_Z,
        1.0f);

    // Due to numerical precision issues, the margin is modified.
    constexpr float APPROXIMATION_ALLOWED_ABSOLUTE_MARGIN = 0.0001f;
    REQUIRE(EXPECTED_TRANSFORMED_VERTEX.X == Approx(world_vertex.X).margin(APPROXIMATION_ALLOWED_ABSOLUTE_MARGIN));
    REQUIRE(EXPECTED_TRANSFORMED_VERTEX.Y == Approx(world_vertex.Y).margin(APPROXIMATION_ALLOWED_ABSOLUTE_MARGIN));
    REQUIRE(EXPECTED_TRANSFORMED_VERTEX.Z == Approx(world_vertex.Z).margin(APPROXIMATION_ALLOWED_ABSOLUTE_MARGIN));
    REQUIRE(EXPECTED_TRANSFORMED_VERTEX.W == Approx(world_vertex.W));
}

TEST_CASE("World transform can non-uniform scale a vector.", "[Object3D][WorldTransform][Rotation]")
{
    // DEFINE A VECTOR TO TRANSFORM.
    MATH::Vector4f vector_to_rotate(1.0f, 0.5f, -1.0f, 1.0f);

    // DEFINE AN OBJECT WORLD TRANSFORM.
    GRAPHICS::Object3D object;
    object.Scale = MATH::Vector3f(2.0f, 3.0f, 4.0f);

    // TRANSFORM THE VECTOR TO WORLD SPACE.
    MATH::Matrix4x4f world_transform = object.WorldTransform();
    MATH::Vector4f world_vertex = world_transform * vector_to_rotate;

    // VERIFY THE TRANSFORMATION.
    REQUIRE(2.0f == world_vertex.X);
    REQUIRE(1.5f == world_vertex.Y);
    REQUIRE(-4.0f == world_vertex.Z);
    REQUIRE(1.0f == world_vertex.W);
}
