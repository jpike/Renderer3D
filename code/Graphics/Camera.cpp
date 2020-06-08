#include <cmath>
#include "Graphics/Camera.h"

namespace GRAPHICS
{
    /// Creates an orthographic projection matrix.
    /// @param[in]  left_x_world_boundary - The left (x) world boundary of the
    ///     orthographic view volume.
    /// @param[in]  right_x_world_boundary - The right (x) world boundary of the
    ///     orthographic view volume.
    /// @param[in]  bottom_y_world_boundary - The bottom (y) world boundary of the
    ///     orthographic view volume.
    /// @param[in]  top_y_world_boundary - The top (y) world boundary of the
    ///     orthographic view volume.
    /// @param[in]  near_z_world_boundary - The near (z) world boundary of the
    ///     orthographic view volume.
    /// @param[in]  far_z_world_boundary - The far (z) world boundary of the
    ///     orthographic view volume.
    /// @return The specified orthographic projection matrix.
    MATH::Matrix4x4f Camera::OrthographicProjection(
        const float left_x_world_boundary,
        const float right_x_world_boundary,
        const float bottom_y_world_boundary,
        const float top_y_world_boundary,
        const float near_z_world_boundary,
        const float far_z_world_boundary)
    {
        // CREATE A MATRIX TO TRANSLATE THE ORTHOGRAPHIC VIEW VOLUME TO THE ORIGIN.
        MATH::Vector3f translation_vector;

        float orthographic_view_volume_x_midpoint = (left_x_world_boundary + right_x_world_boundary) / 2.0f;
        translation_vector.X = -orthographic_view_volume_x_midpoint;

        float orthographic_view_volume_y_midpoint = (bottom_y_world_boundary + top_y_world_boundary) / 2.0f;
        translation_vector.Y = -orthographic_view_volume_y_midpoint;

        float orthographic_view_volume_z_midpoint = (near_z_world_boundary + far_z_world_boundary) / 2.0f;
        translation_vector.Z = -orthographic_view_volume_z_midpoint;

        MATH::Matrix4x4f translate_view_volume_matrix = MATH::Matrix4x4f::Translation(translation_vector);

        // CREATE A MATRIX TO SCALE THE ORTHOGRAPHICS VIEW VOLUME TO THE CANONICAL VIEW VOLUME.
        const float CANONICAL_VIEW_VOLUME_DIMENSION = 2.0f;
        MATH::Vector3f scale_vector;

        float orthographic_view_volume_width = right_x_world_boundary - left_x_world_boundary;
        scale_vector.X = CANONICAL_VIEW_VOLUME_DIMENSION / orthographic_view_volume_width;

        float orthographic_view_volume_height = top_y_world_boundary - bottom_y_world_boundary;
        scale_vector.Y = CANONICAL_VIEW_VOLUME_DIMENSION / orthographic_view_volume_height;

        float orthographic_view_volume_depth = near_z_world_boundary - far_z_world_boundary;
        scale_vector.Z = CANONICAL_VIEW_VOLUME_DIMENSION / orthographic_view_volume_depth;

        MATH::Matrix4x4f scale_view_volume_matrix = MATH::Matrix4x4f::Scale(scale_vector);

        // CREATE THE ORTHOGRAPHIC PROJECTION MATRIX.
        MATH::Matrix4x4f orthographic_projection_matrix = scale_view_volume_matrix * translate_view_volume_matrix;
        return orthographic_projection_matrix;
    }

