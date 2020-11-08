#include <cassert>
#include "Graphics/SoftwareRasterizationAlgorithm.h"
#include "Math/Number.h"

namespace GRAPHICS
{
    /// Renders some text onto the render target.
    /// @param[in]  text - The text to render.
    /// @param[in,out]  render_target - The target to render to.
    void SoftwareRasterizationAlgorithm::Render(const GUI::Text& text, Bitmap& render_target)
    {
        // MAKE SURE A FONT EXISTS.
        if (!text.Font)
        {
            return;
        }

        // RENDER A GLYPH FOR EACH CHARACTER.
        unsigned int current_glyph_left_x_position = static_cast<unsigned int>(text.LeftTopPosition.X);
        unsigned int current_glyph_top_y_position = static_cast<unsigned int>(text.LeftTopPosition.Y);
        for (char character : text.String)
        {
            // RENDER ALL PIXELS FOR THE CURRENT GLYPH.
            const GUI::Glyph& glyph = text.Font->GlyphsByCharacter[static_cast<unsigned char>(character)];
            for (unsigned int glyph_local_pixel_y_position = 0; glyph_local_pixel_y_position < glyph.HeightInPixels; ++glyph_local_pixel_y_position)
            {
                for (unsigned int glyph_local_pixel_x_position = 0; glyph_local_pixel_x_position < glyph.WidthInPixels; ++glyph_local_pixel_x_position)
                {
                    // ONLY WRITE THE PIXEL IF IT IS VISIBLE.
                    /// @todo   Fancier alpha blending?
                    GRAPHICS::Color pixel_color = glyph.GetPixelColor(glyph_local_pixel_x_position, glyph_local_pixel_y_position);
                    bool pixel_visible = (pixel_color.Alpha > 0);
                    if (pixel_visible)
                    {
                        unsigned int glyph_destination_x_position = current_glyph_left_x_position + glyph_local_pixel_x_position;
                        unsigned int glyph_destination_y_position = current_glyph_top_y_position + glyph_local_pixel_y_position;
                        render_target.WritePixel(glyph_destination_x_position, glyph_destination_y_position, pixel_color);
                    }
                }
            }

            // MOVE TO THE NEXT GLYPH.
            current_glyph_left_x_position += glyph.WidthInPixels;
        }
    }

    /// Renders an entire 3D scene.
    /// @param[in]  scene - The scene to render.
    /// @param[in]  camera - The camera to use to view the scene.
    /// @param[in]  cull_backfaces - True if backfaces should be culled; false otherwise.
    /// @param[in,out]  output_bitmap - The bitmap to render to.
    void SoftwareRasterizationAlgorithm::Render(const Scene& scene, const Camera& camera, const bool cull_backfaces, Bitmap& output_bitmap)
    {
        // CLEAR THE BACKGROUND.
        output_bitmap.FillPixels(scene.BackgroundColor);

        // RENDER EACH OBJECT IN THE SCENE.
        for (const auto& object_3D : scene.Objects)
        {
            Render(object_3D, scene.PointLights, camera, cull_backfaces, output_bitmap);
        }
    }

