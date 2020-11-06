#include "Graphics/Camera.h"
#include "ThirdParty/Catch/catch.hpp"

TEST_CASE("An orthographic viewing ray for the top-left corner can be correctly computed.", "[Camera][ViewingRay][Orthographic]")
{
    // CREATE A RENDER TARGET.
    // The render target's dimension are chosen for easy verification.
    // These dimensions result in 10 pixels on either side of the origin on each axis,
    // which makes understanding pixel centers easier.
    constexpr unsigned int RENDER_TARGET_WIDTH_IN_PIXELS = 20;
    constexpr unsigned int RENDER_TARGET_HEIGHT_IN_PIXELS = 20;
    GRAPHICS::Bitmap render_target(
        RENDER_TARGET_WIDTH_IN_PIXELS,
        RENDER_TARGET_HEIGHT_IN_PIXELS,
        GRAPHICS::ColorFormat::RGBA);

    // DEFINE AN ORTHOGRAPHIC CAMERA.
    GRAPHICS::Camera camera;
    camera.WorldPosition = MATH::Vector3f(0.0f, 0.0f, 1.0f);
    camera.Projection = GRAPHICS::ProjectionType::ORTHOGRAPHIC;

    // DEFINE PIXEL COORDINATES FOR THE TOP-LEFT CORNER.
    const MATH::Vector2ui PIXEL_COORDINATES(0, 0);

    // COMPUTE THE VIEWING RAY.
    GRAPHICS::RAY_TRACING::Ray actual_viewing_ray = camera.ViewingRay(PIXEL_COORDINATES, render_target);

    // VERIFY THE CORRECT VIEWING RAY WAS COMPUTED.
    // The ray should start at the top-left corner of the viewing plane in front of the camera.
    const MATH::Vector3f EXPECTED_RAY_ORIGIN(-0.95f, 0.95f, 0.0f);
    REQUIRE(EXPECTED_RAY_ORIGIN.X == actual_viewing_ray.Origin.X);
    REQUIRE(EXPECTED_RAY_ORIGIN.Y == actual_viewing_ray.Origin.Y);
    REQUIRE(EXPECTED_RAY_ORIGIN.Z == actual_viewing_ray.Origin.Z);

    // The ray should go in the direction of the camera's view.
    const MATH::Vector3f EXPECTED_RAY_DIRECTION(0.0, 0.0, -1.0f);
    REQUIRE(EXPECTED_RAY_DIRECTION.X == actual_viewing_ray.Direction.X);
    REQUIRE(EXPECTED_RAY_DIRECTION.Y == actual_viewing_ray.Direction.Y);
    REQUIRE(EXPECTED_RAY_DIRECTION.Z == actual_viewing_ray.Direction.Z);
}

