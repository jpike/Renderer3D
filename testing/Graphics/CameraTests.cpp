#include "Graphics/Camera.h"
#include "ThirdParty/Catch/catch.hpp"

TEST_CASE("View transform can transform world space coordinates to camera space coordinates.", "[Camera][ViewTransform]")
{
    // CREATE THE COORDINATES TO TRANSFORM.
    // These form a basic triangle.
    MATH::Vector4f top_coordinate(0.0f, 1.0f, 0.0f, 1.0f);
    MATH::Vector4f left_coordinate(-1.0f, 0.0f, 0.0f, 1.0f);
    MATH::Vector4f right_coordinate(1.0f, 0.0f, 0.0f, 1.0f);

    // DEFINE A BASIC CAMERA.
    GRAPHICS::Camera camera = GRAPHICS::Camera::LookAtFrom(MATH::Vector3f(0.0f, 0.0f, 0.0f), MATH::Vector3f(0.0f, 0.0f, -1.0f));

    // CREATE THE VIEW TRANSFORM.
    MATH::Matrix4x4f view_transform = camera.ViewTransform();

    // TRANSFORM THE COORDINATES.
    MATH::Vector4f transformed_top_coordinate = view_transform * top_coordinate;
    MATH::Vector4f transformed_left_coordinate = view_transform * left_coordinate;
    MATH::Vector4f transformed_right_coordinate = view_transform * right_coordinate;

    // VERIFY THE TRANSFORMED COORDINATES.
    const MATH::Vector4f EXPECTED_TRANSFORMED_TOP_COORDINATE(0.0f, 1.0f, -1.0f, 1.0f);
    REQUIRE(EXPECTED_TRANSFORMED_TOP_COORDINATE.X == transformed_top_coordinate.X);
    REQUIRE(EXPECTED_TRANSFORMED_TOP_COORDINATE.Y == transformed_top_coordinate.Y);
    REQUIRE(EXPECTED_TRANSFORMED_TOP_COORDINATE.Z == transformed_top_coordinate.Z);
    REQUIRE(EXPECTED_TRANSFORMED_TOP_COORDINATE.W == transformed_top_coordinate.W);

    const MATH::Vector4f EXPECTED_TRANSFORMED_LEFT_COORDINATE(1.0f, 0.0f, -1.0f, 1.0f);
    REQUIRE(EXPECTED_TRANSFORMED_LEFT_COORDINATE.X == transformed_left_coordinate.X);
    REQUIRE(EXPECTED_TRANSFORMED_LEFT_COORDINATE.Y == transformed_left_coordinate.Y);
    REQUIRE(EXPECTED_TRANSFORMED_LEFT_COORDINATE.Z == transformed_left_coordinate.Z);
    REQUIRE(EXPECTED_TRANSFORMED_LEFT_COORDINATE.W == transformed_left_coordinate.W);

    const MATH::Vector4f EXPECTED_TRANSFORMED_RIGHT_COORDINATE(-1.0f, 0.0f, -1.0f, 1.0f);
    REQUIRE(EXPECTED_TRANSFORMED_RIGHT_COORDINATE.X == transformed_right_coordinate.X);
    REQUIRE(EXPECTED_TRANSFORMED_RIGHT_COORDINATE.Y == transformed_right_coordinate.Y);
    REQUIRE(EXPECTED_TRANSFORMED_RIGHT_COORDINATE.Z == transformed_right_coordinate.Z);
    REQUIRE(EXPECTED_TRANSFORMED_RIGHT_COORDINATE.W == transformed_right_coordinate.W);
}

TEST_CASE("Perspective projection projects camera space coordinates correctly.", "[Camera][Perspective]")
{
    // CREATE THE CAMERA SPACE COORDINATES.
    // These are the same coordinates from the previous test case.
    MATH::Vector4f camera_space_top_coordinate(0.0f, 1.0f, -1.0f, 1.0f);
    MATH::Vector4f camera_space_left_coordinate(1.0f, 0.0f, -1.0f, 1.0f);
    MATH::Vector4f camera_space_right_coordinate(-1.0f, 0.0f, -1.0f, 1.0f);

    // DEFINE A BASIC CAMERA.
    GRAPHICS::Camera camera = GRAPHICS::Camera::LookAt(MATH::Vector3f(0.0f, 0.0f, 0.0f));

    // CREATE THE PERSPECTIVE PROJECTION MATRIX.
    const MATH::Angle<float>::Degrees FIELD_OF_VIEW(90.0f);
    constexpr float ASPECT_RATIO = 1.0f;
    constexpr float Z_NEAR = 1.0f;
    constexpr float Z_FAR = 100.0f;
    MATH::Matrix4x4f perspective_projection = camera.PerspectiveProjection(FIELD_OF_VIEW, ASPECT_RATIO, Z_NEAR, Z_FAR);

    // TRANSFORM THE COORDINATES.
    MATH::Vector4f projected_top_coordinate = perspective_projection * camera_space_top_coordinate;
    MATH::Vector4f projected_left_coordinate = perspective_projection * camera_space_left_coordinate;
    MATH::Vector4f projected_right_coordinate = perspective_projection * camera_space_right_coordinate;

    // VERIFY THE TRANSFORMED COORDINATES.
    const MATH::Vector4f EXPECTED_PROJECTED_TOP_COORDINATE(0.0f, 1.0f, 3.0404f, -1.0f);
    REQUIRE(EXPECTED_PROJECTED_TOP_COORDINATE.X == Approx(projected_top_coordinate.X));
    REQUIRE(EXPECTED_PROJECTED_TOP_COORDINATE.Y == Approx(projected_top_coordinate.Y));
    REQUIRE(EXPECTED_PROJECTED_TOP_COORDINATE.Z == Approx(projected_top_coordinate.Z));
    REQUIRE(EXPECTED_PROJECTED_TOP_COORDINATE.W == Approx(projected_top_coordinate.W));

    const MATH::Vector4f EXPECTED_PROJECTED_LEFT_COORDINATE(1.0f, 0.0f, 3.0404f, -1.0f);
    REQUIRE(EXPECTED_PROJECTED_LEFT_COORDINATE.X == Approx(projected_left_coordinate.X));
    REQUIRE(EXPECTED_PROJECTED_LEFT_COORDINATE.Y == Approx(projected_left_coordinate.Y));
    REQUIRE(EXPECTED_PROJECTED_LEFT_COORDINATE.Z == Approx(projected_left_coordinate.Z));
    REQUIRE(EXPECTED_PROJECTED_LEFT_COORDINATE.W == Approx(projected_left_coordinate.W));

    const MATH::Vector4f EXPECTED_PROJECTED_RIGHT_COORDINATE(-1.0f, 0.0f, 3.0404f, -1.0f);
    REQUIRE(EXPECTED_PROJECTED_RIGHT_COORDINATE.X == Approx(projected_right_coordinate.X));
    REQUIRE(EXPECTED_PROJECTED_RIGHT_COORDINATE.Y == Approx(projected_right_coordinate.Y));
    REQUIRE(EXPECTED_PROJECTED_RIGHT_COORDINATE.Z == Approx(projected_right_coordinate.Z));
    REQUIRE(EXPECTED_PROJECTED_RIGHT_COORDINATE.W == Approx(projected_right_coordinate.W));
}