    /// Renders a 3D object to the render target.
    /// @param[in]  object_3D - The object to render.
    /// @param[in]  lights - Any lights that should illuminate the object.
    /// @param[in]  camera - The camera to use to view the object.
    /// @param[in,out]  output_bitmap - The bitmap to render to.
    void SoftwareRasterizationAlgorithm::Render(const Object3D& object_3D, const std::vector<Light>& lights, const Camera& camera, const bool cull_backfaces, Bitmap& output_bitmap)
    {
        // GET RE-USED TRANSFORMATION MATRICES.
        // This is done before the loop to avoid performance hits for repeatedly calculating these matrices.
        MATH::Matrix4x4f object_world_transform = object_3D.WorldTransform();
        MATH::Matrix4x4f camera_view_transform = camera.ViewTransform();

        MATH::Matrix4x4f camera_projection_transform = camera.ProjectionTransform();

        MATH::Matrix4x4f flip_y_transform = MATH::Matrix4x4f::Scale(MATH::Vector3f(1.0f, -1.0f, 1.0f));
        MATH::Matrix4x4f scale_to_screen_transform = MATH::Matrix4x4f::Scale(MATH::Vector3f(
            static_cast<float>(output_bitmap.GetWidthInPixels()) / 2.0f,
            static_cast<float>(output_bitmap.GetHeightInPixels()) / 2.0f,
            1.0f));
        MATH::Matrix4x4f translate_to_screen_center_transform = MATH::Matrix4x4f::Translation(MATH::Vector3f(
            static_cast<float>(output_bitmap.GetWidthInPixels()) / 2.0f,
            static_cast<float>(output_bitmap.GetHeightInPixels()) / 2.0f,
            0.0f));
        MATH::Matrix4x4 screen_transform = translate_to_screen_center_transform * scale_to_screen_transform * flip_y_transform;

        // RENDER EACH TRIANGLE OF THE OBJECT.
        for (const auto& local_triangle : object_3D.Triangles)
        {
            // TRANSFORM THE TRIANGLE INTO WORLD SPACE.
            Triangle world_space_triangle = local_triangle;
            std::size_t triangle_vertex_count = world_space_triangle.Vertices.size();
            for (std::size_t vertex_index = 0; vertex_index < triangle_vertex_count; ++vertex_index)
            {
                // TRANFORM THE CURRENT LOCAL VERTEX INTO WORLD SPACE.
                const MATH::Vector3f& local_vertex = local_triangle.Vertices[vertex_index];
                MATH::Vector4f local_homogeneous_vertex = MATH::Vector4f::HomogeneousPositionVector(local_vertex);

                /// @todo   More unit testing!
                MATH::Vector4f world_homogeneous_vertex = object_world_transform * local_homogeneous_vertex;
                MATH::Vector3f& world_vertex = world_space_triangle.Vertices[vertex_index];
                world_vertex = MATH::Vector3f(world_homogeneous_vertex.X, world_homogeneous_vertex.Y, world_homogeneous_vertex.Z);
            }

            /// @todo   Clipping against camera z-boundaries!

            /// @todo   Surface normal!
            MATH::Vector3f unit_surface_normal = world_space_triangle.SurfaceNormal();
            if (cull_backfaces)
            {
                MATH::Vector3f view_direction = -camera.CoordinateFrame.Forward;
                float surface_normal_camera_view_direction_dot_product = MATH::Vector3f::DotProduct(unit_surface_normal, view_direction);
                bool triangle_facing_toward_camera = (surface_normal_camera_view_direction_dot_product < 0.0f);
                if (!triangle_facing_toward_camera)
                {
                    continue;
                }
            }

            // TRANSFORM THE TRIANGLE FOR PROPER CAMERA VIEWING.
            /// @todo   Combine this with previous loop?
            bool triangle_within_near_far_clip_planes = true;
            ScreenSpaceTriangle screen_space_triangle = 
            {
                .Material = world_space_triangle.Material,
                .VertexPositions = world_space_triangle.Vertices,
                .VertexColors = {}
            };
            for (std::size_t vertex_index = 0; vertex_index < triangle_vertex_count; ++vertex_index)
            {
                const MATH::Vector3f& world_vertex = world_space_triangle.Vertices[vertex_index];
                MATH::Vector4f world_homogeneous_vertex = MATH::Vector4f::HomogeneousPositionVector(world_vertex);

                MATH::Vector4f view_vertex = camera_view_transform * world_homogeneous_vertex;

                float near_z_boundary = -camera.NearClipPlaneViewDistance;
                float far_z_boundary = -camera.FarClipPlaneViewDistance;
                // "Direction" of >= comparisons is reversed due to being along negative Z axis.
                bool current_vertex_within_near_far_clip_planes = (near_z_boundary >= view_vertex.Z && view_vertex.Z >= far_z_boundary);
                triangle_within_near_far_clip_planes = triangle_within_near_far_clip_planes && current_vertex_within_near_far_clip_planes;

                MATH::Vector4f projected_vertex = camera_projection_transform * view_vertex;

                // The vertex must be de-homogenized.
                /// @todo   absolute value may only need to be taken into account for x and y, not z.
                MATH::Vector4f transformed_vertex = MATH::Vector4f::Scale(1.0f / projected_vertex.W, projected_vertex);

                MATH::Vector4f screen_space_vertex = screen_transform * transformed_vertex;
                screen_space_triangle.VertexPositions[vertex_index] = MATH::Vector3f(screen_space_vertex.X, screen_space_vertex.Y, screen_space_vertex.Z);
            }

            if (!triangle_within_near_far_clip_planes)
            {
                continue;
            }

            /// @todo   Render screen-space triangle!
            lights;

            switch (screen_space_triangle.Material->Shading)
            {
                case ShadingType::WIREFRAME:
                    screen_space_triangle.VertexColors =
                    {
                        screen_space_triangle.Material->WireframeColor,
                        screen_space_triangle.Material->WireframeColor,
                        screen_space_triangle.Material->WireframeColor,
                    };
                    break;
                case ShadingType::WIREFRAME_VERTEX_COLOR_INTERPOLATION:
                    screen_space_triangle.VertexColors =
                    {
                        screen_space_triangle.Material->VertexWireframeColors[0],
                        screen_space_triangle.Material->VertexWireframeColors[1],
                        screen_space_triangle.Material->VertexWireframeColors[2],
                    };
                    break;
                case ShadingType::FLAT:
                    screen_space_triangle.VertexColors =
                    {
                        screen_space_triangle.Material->FaceColor,
                        screen_space_triangle.Material->FaceColor,
                        screen_space_triangle.Material->FaceColor,
                    };
                    break;
                case ShadingType::FACE_VERTEX_COLOR_INTERPOLATION:
                    screen_space_triangle.VertexColors =
                    {
                        screen_space_triangle.Material->VertexFaceColors[0],
                        screen_space_triangle.Material->VertexFaceColors[1],
                        screen_space_triangle.Material->VertexFaceColors[2],
                    };
                    break;
                case ShadingType::GOURAUD:
                    screen_space_triangle.VertexColors =
                    {
                        screen_space_triangle.Material->VertexColors[0],
                        screen_space_triangle.Material->VertexColors[1],
                        screen_space_triangle.Material->VertexColors[2],
                    };
                    break;
                case ShadingType::TEXTURED:
                    screen_space_triangle.VertexColors =
                    {
                        screen_space_triangle.Material->VertexColors[0],
                        screen_space_triangle.Material->VertexColors[1],
                        screen_space_triangle.Material->VertexColors[2],
                    };
                    break;
                case ShadingType::MATERIAL:
                    /// @todo
                    break;
            }

            Render(screen_space_triangle, output_bitmap);
        }
    }