TEST_CASE("An orthographic viewing ray for the top-right corner can be correctly computed.", "[Camera][ViewingRay][Orthographic]")
{
    // CREATE A RENDER TARGET.
    // The render target's dimension are chosen for easy verification.
    // These dimensions result in 10 pixels on either side of the origin on each axis,
    // which makes understanding pixel centers easier.
    constexpr unsigned int RENDER_TARGET_WIDTH_IN_PIXELS = 20;
    constexpr unsigned int RENDER_TARGET_HEIGHT_IN_PIXELS = 20;
    GRAPHICS::Bitmap render_target(
        RENDER_TARGET_WIDTH_IN_PIXELS,
        RENDER_TARGET_HEIGHT_IN_PIXELS,
        GRAPHICS::ColorFormat::RGBA);

    // DEFINE AN ORTHOGRAPHIC CAMERA.
    GRAPHICS::Camera camera;
    camera.WorldPosition = MATH::Vector3f(0.0f, 0.0f, 1.0f);
    camera.Projection = GRAPHICS::ProjectionType::ORTHOGRAPHIC;

    // DEFINE PIXEL COORDINATES FOR THE TOP-RIGHT CORNER.
    const MATH::Vector2ui PIXEL_COORDINATES(
        RENDER_TARGET_WIDTH_IN_PIXELS - 1,
        0);

    // COMPUTE THE VIEWING RAY.
    GRAPHICS::RAY_TRACING::Ray actual_viewing_ray = camera.ViewingRay(PIXEL_COORDINATES, render_target);

    // VERIFY THE CORRECT VIEWING RAY WAS COMPUTED.
    // The ray should start at the top-right corner of the viewing plane in front of the camera.
    const MATH::Vector3f EXPECTED_RAY_ORIGIN(0.95f, 0.95f, 0.0f);
    REQUIRE(EXPECTED_RAY_ORIGIN.X == actual_viewing_ray.Origin.X);
    REQUIRE(EXPECTED_RAY_ORIGIN.Y == actual_viewing_ray.Origin.Y);
    REQUIRE(EXPECTED_RAY_ORIGIN.Z == actual_viewing_ray.Origin.Z);

    // The ray should go in the direction of the camera's view.
    const MATH::Vector3f EXPECTED_RAY_DIRECTION(0.0, 0.0, -1.0f);
    REQUIRE(EXPECTED_RAY_DIRECTION.X == actual_viewing_ray.Direction.X);
    REQUIRE(EXPECTED_RAY_DIRECTION.Y == actual_viewing_ray.Direction.Y);
    REQUIRE(EXPECTED_RAY_DIRECTION.Z == actual_viewing_ray.Direction.Z);
}

TEST_CASE("An orthographic viewing ray for the bottom-left corner can be correctly computed.", "[Camera][ViewingRay][Orthographic]")
{
    // CREATE A RENDER TARGET.
    // The render target's dimension are chosen for easy verification.
    // These dimensions result in 10 pixels on either side of the origin on each axis,
    // which makes understanding pixel centers easier.
    constexpr unsigned int RENDER_TARGET_WIDTH_IN_PIXELS = 20;
    constexpr unsigned int RENDER_TARGET_HEIGHT_IN_PIXELS = 20;
    GRAPHICS::Bitmap render_target(
        RENDER_TARGET_WIDTH_IN_PIXELS,
        RENDER_TARGET_HEIGHT_IN_PIXELS,
        GRAPHICS::ColorFormat::RGBA);

    // DEFINE AN ORTHOGRAPHIC CAMERA.
    GRAPHICS::Camera camera;
    camera.WorldPosition = MATH::Vector3f(0.0f, 0.0f, 1.0f);
    camera.Projection = GRAPHICS::ProjectionType::ORTHOGRAPHIC;

    // DEFINE PIXEL COORDINATES FOR THE BOTTOM-LEFT CORNER.
    const MATH::Vector2ui PIXEL_COORDINATES(
        0,
        RENDER_TARGET_HEIGHT_IN_PIXELS - 1);

    // COMPUTE THE VIEWING RAY.
    GRAPHICS::RAY_TRACING::Ray actual_viewing_ray = camera.ViewingRay(PIXEL_COORDINATES, render_target);

    // VERIFY THE CORRECT VIEWING RAY WAS COMPUTED.
    // The ray should start at the bottom-left corner of the viewing plane in front of the camera.
    const MATH::Vector3f EXPECTED_RAY_ORIGIN(-0.95f, -0.95f, 0.0f);
    REQUIRE(EXPECTED_RAY_ORIGIN.X == actual_viewing_ray.Origin.X);
    REQUIRE(EXPECTED_RAY_ORIGIN.Y == actual_viewing_ray.Origin.Y);
    REQUIRE(EXPECTED_RAY_ORIGIN.Z == actual_viewing_ray.Origin.Z);

    // The ray should go in the direction of the camera's view.
    const MATH::Vector3f EXPECTED_RAY_DIRECTION(0.0, 0.0, -1.0f);
    REQUIRE(EXPECTED_RAY_DIRECTION.X == actual_viewing_ray.Direction.X);
    REQUIRE(EXPECTED_RAY_DIRECTION.Y == actual_viewing_ray.Direction.Y);
    REQUIRE(EXPECTED_RAY_DIRECTION.Z == actual_viewing_ray.Direction.Z);
}

