#pragma once

#include "Graphics/ProjectionType.h"
#include "Graphics/RayTracing/Ray.h"
#include "Graphics/RenderTarget.h"
#include "Graphics/ViewingPlane.h"
#include "Math/Angle.h"
#include "Math/CoordinateFrame.h"
#include "Math/Matrix4x4.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"

namespace GRAPHICS
{
    /// A camera defining the portion of a 3D scene that is currently viewable.
    class Camera
    {
    public:
        // STATIC PROJECTION MATRIX METHODS.
        static MATH::Matrix4x4f OrthographicProjection(
            const float left_x_world_boundary,
            const float right_x_world_boundary,
            const float bottom_y_world_boundary,
            const float top_y_world_boundary,
            const float near_z_world_boundary,
            const float far_z_world_boundary);
        static MATH::Matrix4x4f PerspectiveProjection(
            const MATH::Angle<float>::Degrees vertical_field_of_view_in_degrees,
            const float aspect_ratio_width_over_height,
            const float near_z_world_boundary,
            const float far_z_world_boundary);

        // CONSTRUCTION.
        static Camera LookAt(const MATH::Vector3f& look_at_world_position);  
        static Camera LookAtFrom(const MATH::Vector3f& look_at_world_position, const MATH::Vector3f& camera_world_position);

        // TRANSFORM METHODS.
        MATH::Matrix4x4f ViewTransform() const;

        // RAY TRACING METHODS.
        RAY_TRACING::Ray ViewingRay(
            const MATH::Vector2ui& pixel_coordinates,
            const RenderTarget& render_target) const;

        // PUBLIC MEMBER VARIABLES FOR EASY ACCESS.
        /// The type of projection the camera is currently using.
        ProjectionType Projection = ProjectionType::ORTHOGRAPHIC;
        /// The position of the camera in the world.
        MATH::Vector3f WorldPosition = MATH::Vector3f(0.0f, 0.0f, 1.0f);
        /// The coordinate frame for the camera's orientation.
        MATH::CoordinateFrame CoordinateFrame = MATH::CoordinateFrame();
        /// The field of view of the camera.
        MATH::Angle<float>::Degrees FieldOfView = MATH::Angle<float>::Degrees(45.0f);
        /// The viewing plane onto which the image is projected.
        GRAPHICS::ViewingPlane ViewingPlane = GRAPHICS::ViewingPlane();
    };
}
