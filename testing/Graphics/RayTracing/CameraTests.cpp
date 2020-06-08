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
    GRAPHICS::RenderTarget render_target(
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
    GRAPHICS::RenderTarget render_target(
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
    GRAPHICS::RenderTarget render_target(
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
    GRAPHICS::RenderTarget render_target(
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
    GRAPHICS::RenderTarget render_target(
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
    GRAPHICS::RenderTarget render_target(
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
    GRAPHICS::RenderTarget render_target(
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
    GRAPHICS::RenderTarget render_target(
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
    GRAPHICS::RenderTarget render_target(
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