    /// Creates a perspective projection matrix.
    /// @param[in]  vertical_field_of_view_in_degrees - The vertical field of view, in degrees.
    /// @param[in]  aspect_ratio_width_over_height - The aspect ratio (width over height) of the view.
    /// @param[in]  near_z_world_boundary - The near (z) world boundary of the
    ///     view frustum.
    /// @param[in]  far_z_world_boundary - The far (z) world boundary of the
    ///     view frustum.
    /// @return The specified perspective projection matrix.
    MATH::Matrix4x4f Camera::PerspectiveProjection(
        const MATH::Angle<float>::Degrees vertical_field_of_view_in_degrees,
        const float aspect_ratio_width_over_height,
        const float near_z_world_boundary,
        const float far_z_world_boundary)
    {
        // CREATE THE BASIC PERSPECTIVE MATRIX.
        MATH::Matrix4x4f perspective_matrix;
        // Multiples the x/y coordinates by the near z world boundary so that the x/y coordinates
        // can be properly scaled relative to the near plain and the corresponding z coordinate.
        perspective_matrix.Elements(0, 0) = near_z_world_boundary;
        perspective_matrix.Elements(1, 1) = near_z_world_boundary;
        // Ensures that points on the near and far z planes are left alone in terms of the z coordinate.
        perspective_matrix.Elements(2, 2) = near_z_world_boundary + far_z_world_boundary;
        perspective_matrix.Elements(3, 2) = -far_z_world_boundary * near_z_world_boundary;
        // Helps preserve the z coordinate.
        perspective_matrix.Elements(2, 3) = 1.0f;

        // CREATE THE ORTHOGRAPHIC MATRIX.
        // The tangent function requires the field of view in radians.
        MATH::Angle<float>::Radians vertical_field_of_view_in_radians = MATH::Angle<float>::DegreesToRadians(vertical_field_of_view_in_degrees);

        // Half of the field of view defines how "high" the view frustum's near plane should be.
        float half_vertical_field_of_view_in_radians = vertical_field_of_view_in_radians.Value / 2.0f;
        float half_vertical_field_of_view_tangent = tan(half_vertical_field_of_view_in_radians);

        // An orthographic projection can handle projecting to the near plane of the frustum.
        float near_z_distance = abs(near_z_world_boundary);
        float top_y_world_boundary = half_vertical_field_of_view_tangent * near_z_distance;
        float bottom_y_world_boundary = -top_y_world_boundary;
        float right_x_world_boundary = aspect_ratio_width_over_height * top_y_world_boundary;
        float left_x_world_boundary = -right_x_world_boundary;

        MATH::Matrix4x4f orthographic_matrix = OrthographicProjection(
            left_x_world_boundary,
            right_x_world_boundary,
            bottom_y_world_boundary,
            top_y_world_boundary,
            near_z_world_boundary,
            far_z_world_boundary);

        // COMPUTE THE FULL PERSPECTIVE PROJECTION MATRIX.
        MATH::Matrix4x4f perspective_projection_matrix = orthographic_matrix * perspective_matrix;

        return perspective_projection_matrix;
    }

    /// Creates a camera looking at the specified world position.
    /// It is assumed that this look at position doesn't match the camera's position.
    /// If that isn't true, this may not work.
    /// @param[in]  look_at_world_position - The world position the camera should look at.
    /// @return A camera looking at the specified position.
    Camera Camera::LookAt(const MATH::Vector3f& look_at_world_position)
    {
        // CREATE A CAMERA LOOKING FROM ITS DEFAULT WORLD POSITION.
        Camera camera;
        camera = Camera::LookAtFrom(look_at_world_position, camera.WorldPosition);
        return camera;
    }

    /// Creates a camera looking at the specified world position from another specified world position.
    /// It is assumed that this look at position doesn't match the camera's position.
    /// If that isn't true, this may not work.
    /// @param[in]  camera_world_position - The world position the camera should be placed at.
    /// @param[in]  look_at_world_position - The world position the camera should look at.
    /// @return A camera looking at the specified position from another specified position.
    Camera Camera::LookAtFrom(const MATH::Vector3f& look_at_world_position, const MATH::Vector3f& camera_world_position)
    {
        // CREATE A CAMERA AT THE SPECIFIED WORLD POSITION.
        Camera camera;
        camera.WorldPosition = camera_world_position;

        // COMPUTE THE VIEWING DIRECTION FOR THE CAMERA.
        MATH::Vector3f camera_view_direction = look_at_world_position - camera.WorldPosition;
        MATH::Vector3f normalized_camera_view_direction = MATH::Vector3f::Normalize(camera_view_direction);

        // COMPUTE THE COORDINATE FRAME FOR THE CAMERA.
        MATH::Vector3f negative_camera_view_direction = MATH::Vector3f::Scale(-1.0f, normalized_camera_view_direction);
        camera.CoordinateFrame = MATH::CoordinateFrame::FromUpAndForward(camera.CoordinateFrame.Up, negative_camera_view_direction);

        return camera;
    }