TEST_CASE("An orthographic viewing ray for the bottom-right corner can be correctly computed.", "[Camera][ViewingRay][Orthographic]")
{
    // CREATE A RENDER TARGET.
    // The render target's dimension are chosen for easy verification.
    // These dimensions result in 10 pixels on either side of the origin on each axis,
    // which makes understanding pixel centers easier.
    constexpr unsigned int RENDER_TARGET_WIDTH_IN_PIXELS = 20;
    constexpr unsigned int RENDER_TARGET_HEIGHT_IN_PIXELS = 20;
    GRAPHICS::Bitmap render_target(
        RENDER_TARGET_WIDTH_IN_PIXELS,
        RENDER_TARGET_HEIGHT_IN_PIXELS,
        GRAPHICS::ColorFormat::RGBA);

    // DEFINE AN ORTHOGRAPHIC CAMERA.
    GRAPHICS::Camera camera;
    camera.WorldPosition = MATH::Vector3f(0.0f, 0.0f, 1.0f);
    camera.Projection = GRAPHICS::ProjectionType::ORTHOGRAPHIC;

    // DEFINE PIXEL COORDINATES FOR THE BOTTOM-RIGHT CORNER.
    const MATH::Vector2ui PIXEL_COORDINATES(
        RENDER_TARGET_WIDTH_IN_PIXELS - 1,
        RENDER_TARGET_HEIGHT_IN_PIXELS - 1);

    // COMPUTE THE VIEWING RAY.
    GRAPHICS::RAY_TRACING::Ray actual_viewing_ray = camera.ViewingRay(PIXEL_COORDINATES, render_target);

    // VERIFY THE CORRECT VIEWING RAY WAS COMPUTED.
    // The ray should start at the bottom-right corner of the viewing plane in front of the camera.
    const MATH::Vector3f EXPECTED_RAY_ORIGIN(0.95f, -0.95f, 0.0f);
    REQUIRE(EXPECTED_RAY_ORIGIN.X == actual_viewing_ray.Origin.X);
    REQUIRE(EXPECTED_RAY_ORIGIN.Y == actual_viewing_ray.Origin.Y);
    REQUIRE(EXPECTED_RAY_ORIGIN.Z == actual_viewing_ray.Origin.Z);

    // The ray should go in the direction of the camera's view.
    const MATH::Vector3f EXPECTED_RAY_DIRECTION(0.0, 0.0, -1.0f);
    REQUIRE(EXPECTED_RAY_DIRECTION.X == actual_viewing_ray.Direction.X);
    REQUIRE(EXPECTED_RAY_DIRECTION.Y == actual_viewing_ray.Direction.Y);
    REQUIRE(EXPECTED_RAY_DIRECTION.Z == actual_viewing_ray.Direction.Z);
}