    /// Renders a single triangle to the render target.
    /// @param[in]  triangle - The triangle to render.
    /// @param[in,out]  render_target - The target to render to.
    void SoftwareRasterizationAlgorithm::Render(const ScreenSpaceTriangle& triangle, Bitmap& render_target)
    {
        // GET THE VERTICES.
        // They're needed for all kinds of shading.
        const MATH::Vector3f& first_vertex = triangle.VertexPositions[0];
        const MATH::Vector3f& second_vertex = triangle.VertexPositions[1];
        const MATH::Vector3f& third_vertex = triangle.VertexPositions[2];

        // RENDER THE TRIANGLE BASED ON SHADING TYPE.
        switch (triangle.Material->Shading)
        {
            case ShadingType::WIREFRAME:
            {
                // GET THE COLOR.
                /// @todo   Assuming all vertices have the same color here.
                Color wireframe_color = triangle.VertexColors[0];

                // DRAW THE FIRST EDGE.
                DrawLine(
                    first_vertex.X,
                    first_vertex.Y,
                    second_vertex.X,
                    second_vertex.Y,
                    wireframe_color,
                    render_target);

                // DRAW THE SECOND EDGE.
                DrawLine(
                    second_vertex.X,
                    second_vertex.Y,
                    third_vertex.X,
                    third_vertex.Y,
                    wireframe_color,
                    render_target);

                // DRAW THE THIRD EDGE.
                DrawLine(
                    third_vertex.X,
                    third_vertex.Y,
                    first_vertex.X,
                    first_vertex.Y,
                    wireframe_color,
                    render_target);
                break;
            }
            case ShadingType::WIREFRAME_VERTEX_COLOR_INTERPOLATION:
            {
                // GET THE VERTEX COLORS.
                Color vertex_0_wireframe_color = triangle.VertexColors[0];
                Color vertex_1_wireframe_color = triangle.VertexColors[1];
                Color vertex_2_wireframe_color = triangle.VertexColors[2];

                // DRAW THE FIRST EDGE.
                DrawLineWithInterpolatedColor(
                    first_vertex.X,
                    first_vertex.Y,
                    second_vertex.X,
                    second_vertex.Y,
                    vertex_0_wireframe_color,
                    vertex_1_wireframe_color,
                    render_target);

                // DRAW THE SECOND EDGE.
                DrawLineWithInterpolatedColor(
                    second_vertex.X,
                    second_vertex.Y,
                    third_vertex.X,
                    third_vertex.Y,
                    vertex_1_wireframe_color,
                    vertex_2_wireframe_color,
                    render_target);

                // DRAW THE THIRD EDGE.
                DrawLineWithInterpolatedColor(
                    third_vertex.X,
                    third_vertex.Y,
                    first_vertex.X,
                    first_vertex.Y,
                    vertex_2_wireframe_color,
                    vertex_0_wireframe_color,
                    render_target);
                break;
            }
            case ShadingType::FLAT:
            {
                // COMPUTE THE BARYCENTRIC COORDINATES OF THE TRIANGLE VERTICES.
                float top_vertex_signed_distance_from_bottom_edge = (
                    ((second_vertex.Y - third_vertex.Y) * first_vertex.X) +
                    ((third_vertex.X - second_vertex.X) * first_vertex.Y) +
                    (second_vertex.X * third_vertex.Y) -
                    (third_vertex.X * second_vertex.Y));
                float right_vertex_signed_distance_from_left_edge = (
                    ((second_vertex.Y - first_vertex.Y) * third_vertex.X) +
                    ((first_vertex.X - second_vertex.X) * third_vertex.Y) +
                    (second_vertex.X * first_vertex.Y) -
                    (first_vertex.X * second_vertex.Y));

                // GET THE BOUNDING RECTANGLE OF THE TRIANGLE.
                /// @todo   Create rectangle class.
                float min_x = std::min({ first_vertex.X, second_vertex.X, third_vertex.X });
                float max_x = std::max({ first_vertex.X, second_vertex.X, third_vertex.X });
                float min_y = std::min({ first_vertex.Y, second_vertex.Y, third_vertex.Y });
                float max_y = std::max({ first_vertex.Y, second_vertex.Y, third_vertex.Y });

                // Endpoints are clamped to avoid trying to draw really huge lines off-screen.
                constexpr float MIN_BITMAP_COORDINATE = 1.0f;

                float max_x_position = static_cast<float>(render_target.GetWidthInPixels() - 1);
                float clamped_min_x = MATH::Number::Clamp<float>(min_x, MIN_BITMAP_COORDINATE, max_x_position);
                float clamped_max_x = MATH::Number::Clamp<float>(max_x, MIN_BITMAP_COORDINATE, max_x_position);

                float max_y_position = static_cast<float>(render_target.GetHeightInPixels() - 1);
                float clamped_min_y = MATH::Number::Clamp<float>(min_y, MIN_BITMAP_COORDINATE, max_y_position);
                float clamped_max_y = MATH::Number::Clamp<float>(max_y, MIN_BITMAP_COORDINATE, max_y_position);

                // COLOR PIXELS WITHIN THE TRIANGLE.
                constexpr float ONE_PIXEL = 1.0f;
                for (float y = clamped_min_y; y <= clamped_max_y; y += ONE_PIXEL)
                {
                    for (float x = clamped_min_x; x <= clamped_max_x; x += ONE_PIXEL)
                    {
                        // COMPUTE THE BARYCENTRIC COORDINATES OF THE CURRENT PIXEL POSITION.
                        // The following diagram shows the order of the vertices:
                        //             first_vertex
                        //                 /\
                        //                /  \
                        // second_vertex /____\ third_vertex
                        float current_pixel_signed_distance_from_bottom_edge = (
                            ((second_vertex.Y - third_vertex.Y) * x) +
                            ((third_vertex.X - second_vertex.X) * y) +
                            (second_vertex.X * third_vertex.Y) -
                            (third_vertex.X * second_vertex.Y));
                        float scaled_signed_distance_of_current_pixel_relative_to_bottom_edge = (current_pixel_signed_distance_from_bottom_edge / top_vertex_signed_distance_from_bottom_edge);

                        float current_pixel_signed_distance_from_left_edge = (
                            ((second_vertex.Y - first_vertex.Y) * x) +
                            ((first_vertex.X - second_vertex.X) * y) +
                            (second_vertex.X * first_vertex.Y) -
                            (first_vertex.X * second_vertex.Y));
                        float scaled_signed_distance_of_current_pixel_relative_to_left_edge = (current_pixel_signed_distance_from_left_edge / right_vertex_signed_distance_from_left_edge);

                        float scaled_signed_distance_of_current_pixel_relative_to_right_edge = (
                            1.0f -
                            scaled_signed_distance_of_current_pixel_relative_to_left_edge -
                            scaled_signed_distance_of_current_pixel_relative_to_bottom_edge);

                        // CHECK IF THE PIXEL IS WITHIN THE TRIANGLE.
                        // It's allowed to be on the borders too.
                        constexpr float MIN_SIGNED_DISTANCE_TO_BE_ON_EDGE = 0.0f;
                        constexpr float MAX_SIGNED_DISTANCE_TO_BE_ON_VERTEX = 1.0f;
                        bool pixel_between_bottom_edge_and_top_vertex = (
                            (MIN_SIGNED_DISTANCE_TO_BE_ON_EDGE <= scaled_signed_distance_of_current_pixel_relative_to_bottom_edge) &&
                            (scaled_signed_distance_of_current_pixel_relative_to_bottom_edge <= MAX_SIGNED_DISTANCE_TO_BE_ON_VERTEX));
                        bool pixel_between_left_edge_and_right_vertex = (
                            (MIN_SIGNED_DISTANCE_TO_BE_ON_EDGE <= scaled_signed_distance_of_current_pixel_relative_to_left_edge) &&
                            (scaled_signed_distance_of_current_pixel_relative_to_left_edge <= MAX_SIGNED_DISTANCE_TO_BE_ON_VERTEX));
                        bool pixel_between_right_edge_and_left_vertex = (
                            (MIN_SIGNED_DISTANCE_TO_BE_ON_EDGE <= scaled_signed_distance_of_current_pixel_relative_to_right_edge) &&
                            (scaled_signed_distance_of_current_pixel_relative_to_right_edge <= MAX_SIGNED_DISTANCE_TO_BE_ON_VERTEX));
                        bool pixel_in_triangle = (
                            pixel_between_bottom_edge_and_top_vertex &&
                            pixel_between_left_edge_and_right_vertex &&
                            pixel_between_right_edge_and_left_vertex);
                        if (pixel_in_triangle)
                        {
                            // GET THE COLOR.
                            /// @todo   Assuming all vertices have the same color here.
                            Color face_color = triangle.VertexColors[0];

                            // DRAW THE COLORED PIXEL.
                            // The coordinates need to be rounded to integer in order
                            // to plot a pixel on a fixed grid.
                            render_target.WritePixel(
                                static_cast<unsigned int>(std::round(x)),
                                static_cast<unsigned int>(std::round(y)),
                                face_color);
                        }
                    }
                }
                break;
            }
            case ShadingType::FACE_VERTEX_COLOR_INTERPOLATION:
            case ShadingType::GOURAUD: /// @todo    This should be the same?
            case ShadingType::TEXTURED: /// @todo    This should be the same?
            case ShadingType::MATERIAL: /// @todo    This should be the same?
            {
                // COMPUTE THE BARYCENTRIC COORDINATES OF THE TRIANGLE VERTICES.
                float top_vertex_signed_distance_from_bottom_edge = (
                    ((second_vertex.Y - third_vertex.Y) * first_vertex.X) +
                    ((third_vertex.X - second_vertex.X) * first_vertex.Y) +
                    (second_vertex.X * third_vertex.Y) -
                    (third_vertex.X * second_vertex.Y));
                float right_vertex_signed_distance_from_left_edge = (
                    ((second_vertex.Y - first_vertex.Y) * third_vertex.X) +
                    ((first_vertex.X - second_vertex.X) * third_vertex.Y) +
                    (second_vertex.X * first_vertex.Y) -
                    (first_vertex.X * second_vertex.Y));

                // GET THE BOUNDING RECTANGLE OF THE TRIANGLE.
                /// @todo   Create rectangle class.
                float min_x = std::min({ first_vertex.X, second_vertex.X, third_vertex.X });
                float max_x = std::max({ first_vertex.X, second_vertex.X, third_vertex.X });
                float min_y = std::min({ first_vertex.Y, second_vertex.Y, third_vertex.Y });
                float max_y = std::max({ first_vertex.Y, second_vertex.Y, third_vertex.Y });

                // Endpoints are clamped to avoid trying to draw really huge lines off-screen.
                constexpr float MIN_BITMAP_COORDINATE = 1.0f;

                float max_x_position = static_cast<float>(render_target.GetWidthInPixels() - 1);
                float clamped_min_x = MATH::Number::Clamp<float>(min_x, MIN_BITMAP_COORDINATE, max_x_position);
                float clamped_max_x = MATH::Number::Clamp<float>(max_x, MIN_BITMAP_COORDINATE, max_x_position);

                float max_y_position = static_cast<float>(render_target.GetHeightInPixels() - 1);
                float clamped_min_y = MATH::Number::Clamp<float>(min_y, MIN_BITMAP_COORDINATE, max_y_position);
                float clamped_max_y = MATH::Number::Clamp<float>(max_y, MIN_BITMAP_COORDINATE, max_y_position);

                // COLOR PIXELS WITHIN THE TRIANGLE.
                constexpr float ONE_PIXEL = 1.0f;
                for (float y = clamped_min_y; y <= clamped_max_y; y += ONE_PIXEL)
                {
                    for (float x = clamped_min_x; x <= clamped_max_x; x += ONE_PIXEL)
                    {
                        // COMPUTE THE BARYCENTRIC COORDINATES OF THE CURRENT PIXEL POSITION.
                        // The following diagram shows the order of the vertices:
                        //             first_vertex
                        //                 /\
                        //                /  \
                        // second_vertex /____\ third_vertex
                        float current_pixel_signed_distance_from_bottom_edge = (
                            ((second_vertex.Y - third_vertex.Y) * x) +
                            ((third_vertex.X - second_vertex.X) * y) +
                            (second_vertex.X * third_vertex.Y) -
                            (third_vertex.X * second_vertex.Y));
                        float scaled_signed_distance_of_current_pixel_relative_to_bottom_edge = (current_pixel_signed_distance_from_bottom_edge / top_vertex_signed_distance_from_bottom_edge);

                        float current_pixel_signed_distance_from_left_edge = (
                            ((second_vertex.Y - first_vertex.Y) * x) +
                            ((first_vertex.X - second_vertex.X) * y) +
                            (second_vertex.X * first_vertex.Y) -
                            (first_vertex.X * second_vertex.Y));
                        float scaled_signed_distance_of_current_pixel_relative_to_left_edge = (current_pixel_signed_distance_from_left_edge / right_vertex_signed_distance_from_left_edge);

                        float scaled_signed_distance_of_current_pixel_relative_to_right_edge = (
                            1.0f -
                            scaled_signed_distance_of_current_pixel_relative_to_left_edge -
                            scaled_signed_distance_of_current_pixel_relative_to_bottom_edge);

                        // CHECK IF THE PIXEL IS WITHIN THE TRIANGLE.
                        // It's allowed to be on the borders too.
                        constexpr float MIN_SIGNED_DISTANCE_TO_BE_ON_EDGE = 0.0f;
                        constexpr float MAX_SIGNED_DISTANCE_TO_BE_ON_VERTEX = 1.0f;
                        bool pixel_between_bottom_edge_and_top_vertex = (
                            (MIN_SIGNED_DISTANCE_TO_BE_ON_EDGE <= scaled_signed_distance_of_current_pixel_relative_to_bottom_edge) &&
                            (scaled_signed_distance_of_current_pixel_relative_to_bottom_edge <= MAX_SIGNED_DISTANCE_TO_BE_ON_VERTEX));
                        bool pixel_between_left_edge_and_right_vertex = (
                            (MIN_SIGNED_DISTANCE_TO_BE_ON_EDGE <= scaled_signed_distance_of_current_pixel_relative_to_left_edge) &&
                            (scaled_signed_distance_of_current_pixel_relative_to_left_edge <= MAX_SIGNED_DISTANCE_TO_BE_ON_VERTEX));
                        bool pixel_between_right_edge_and_left_vertex = (
                            (MIN_SIGNED_DISTANCE_TO_BE_ON_EDGE <= scaled_signed_distance_of_current_pixel_relative_to_right_edge) &&
                            (scaled_signed_distance_of_current_pixel_relative_to_right_edge <= MAX_SIGNED_DISTANCE_TO_BE_ON_VERTEX));
                        bool pixel_in_triangle = (
                            pixel_between_bottom_edge_and_top_vertex &&
                            pixel_between_left_edge_and_right_vertex &&
                            pixel_between_right_edge_and_left_vertex);
                        if (pixel_in_triangle)
                        {
                            // The color needs to be interpolated with this kind of shading.
                            Color interpolated_color = GRAPHICS::Color::BLACK;

                            const Color& first_vertex_color = triangle.VertexColors[0];
                            const Color& second_vertex_color = triangle.VertexColors[1];
                            const Color& third_vertex_color = triangle.VertexColors[2];
                            interpolated_color.Red = (
                                (scaled_signed_distance_of_current_pixel_relative_to_right_edge * third_vertex_color.Red) +
                                (scaled_signed_distance_of_current_pixel_relative_to_left_edge * second_vertex_color.Red) +
                                (scaled_signed_distance_of_current_pixel_relative_to_bottom_edge * first_vertex_color.Red));
                            interpolated_color.Green = (
                                (scaled_signed_distance_of_current_pixel_relative_to_right_edge * third_vertex_color.Green) +
                                (scaled_signed_distance_of_current_pixel_relative_to_left_edge * second_vertex_color.Green) +
                                (scaled_signed_distance_of_current_pixel_relative_to_bottom_edge * first_vertex_color.Green));
                            interpolated_color.Blue = (
                                (scaled_signed_distance_of_current_pixel_relative_to_right_edge * third_vertex_color.Blue) +
                                (scaled_signed_distance_of_current_pixel_relative_to_left_edge * second_vertex_color.Blue) +
                                (scaled_signed_distance_of_current_pixel_relative_to_bottom_edge * first_vertex_color.Blue));
                            interpolated_color.Clamp();

                            if (ShadingType::TEXTURED == triangle.Material->Shading)
                            {
                                // INTERPOLATE THE TEXTURE COORDINATES.
                                const MATH::Vector2f& first_texture_coordinate = triangle.Material->VertexTextureCoordinates[0];
                                const MATH::Vector2f& second_texture_coordinate = triangle.Material->VertexTextureCoordinates[1];
                                const MATH::Vector2f& third_texture_coordinate = triangle.Material->VertexTextureCoordinates[2];

                                MATH::Vector2f interpolated_texture_coordinate;
                                interpolated_texture_coordinate.X = (
                                    (scaled_signed_distance_of_current_pixel_relative_to_right_edge * third_texture_coordinate.X) +
                                    (scaled_signed_distance_of_current_pixel_relative_to_left_edge * second_texture_coordinate.X) +
                                    (scaled_signed_distance_of_current_pixel_relative_to_bottom_edge * first_texture_coordinate.X));
                                interpolated_texture_coordinate.Y = (
                                    (scaled_signed_distance_of_current_pixel_relative_to_right_edge * third_texture_coordinate.Y) +
                                    (scaled_signed_distance_of_current_pixel_relative_to_left_edge * second_texture_coordinate.Y) +
                                    (scaled_signed_distance_of_current_pixel_relative_to_bottom_edge * first_texture_coordinate.Y));
                                // Clamping.
                                if (interpolated_texture_coordinate.X < 0.0f)
                                {
                                    interpolated_texture_coordinate.X = 0.0f;
                                }
                                else if (interpolated_texture_coordinate.X > 1.0f)
                                {
                                    interpolated_texture_coordinate.X = 1.0f;
                                }
                                if (interpolated_texture_coordinate.Y < 0.0f)
                                {
                                    interpolated_texture_coordinate.Y = 0.0f;
                                }
                                else if (interpolated_texture_coordinate.Y > 1.0f)
                                {
                                    interpolated_texture_coordinate.Y = 1.0f;
                                }

                                // LOOK UP THE TEXTURE COLOR AT THE COORDINATES.
                                unsigned int texture_width_in_pixels = triangle.Material->Texture->GetWidthInPixels();
                                unsigned int texture_pixel_x_coordinate = static_cast<unsigned int>(texture_width_in_pixels * interpolated_texture_coordinate.X);

                                unsigned int texture_height_in_pixels = triangle.Material->Texture->GetHeightInPixels();
                                unsigned int texture_pixel_y_coordinate = static_cast<unsigned int>(texture_height_in_pixels * interpolated_texture_coordinate.Y);

                                Color texture_color = triangle.Material->Texture->GetPixel(texture_pixel_x_coordinate, texture_pixel_y_coordinate);

                                interpolated_color = Color::ComponentMultiplyRedGreenBlue(interpolated_color, texture_color);
                                interpolated_color.Clamp();
                            }

                            // The coordinates need to be rounded to integer in order
                            // to plot a pixel on a fixed grid.
                            render_target.WritePixel(
                                static_cast<unsigned int>(std::round(x)),
                                static_cast<unsigned int>(std::round(y)),
                                interpolated_color);
                        }
                    }
                }
                break;
            }
        }
    }

