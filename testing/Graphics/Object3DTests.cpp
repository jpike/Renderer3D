#include <cmath>
#include <map>
#include "Graphics/Object3D.h"
#include "ThirdParty/Catch/catch.hpp"

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
                        -0.5f,
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
                        0.36603f, // 1*sin(120) + -1*cos(120) = sqrt(3)/2 - 1/2
                        0.0f),
                })
        },
        {
            135.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    MATH::Vector3f(
                        0.0f),
                    MATH::Vector3f(
                        0.0f),
                    MATH::Vector3f(
                        0.0f),
                })
        },
        {
            150.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    MATH::Vector3f(
                        0.0f),
                    MATH::Vector3f(
                        0.0f),
                    MATH::Vector3f(
                        0.0f),
                })
        },
        {
            180.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    MATH::Vector3f(
                        0.0f),
                    MATH::Vector3f(
                        0.0f),
                    MATH::Vector3f(
                        0.0f),
                })
        },
        {
            210.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    MATH::Vector3f(
                        0.0f),
                    MATH::Vector3f(
                        0.0f),
                    MATH::Vector3f(
                        0.0f),
                })
        },
        {
            225.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    MATH::Vector3f(
                        0.0f),
                    MATH::Vector3f(
                        0.0f),
                    MATH::Vector3f(
                        0.0f),
                })
        },
        {
            240.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    MATH::Vector3f(
                        0.0f),
                    MATH::Vector3f(
                        0.0f),
                    MATH::Vector3f(
                        0.0f),
                })
        },
        {
            270.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    MATH::Vector3f(
                        0.0f),
                    MATH::Vector3f(
                        0.0f),
                    MATH::Vector3f(
                        0.0f),
                })
        },
        {
            300.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    MATH::Vector3f(
                        0.0f),
                    MATH::Vector3f(
                        0.0f),
                    MATH::Vector3f(
                        0.0f),
                })
        },
        {
            315.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    MATH::Vector3f(
                        0.0f),
                    MATH::Vector3f(
                        0.0f),
                    MATH::Vector3f(
                        0.0f),
                })
        },
        {
            330.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    MATH::Vector3f(
                        0.0f),
                    MATH::Vector3f(
                        0.0f),
                    MATH::Vector3f(
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
                        0.0f),
                    MATH::Vector3f(
                        0.0f),
                    MATH::Vector3f(
                        0.0f),
                })
        },
        {
            -45.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    MATH::Vector3f(
                        0.0f),
                    MATH::Vector3f(
                        0.0f),
                    MATH::Vector3f(
                        0.0f),
                })
        },
        {
            -60.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    MATH::Vector3f(
                        0.0f),
                    MATH::Vector3f(
                        0.0f),
                    MATH::Vector3f(
                        0.0f),
                })
        },
        {
            -90.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    MATH::Vector3f(
                        0.0f),
                    MATH::Vector3f(
                        0.0f),
                    MATH::Vector3f(
                        0.0f),
                })
        },
        {
            -120.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    MATH::Vector3f(
                        0.0f),
                    MATH::Vector3f(
                        0.0f),
                    MATH::Vector3f(
                        0.0f),
                })
        },
        {
            -135.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    MATH::Vector3f(
                        0.0f),
                    MATH::Vector3f(
                        0.0f),
                    MATH::Vector3f(
                        0.0f),
                })
        },
        {
            -150.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    MATH::Vector3f(
                        0.0f),
                    MATH::Vector3f(
                        0.0f),
                    MATH::Vector3f(
                        0.0f),
                })
        },
        {
            -180.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    MATH::Vector3f(
                        0.0f),
                    MATH::Vector3f(
                        0.0f),
                    MATH::Vector3f(
                        0.0f),
                })
        },
        {
            -210.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    MATH::Vector3f(
                        0.0f),
                    MATH::Vector3f(
                        0.0f),
                    MATH::Vector3f(
                        0.0f),
                })
        },
        {
            -225.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    MATH::Vector3f(
                        0.0f),
                    MATH::Vector3f(
                        0.0f),
                    MATH::Vector3f(
                        0.0f),
                })
        },
        {
            -240.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    MATH::Vector3f(
                        0.0f),
                    MATH::Vector3f(
                        0.0f),
                    MATH::Vector3f(
                        0.0f),
                })
        },
        {
            -270.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    MATH::Vector3f(
                        0.0f),
                    MATH::Vector3f(
                        0.0f),
                    MATH::Vector3f(
                        0.0f),
                })
        },
        {
            -300.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    MATH::Vector3f(
                        0.0f),
                    MATH::Vector3f(
                        0.0f),
                    MATH::Vector3f(
                        0.0f),
                })
        },
        {
            -315.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    MATH::Vector3f(
                        0.0f),
                    MATH::Vector3f(
                        0.0f),
                    MATH::Vector3f(
                        0.0f),
                })
        },
        {
            -330.0f,
            GRAPHICS::Triangle(
                NO_MATERIAL,
                {
                    MATH::Vector3f(
                        0.0f),
                    MATH::Vector3f(
                        0.0f),
                    MATH::Vector3f(
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
        constexpr float APPROXIMATION_ALLOWED_ABSOLUTE_MARGIN = 0.00001f;
        REQUIRE(EXPECTED_TOP_WORLD_VERTEX.X == Approx(actual_top_world_vertex.X).margin(APPROXIMATION_ALLOWED_ABSOLUTE_MARGIN));
        REQUIRE(EXPECTED_TOP_WORLD_VERTEX.Y == Approx(actual_top_world_vertex.Y));
        REQUIRE(EXPECTED_TOP_WORLD_VERTEX.Z == Approx(actual_top_world_vertex.Z));
        REQUIRE(EXPECTED_TOP_WORLD_VERTEX.W == Approx(actual_top_world_vertex.W));

        constexpr unsigned int LEFT_VERTEX_INDEX = 1;
        const MATH::Vector4f EXPECTED_LEFT_WORLD_VERTEX = MATH::Vector4f::HomogeneousPositionVector(expected_triangle.Vertices[LEFT_VERTEX_INDEX]);
        const MATH::Vector4f& actual_left_world_vertex = world_vertices[1];
        REQUIRE(EXPECTED_LEFT_WORLD_VERTEX.X == Approx(actual_left_world_vertex.X).margin(APPROXIMATION_ALLOWED_ABSOLUTE_MARGIN));
        REQUIRE(EXPECTED_LEFT_WORLD_VERTEX.Y == Approx(actual_left_world_vertex.Y));
        REQUIRE(EXPECTED_LEFT_WORLD_VERTEX.Z == Approx(actual_left_world_vertex.Z));
        REQUIRE(EXPECTED_LEFT_WORLD_VERTEX.W == Approx(actual_left_world_vertex.W));

        constexpr unsigned int RIGHT_VERTEX_INDEX = 2;
        const MATH::Vector4f EXPECTED_RIGHT_WORLD_VERTEX = MATH::Vector4f::HomogeneousPositionVector(expected_triangle.Vertices[RIGHT_VERTEX_INDEX]);;
        const MATH::Vector4f& actual_right_world_vertex = world_vertices[2];
        REQUIRE(EXPECTED_RIGHT_WORLD_VERTEX.X == Approx(actual_right_world_vertex.X));
        REQUIRE(EXPECTED_RIGHT_WORLD_VERTEX.Y == Approx(actual_right_world_vertex.Y).margin(APPROXIMATION_ALLOWED_ABSOLUTE_MARGIN));
        REQUIRE(EXPECTED_RIGHT_WORLD_VERTEX.Z == Approx(actual_right_world_vertex.Z));
        REQUIRE(EXPECTED_RIGHT_WORLD_VERTEX.W == Approx(actual_right_world_vertex.W));
    }
}