TEST_CASE("An orthographic viewing ray for the center can be correctly computed.", "[Camera][ViewingRay][Orthographic]")
{
    // CREATE A RENDER TARGET.
    // The render target's dimension are chosen for easy verification.
    // These dimensions result in 10 pixels on either side of the origin on each axis,
    // which makes understanding pixel centers easier.
    constexpr unsigned int RENDER_TARGET_WIDTH_IN_PIXELS = 20;
    constexpr unsigned int RENDER_TARGET_HEIGHT_IN_PIXELS = 20;
    GRAPHICS::Bitmap render_target(
        RENDER_TARGET_WIDTH_IN_PIXELS,
        RENDER_TARGET_HEIGHT_IN_PIXELS,
        GRAPHICS::ColorFormat::RGBA);

    // DEFINE AN ORTHOGRAPHIC CAMERA.
    GRAPHICS::Camera camera;
    camera.WorldPosition = MATH::Vector3f(0.0f, 0.0f, 1.0f);
    camera.Projection = GRAPHICS::ProjectionType::ORTHOGRAPHIC;

    // DEFINE PIXEL COORDINATES FOR THE CENTER.
    const MATH::Vector2ui PIXEL_COORDINATES(
        RENDER_TARGET_WIDTH_IN_PIXELS / 2,
        RENDER_TARGET_HEIGHT_IN_PIXELS / 2);

    // COMPUTE THE VIEWING RAY.
    GRAPHICS::RAY_TRACING::Ray actual_viewing_ray = camera.ViewingRay(PIXEL_COORDINATES, render_target);

    // VERIFY THE CORRECT VIEWING RAY WAS COMPUTED.
    // The ray should start at the center of the viewing plane in front of the camera.
    // Since the pixel centers are used, the origin is slightly offset.
    const MATH::Vector3f EXPECTED_RAY_ORIGIN(0.05f, -0.05f, 0.0f);
    REQUIRE(EXPECTED_RAY_ORIGIN.X == actual_viewing_ray.Origin.X);
    REQUIRE(EXPECTED_RAY_ORIGIN.Y == actual_viewing_ray.Origin.Y);
    REQUIRE(EXPECTED_RAY_ORIGIN.Z == actual_viewing_ray.Origin.Z);

    // The ray should go in the direction of the camera's view.
    const MATH::Vector3f EXPECTED_RAY_DIRECTION(0.0, 0.0, -1.0f);
    REQUIRE(EXPECTED_RAY_DIRECTION.X == actual_viewing_ray.Direction.X);
    REQUIRE(EXPECTED_RAY_DIRECTION.Y == actual_viewing_ray.Direction.Y);
    REQUIRE(EXPECTED_RAY_DIRECTION.Z == actual_viewing_ray.Direction.Z);
}

TEST_CASE("An orthographic viewing ray for the top-right quadrant can be correctly computed.", "[Camera][ViewingRay][Orthographic]")
{
    // CREATE A RENDER TARGET.
    // The render target's dimension are chosen for easy verification.
    // These dimensions result in 10 pixels on either side of the origin on each axis,
    // which makes understanding pixel centers easier.
    constexpr unsigned int RENDER_TARGET_WIDTH_IN_PIXELS = 20;
    constexpr unsigned int RENDER_TARGET_HEIGHT_IN_PIXELS = 20;
    GRAPHICS::Bitmap render_target(
        RENDER_TARGET_WIDTH_IN_PIXELS,
        RENDER_TARGET_HEIGHT_IN_PIXELS,
        GRAPHICS::ColorFormat::RGBA);

    // DEFINE AN ORTHOGRAPHIC CAMERA.
    GRAPHICS::Camera camera;
    camera.WorldPosition = MATH::Vector3f(0.0f, 0.0f, 1.0f);
    camera.Projection = GRAPHICS::ProjectionType::ORTHOGRAPHIC;

    // DEFINE PIXEL COORDINATES FOR THE TOP-RIGHT QUADRANT.
    const MATH::Vector2ui PIXEL_COORDINATES(12, 4);

    // COMPUTE THE VIEWING RAY.
    GRAPHICS::RAY_TRACING::Ray actual_viewing_ray = camera.ViewingRay(PIXEL_COORDINATES, render_target);

    // VERIFY THE CORRECT VIEWING RAY WAS COMPUTED.
    // The ray should start in the top-right quadrant of the viewing plane in front of the camera.
    const MATH::Vector3f EXPECTED_RAY_ORIGIN(0.25f, 0.55f, 0.0f);
    REQUIRE(EXPECTED_RAY_ORIGIN.X == actual_viewing_ray.Origin.X);
    REQUIRE(EXPECTED_RAY_ORIGIN.Y == actual_viewing_ray.Origin.Y);
    REQUIRE(EXPECTED_RAY_ORIGIN.Z == actual_viewing_ray.Origin.Z);

    // The ray should go in the direction of the camera's view.
    const MATH::Vector3f EXPECTED_RAY_DIRECTION(0.0, 0.0, -1.0f);
    REQUIRE(EXPECTED_RAY_DIRECTION.X == actual_viewing_ray.Direction.X);
    REQUIRE(EXPECTED_RAY_DIRECTION.Y == actual_viewing_ray.Direction.Y);
    REQUIRE(EXPECTED_RAY_DIRECTION.Z == actual_viewing_ray.Direction.Z);
}