    /// Renders a line with the specified endpoints (in screen coordinates).
    /// @param[in]  start_x - The starting x coordinate of the line.
    /// @param[in]  start_y - The starting y coordinate of the line.
    /// @param[in]  end_x - The ending x coordinate of the line.
    /// @param[in]  end_y - The ending y coordinate of the line.
    /// @param[in]  color - The color of the line to draw.
    /// @param[in,out]  render_target - The target to render to.
    void SoftwareRasterizationAlgorithm::DrawLine(
        const float start_x,
        const float start_y,
        const float end_x,
        const float end_y,
        const Color& color,
        Bitmap& render_target)
    {
        // CLAMP ENDPOINTS TO AVOID TRYING TO DRAW REALLY HUGE LINES OFF-SCREEN.
        constexpr float MIN_BITMAP_COORDINATE = 1.0f;

        float max_x_position = static_cast<float>(render_target.GetWidthInPixels() - 1);
        float clamped_start_x = MATH::Number::Clamp<float>(start_x, MIN_BITMAP_COORDINATE, max_x_position);
        float clamped_end_x = MATH::Number::Clamp<float>(end_x, MIN_BITMAP_COORDINATE, max_x_position);

        float max_y_position = static_cast<float>(render_target.GetHeightInPixels() - 1);
        float clamped_start_y = MATH::Number::Clamp<float>(start_y, MIN_BITMAP_COORDINATE, max_y_position);
        float clamped_end_y = MATH::Number::Clamp<float>(end_y, MIN_BITMAP_COORDINATE, max_y_position);

        // COMPUTE THE INCREMENTS ALONG EACH AXIS FOR EACH PIXEL.
        // Each time we draw a pixel, we need to move slightly further along the axes.
        float delta_x = clamped_end_x - clamped_start_x;
        float delta_y = clamped_end_y - clamped_start_y;
        float length = std::max(std::abs(delta_x), std::abs(delta_y));
        float x_increment = delta_x / length;
        float y_increment = delta_y / length;

        // HAVE THE LINE START BEING DRAWN AT THE STARTING COORDINATES.
        float x = clamped_start_x;
        float y = clamped_start_y;

        // DRAW PIXELS FOR THE LINE.
        for (float pixel_index = 0.0f; pixel_index <= length; ++pixel_index)
        {
            // PREVENT WRITING BEYOND THE BOUNDARIES OF THE RENDER TARGET.
            bool x_boundary_exceeded = (
                (x < 0.0f) ||
                (x > max_x_position));
            bool y_boundary_exceeded = (
                (y < 0.0f) ||
                (y > max_y_position));
            bool boundary_exceeded = (x_boundary_exceeded || y_boundary_exceeded);
            if (boundary_exceeded)
            {
                // Continue to the next iteration of the loop in
                // case there is another pixel to draw.
                x += x_increment;
                y += y_increment;
                continue;
            }

            // DRAW A PIXEL AT THE CURRENT POSITION.
            // The coordinates need to be rounded to integer in order
            // to plot a pixel on a fixed grid.
            render_target.WritePixel(
                static_cast<unsigned int>(std::round(x)),
                static_cast<unsigned int>(std::round(y)),
                color);

            // MOVE ALONG THE LINE FOR THE NEXT PIXEL.
            x += x_increment;
            y += y_increment;
        }
    }