    /// Computes the view transformation of the camera to transform
    /// world space coordinates to camera space coordinates.
    /// @return The view transformation matrix for the camera.
    MATH::Matrix4x4f Camera::ViewTransform() const
    {
        // CALCULATE THE ORTHONORMAL BASIS FOR THE CAMERA'S COORDINATE SYSTEM.
        MATH::Vector3f view_direction = -CoordinateFrame.Forward;
        MATH::Vector3f negative_view_direction_basis_vector = -MATH::Vector3f::Normalize(view_direction);

        MATH::Vector3f unnormalized_sideways_vector = MATH::Vector3f::CrossProduct(CoordinateFrame.Up, negative_view_direction_basis_vector);
        MATH::Vector3f sideways_basis_vector = MATH::Vector3f::Normalize(unnormalized_sideways_vector);

        MATH::Vector3f near_up_direction_basis_vector = MATH::Vector3f::CrossProduct(negative_view_direction_basis_vector, sideways_basis_vector);

        // CREATE A MATRIX FOR TRANSLATING TO CAMERA TO THE WORLD ORIGIN.
        MATH::Matrix4x4f translate_camera_to_origin_matrix = MATH::Matrix4x4f::Translation(-WorldPosition);

        // CREATE A MATRIX FOR ALIGNING THE THE CAMERA'S COORDINATE SYSTEM WITH THE WORLD COORDINATE SYSTEM.
        MATH::Matrix4x4f align_camera_to_world_matrix = MATH::Matrix4x4f::Identity();
        align_camera_to_world_matrix.SetRow(0, sideways_basis_vector);
        align_camera_to_world_matrix.SetRow(1, near_up_direction_basis_vector);
        align_camera_to_world_matrix.SetRow(2, negative_view_direction_basis_vector);

        // FORM THE FINAL VIEW TRANSFORM MATRIX.
        MATH::Matrix4x4f view_transform = align_camera_to_world_matrix * translate_camera_to_origin_matrix;
        return view_transform;
    }