TEST_CASE("An orthographic viewing ray for the top-left quadrant can be correctly computed.", "[Camera][ViewingRay][Orthographic]")
{
    // CREATE A RENDER TARGET.
    // The render target's dimension are chosen for easy verification.
    // These dimensions result in 10 pixels on either side of the origin on each axis,
    // which makes understanding pixel centers easier.
    constexpr unsigned int RENDER_TARGET_WIDTH_IN_PIXELS = 20;
    constexpr unsigned int RENDER_TARGET_HEIGHT_IN_PIXELS = 20;
    GRAPHICS::Bitmap render_target(
        RENDER_TARGET_WIDTH_IN_PIXELS,
        RENDER_TARGET_HEIGHT_IN_PIXELS,
        GRAPHICS::ColorFormat::RGBA);

    // DEFINE AN ORTHOGRAPHIC CAMERA.
    GRAPHICS::Camera camera;
    camera.WorldPosition = MATH::Vector3f(0.0f, 0.0f, 1.0f);
    camera.Projection = GRAPHICS::ProjectionType::ORTHOGRAPHIC;

    // DEFINE PIXEL COORDINATES FOR THE TOP-LEFT QUADRANT.
    const MATH::Vector2ui PIXEL_COORDINATES(3, 6);

    // COMPUTE THE VIEWING RAY.
    GRAPHICS::RAY_TRACING::Ray actual_viewing_ray = camera.ViewingRay(PIXEL_COORDINATES, render_target);

    // VERIFY THE CORRECT VIEWING RAY WAS COMPUTED.
    // The ray should start in the top-left quadrant of the viewing plane in front of the camera.
    const MATH::Vector3f EXPECTED_RAY_ORIGIN(-0.65f, 0.35f, 0.0f);
    REQUIRE(EXPECTED_RAY_ORIGIN.X == actual_viewing_ray.Origin.X);
    REQUIRE(EXPECTED_RAY_ORIGIN.Y == actual_viewing_ray.Origin.Y);
    REQUIRE(EXPECTED_RAY_ORIGIN.Z == actual_viewing_ray.Origin.Z);

    // The ray should go in the direction of the camera's view.
    const MATH::Vector3f EXPECTED_RAY_DIRECTION(0.0, 0.0, -1.0f);
    REQUIRE(EXPECTED_RAY_DIRECTION.X == actual_viewing_ray.Direction.X);
    REQUIRE(EXPECTED_RAY_DIRECTION.Y == actual_viewing_ray.Direction.Y);
    REQUIRE(EXPECTED_RAY_DIRECTION.Z == actual_viewing_ray.Direction.Z);
}