    /// Renders a line with the specified endpoints (in screen coordinates) and interpolated color.
    /// @param[in]  start_x - The starting x coordinate of the line.
    /// @param[in]  start_y - The starting y coordinate of the line.
    /// @param[in]  end_x - The ending x coordinate of the line.
    /// @param[in]  end_y - The ending y coordinate of the line.
    /// @param[in]  start_color - The color of the line at the starting coordinate.
    /// @param[in]  end_color - The color of the line at the ending coordinate.
    /// @param[in,out]  render_target - The target to render to.
    void SoftwareRasterizationAlgorithm::DrawLineWithInterpolatedColor(
        const float start_x,
        const float start_y,
        const float end_x,
        const float end_y,
        const Color& start_color,
        const Color& end_color,
        Bitmap& render_target)
    {
        // CLAMP ENDPOINTS TO AVOID TRYING TO DRAW REALLY HUGE LINES OFF-SCREEN.
        constexpr float MIN_BITMAP_COORDINATE = 1.0f;

        float max_x_position = static_cast<float>(render_target.GetWidthInPixels() - 1);
        float clamped_start_x = MATH::Number::Clamp<float>(start_x, MIN_BITMAP_COORDINATE, max_x_position);
        float clamped_end_x = MATH::Number::Clamp<float>(end_x, MIN_BITMAP_COORDINATE, max_x_position);

        float max_y_position = static_cast<float>(render_target.GetHeightInPixels() - 1);
        float clamped_start_y = MATH::Number::Clamp<float>(start_y, MIN_BITMAP_COORDINATE, max_y_position);
        float clamped_end_y = MATH::Number::Clamp<float>(end_y, MIN_BITMAP_COORDINATE, max_y_position);

        // COMPUTE THE LENGTH OF THE ENTIRE LINE.
        MATH::Vector2f vector_from_start_to_end(clamped_end_x - clamped_start_x, clamped_end_y - clamped_start_y);
        float line_length = vector_from_start_to_end.Length();

        // COMPUTE THE INCREMENTS ALONG EACH AXIS FOR EACH PIXEL.
        // Each time we draw a pixel, we need to move slightly
        // further along the axes.
        float delta_x = clamped_end_x - clamped_start_x;
        float delta_y = clamped_end_y - clamped_start_y;
        float length = std::max(std::abs(delta_x), std::abs(delta_y));
        float x_increment = delta_x / length;
        float y_increment = delta_y / length;

        // HAVE THE LINE START BEING DRAWN AT THE STARTING COORDINATES.
        float x = clamped_start_x;
        float y = clamped_start_y;

        // DRAW PIXELS FOR THE LINE.
        for (float pixel_index = 0.0f; pixel_index <= length; ++pixel_index)
        {
            // PREVENT WRITING BEYOND THE BOUNDARIES OF THE RENDER TARGET.
            bool x_boundary_exceeded = (
                (x < 0.0f) ||
                (x > max_x_position));
            bool y_boundary_exceeded = (
                (y < 0.0f) ||
                (y > max_y_position));
            bool boundary_exceeded = (x_boundary_exceeded || y_boundary_exceeded);
            if (boundary_exceeded)
            {
                // Continue to the next iteration of the loop in
                // case there is another pixel to draw.
                x += x_increment;
                y += y_increment;
                continue;
            }

            // CALCULATE THE COLOR AT THE CURRENT POINT.
            MATH::Vector2f vector_to_current_pixel(x - start_x, y - start_y);
            float length_to_current_pixel_from_line_start = vector_to_current_pixel.Length();
            float ratio_toward_end_of_line = (length_to_current_pixel_from_line_start / line_length);
            Color interpolated_color = Color::InterpolateRedGreenBlue(start_color, end_color, ratio_toward_end_of_line);

            // DRAW A PIXEL AT THE CURRENT POSITION.
            // The coordinates need to be rounded to integer in order
            // to plot a pixel on a fixed grid.
            render_target.WritePixel(
                static_cast<unsigned int>(std::round(x)),
                static_cast<unsigned int>(std::round(y)),
                interpolated_color);

            // MOVE ALONG THE LINE FOR THE NEXT PIXEL.
            x += x_increment;
            y += y_increment;
        }
    }
}