    /// Computes a viewing ray coming from this camera for the specified pixel coordinates.
    /// @param[in]  pixel_coordinates - The coordinates of the render target
    ///     for the pixel through which to compute the viewing ray. 
    /// @param[in]  render_target - The render target for which the viewing ray is to
    ///     be computed.
    /// @return The viewing ray from the camera through the specified coordinates;
    ///     the exact ray will vary depending on the type of projection this camera
    ///     is using.
    RAY_TRACING::Ray Camera::ViewingRay(
        const MATH::Vector2ui& pixel_coordinates,
        const RenderTarget& render_target) const
    {
        // CONVERT THE PIXEL COORDINATES TO THE RANGE OF THE VIEWING PLANE.
        // In order to convert the current pixel coordinate to proper coordinates for the viewing ray,
        // several transformations are needed to convert from a [0, pixel dimension] range to
        // a range for the viewing plane:
        // 1. Each pixel may be thought of as a box.  For most consistent rendering,
        //      the ray should go through the center of each pixel.
        constexpr float OFFSET_TO_CENTER_OF_PIXEL = 0.5f;
        float x_pixel_center = pixel_coordinates.X + OFFSET_TO_CENTER_OF_PIXEL;
        // 2. Shift the coordinates down so that the minimum coordinates are negative.
        //      By doing this by the half-width of the render target, this means the
        //      new center will correspond with the center of the render target.
        unsigned int render_target_width_in_pixels = render_target.GetWidthInPixels();
        float render_target_half_width_in_pixels = render_target_width_in_pixels / 2.0f;
        float x_shifted_down = (x_pixel_center - render_target_half_width_in_pixels);
        // 3. Scale the coordinates to be in the range of the viewing plane
        //      instead of the pixel range of the render target.
        float x_scaled_to_viewing_plane_range = x_shifted_down * ViewingPlane.Width / render_target_width_in_pixels;

        // The same conversion must happen for y, but since the render target pixel coordinates
        // have y increasing going down but the viewing plane has y increasing going up,
        // the y coordinate must be flipped.
        unsigned int render_target_height_in_pixels = render_target.GetHeightInPixels();
        float render_target_half_height_in_pixels = render_target_height_in_pixels / 2.0f;
        float y_pixel_center = pixel_coordinates.Y + OFFSET_TO_CENTER_OF_PIXEL;
        float y_shifted_down = (y_pixel_center - render_target_half_height_in_pixels);
        float y_scaled_to_viewing_plane_range = y_shifted_down * ViewingPlane.Height / render_target_height_in_pixels;
        constexpr float FLIP_Y = -1.0f;
        y_scaled_to_viewing_plane_range *= FLIP_Y;

        // COMPUTE A VIEWING RAY ACCORDING TO THE TYPE OF PROJECTION.
        bool using_perspective_projection = (ProjectionType::PERSPECTIVE == Projection);
        if (using_perspective_projection)
        {
            // ADDING PERSPECTIVE SCALING.
            // The coordinates should be scaled based on the ratio of the camera's field of view and the
            // distance of the camera from the viewing plane (the tangent represents the ratio
            // between the horizontal (x) and vertical (y) dimensions with the depth (z) dimension).
            // This helps provide perspective scaling based on distance.
            MATH::Angle<float>::Radians camera_field_of_view_in_radians = MATH::Angle<float>::DegreesToRadians(FieldOfView);
            float half_field_of_view_in_radians = camera_field_of_view_in_radians.Value / 2.0f;
            float ratio_between_camera_view_dimensions_and_distance_from_viewing_plane = std::tan(half_field_of_view_in_radians);
            float x_scaled_based_on_camera_distance = x_scaled_to_viewing_plane_range * ratio_between_camera_view_dimensions_and_distance_from_viewing_plane;
            float y_scaled_based_on_camera_distance = y_scaled_to_viewing_plane_range * ratio_between_camera_view_dimensions_and_distance_from_viewing_plane;

            // RETURN A RAY GOING THROUGH THE SPECIFIED PIXEL.
            // Since we're using a right-handed coordinate system and the camera's focal length needs to be negated.
            MATH::Vector3f view_direction = MATH::Vector3f::Scale(-ViewingPlane.FocalLength, CoordinateFrame.Forward);
            // The view direction needs to be offset based on the perspective scaling.
            MATH::Vector3f horizontal_direction = MATH::Vector3f::Scale(x_scaled_based_on_camera_distance, CoordinateFrame.Right);
            view_direction += horizontal_direction;
            MATH::Vector3f vertical_direction = MATH::Vector3f::Scale(y_scaled_based_on_camera_distance, CoordinateFrame.Up);
            view_direction += vertical_direction;
            MATH::Vector3f normalized_view_direction = MATH::Vector3f::Normalize(view_direction);
            RAY_TRACING::Ray perspective_ray(WorldPosition, normalized_view_direction);
            return perspective_ray;
        }
        else
        {
            // COMPUTE THE RAY ORIGIN.
            // An orthographic projection is assumed by default.
            // The ray should start on the viewing plane in the appropriate direction in front of the camera.
            MATH::Vector3f ray_origin = WorldPosition;
            MATH::Vector3f normalized_camera_view_direction = MATH::Vector3f::Scale(-1.0f, CoordinateFrame.Forward);
            MATH::Vector3f displacement_from_camera_position_to_viewing_plane = MATH::Vector3f::Scale(
                ViewingPlane.FocalLength,
                normalized_camera_view_direction);
            ray_origin += displacement_from_camera_position_to_viewing_plane;

            // The ray needs to be horizontally displaced so that it shoots straight out from the specified pixel.
            MATH::Vector3f horizontal_pixel_displacement = MATH::Vector3f::Scale(x_scaled_to_viewing_plane_range, CoordinateFrame.Right);
            ray_origin += horizontal_pixel_displacement;

            // The ray needs to be vertically displaced so that it shoots straight out from the specified pixel.
            MATH::Vector3f vertical_pixel_displacement = MATH::Vector3f::Scale(y_scaled_to_viewing_plane_range, CoordinateFrame.Up);
            ray_origin += vertical_pixel_displacement;

            // RETURN A RAY GOING THROUGH THE SPECIFIED PIXEL.
            // It should go from the specified pixel on the viewing plane directly in the direction the camera is facing.
            MATH::Vector3f ray_direction = normalized_camera_view_direction;
            RAY_TRACING::Ray orthographic_ray(ray_origin, ray_direction);
            return orthographic_ray;
        }
    }
}