TEST_CASE("An orthographic viewing ray for the bottom-left quadrant can be correctly computed.", "[Camera][ViewingRay][Orthographic]")
{
    // CREATE A RENDER TARGET.
    // The render target's dimension are chosen for easy verification.
    // These dimensions result in 10 pixels on either side of the origin on each axis,
    // which makes understanding pixel centers easier.
    constexpr unsigned int RENDER_TARGET_WIDTH_IN_PIXELS = 20;
    constexpr unsigned int RENDER_TARGET_HEIGHT_IN_PIXELS = 20;
    GRAPHICS::Bitmap render_target(
        RENDER_TARGET_WIDTH_IN_PIXELS,
        RENDER_TARGET_HEIGHT_IN_PIXELS,
        GRAPHICS::ColorFormat::RGBA);

    // DEFINE AN ORTHOGRAPHIC CAMERA.
    GRAPHICS::Camera camera;
    camera.WorldPosition = MATH::Vector3f(0.0f, 0.0f, 1.0f);
    camera.Projection = GRAPHICS::ProjectionType::ORTHOGRAPHIC;

    // DEFINE PIXEL COORDINATES FOR THE BOTTOM-LEFT QUADRANT.
    const MATH::Vector2ui PIXEL_COORDINATES(2, 13);

    // COMPUTE THE VIEWING RAY.
    GRAPHICS::RAY_TRACING::Ray actual_viewing_ray = camera.ViewingRay(PIXEL_COORDINATES, render_target);

    // VERIFY THE CORRECT VIEWING RAY WAS COMPUTED.
    // The ray should start in the bottom-left quadrant of the viewing plane in front of the camera.
    const MATH::Vector3f EXPECTED_RAY_ORIGIN(-0.75f, -0.35f, 0.0f);
    REQUIRE(EXPECTED_RAY_ORIGIN.X == actual_viewing_ray.Origin.X);
    REQUIRE(EXPECTED_RAY_ORIGIN.Y == actual_viewing_ray.Origin.Y);
    REQUIRE(EXPECTED_RAY_ORIGIN.Z == actual_viewing_ray.Origin.Z);

    // The ray should go in the direction of the camera's view.
    const MATH::Vector3f EXPECTED_RAY_DIRECTION(0.0, 0.0, -1.0f);
    REQUIRE(EXPECTED_RAY_DIRECTION.X == actual_viewing_ray.Direction.X);
    REQUIRE(EXPECTED_RAY_DIRECTION.Y == actual_viewing_ray.Direction.Y);
    REQUIRE(EXPECTED_RAY_DIRECTION.Z == actual_viewing_ray.Direction.Z);
}

TEST_CASE("An orthographic viewing ray for the bottom-right quadrant can be correctly computed.", "[Camera][ViewingRay][Orthographic]")
{
    // CREATE A RENDER TARGET.
    // The render target's dimension are chosen for easy verification.
    // These dimensions result in 10 pixels on either side of the origin on each axis,
    // which makes understanding pixel centers easier.
    constexpr unsigned int RENDER_TARGET_WIDTH_IN_PIXELS = 20;
    constexpr unsigned int RENDER_TARGET_HEIGHT_IN_PIXELS = 20;
    GRAPHICS::Bitmap render_target(
        RENDER_TARGET_WIDTH_IN_PIXELS,
        RENDER_TARGET_HEIGHT_IN_PIXELS,
        GRAPHICS::ColorFormat::RGBA);

    // DEFINE AN ORTHOGRAPHIC CAMERA.
    GRAPHICS::Camera camera;
    camera.WorldPosition = MATH::Vector3f(0.0f, 0.0f, 1.0f);
    camera.Projection = GRAPHICS::ProjectionType::ORTHOGRAPHIC;

    // DEFINE PIXEL COORDINATES FOR THE BOTTOM-RIGHT QUADRANT.
    const MATH::Vector2ui PIXEL_COORDINATES(17, 18);

    // COMPUTE THE VIEWING RAY.
    GRAPHICS::RAY_TRACING::Ray actual_viewing_ray = camera.ViewingRay(PIXEL_COORDINATES, render_target);

    // VERIFY THE CORRECT VIEWING RAY WAS COMPUTED.
    // The ray should start in the bottom-right quadrant of the viewing plane in front of the camera.
    const MATH::Vector3f EXPECTED_RAY_ORIGIN(0.75f, -0.85f, 0.0f);
    REQUIRE(EXPECTED_RAY_ORIGIN.X == actual_viewing_ray.Origin.X);
    REQUIRE(EXPECTED_RAY_ORIGIN.Y == actual_viewing_ray.Origin.Y);
    REQUIRE(EXPECTED_RAY_ORIGIN.Z == actual_viewing_ray.Origin.Z);

    // The ray should go in the direction of the camera's view.
    const MATH::Vector3f EXPECTED_RAY_DIRECTION(0.0, 0.0, -1.0f);
    REQUIRE(EXPECTED_RAY_DIRECTION.X == actual_viewing_ray.Direction.X);
    REQUIRE(EXPECTED_RAY_DIRECTION.Y == actual_viewing_ray.Direction.Y);
    REQUIRE(EXPECTED_RAY_DIRECTION.Z == actual_viewing_ray.Direction.Z);
}

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
