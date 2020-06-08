#include <algorithm>
#include <cmath>
#include "Graphics/Renderer.h"

namespace GRAPHICS
{
    /// Renders some text onto the render target.
    /// @param[in]  text - The text to render.
    /// @param[in,out]  render_target - The target to render to.
    void Renderer::Render(const GUI::Text& text, RenderTarget& render_target) const
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

    /// Renders a 3D object to the render target.
    /// @param[in]  object_3D - The object to render.
    /// @param[in]  lights - Any lights that should illuminate the object.
    /// @param[in,out]  render_target - The target to render to.
    void Renderer::Render(const Object3D& object_3D, const std::vector<Light>& lights, RenderTarget& render_target) const
    {
        // COMPUTE THE FINAL TRANSFORMATION MATRIX FOR THE OBJECT.
        MATH::Matrix4x4f object_world_transform = object_3D.WorldTransform();
        MATH::Matrix4x4f camera_view_transform = Camera.ViewTransform();

        /// @todo   Figure out how we want to put projections into camera class.
        constexpr float WORLD_HALF_SIZE = 100.0f;
        const float LEFT_X_WORLD_BOUNDARY = Camera.WorldPosition.X - WORLD_HALF_SIZE;
        const float RIGHT_X_WORLD_BOUNDARY = Camera.WorldPosition.X + WORLD_HALF_SIZE;
        const float BOTTOM_Y_WORLD_BOUNDARY = Camera.WorldPosition.Y - WORLD_HALF_SIZE;
        const float TOP_Y_WORLD_BOUNDARY = Camera.WorldPosition.Y + WORLD_HALF_SIZE;
        //const float NEAR_Z_WORLD_BOUNDARY = Camera.WorldPosition.Z - 0.5f;
        //const float FAR_Z_WORLD_BOUNDARY = Camera.WorldPosition.Z - 2.5f;
        const float NEAR_Z_WORLD_BOUNDARY = Camera.WorldPosition.Z - 1.0f;
        const float FAR_Z_WORLD_BOUNDARY = Camera.WorldPosition.Z - 500.0f;
        MATH::Matrix4x4f orthographic_projection_transform = Camera::OrthographicProjection(
            LEFT_X_WORLD_BOUNDARY,
            RIGHT_X_WORLD_BOUNDARY,
            BOTTOM_Y_WORLD_BOUNDARY,
            TOP_Y_WORLD_BOUNDARY,
            NEAR_Z_WORLD_BOUNDARY,
            FAR_Z_WORLD_BOUNDARY);

        const MATH::Angle<float>::Degrees VERTICAL_FIELD_OF_VIEW_IN_DEGREES(90.0f);
        const float ASPECT_RATIO_WIDTH_OVER_HEIGHT = 1.0f;
        MATH::Matrix4x4f perspective_projection_transform = Camera::PerspectiveProjection(
            VERTICAL_FIELD_OF_VIEW_IN_DEGREES,
            ASPECT_RATIO_WIDTH_OVER_HEIGHT,
            NEAR_Z_WORLD_BOUNDARY,
            FAR_Z_WORLD_BOUNDARY);

        MATH::Matrix4x4f flip_y_transform = MATH::Matrix4x4f::Scale(MATH::Vector3f(1.0f, -1.0f, 1.0f));
        MATH::Matrix4x4f scale_to_screen_transform = MATH::Matrix4x4f::Scale(MATH::Vector3f(
            static_cast<float>(render_target.GetWidthInPixels()) / 2.0f, 
            static_cast<float>(render_target.GetHeightInPixels()) / 2.0f,
            1.0f));
        MATH::Matrix4x4f translate_to_screen_center_transform = MATH::Matrix4x4f::Translation(MATH::Vector3f(
            static_cast<float>(render_target.GetWidthInPixels()) / 2.0f,
            static_cast<float>(render_target.GetHeightInPixels()) / 2.0f,
            0.0f));
        MATH::Matrix4x4 screen_transform = translate_to_screen_center_transform * scale_to_screen_transform * flip_y_transform;

        //MATH::Matrix4x4f final_transform = screen_transform * perspective_projection_transform * camera_view_transform * object_world_transform;

        // RENDER EACH TRIANGLE OF THE OBJECT.
        for (const auto& local_triangle : object_3D.Triangles)
        {
            // TRANSFORM THE TRIANGLE INTO SCREEN-SPACE.
            bool triangle_within_camera_z_boundaries = false;
            Triangle screen_space_triangle = local_triangle;

            /// @todo   Look at reducing duplication from world space triangle.
            Triangle world_space_triangle = local_triangle;
            for (auto& vertex : world_space_triangle.Vertices)
            {
                // Y must be flipped since the world Y coordinates are positive going up,
                // the opposite is true for the screen coordinates.
                vertex.Y = -vertex.Y;
                MATH::Vector4f homogeneous_vertex = MATH::Vector4f::HomogeneousPositionVector(vertex);

                MATH::Vector4f world_vertex = object_world_transform * homogeneous_vertex;
                vertex = MATH::Vector3f(world_vertex.X, world_vertex.Y, world_vertex.Z);
            }

            // Colors based on lighting also need to be computed.
            std::array<Color, Triangle::VERTEX_COUNT> triangle_vertex_colors =
            { 
                Color::BLACK, 
                Color::BLACK,
                Color::BLACK,
            };
            for (std::size_t vertex_index = 0; vertex_index < screen_space_triangle.Vertices.size(); ++vertex_index)
            {
                MATH::Vector3f& vertex = screen_space_triangle.Vertices[vertex_index];

                // Y must be flipped since the world Y coordinates are positive going up,
                // the opposite is true for the screen coordinates.
                vertex.Y = -vertex.Y;
                MATH::Vector4f homogeneous_vertex = MATH::Vector4f::HomogeneousPositionVector(vertex);

                MATH::Vector4f world_vertex = object_world_transform * homogeneous_vertex;
                MATH::Vector4f view_vertex = camera_view_transform * world_vertex;
                MATH::Vector4f projected_vertex = perspective_projection_transform * view_vertex;
                MATH::Vector4f screen_vertex = screen_transform * projected_vertex;
                MATH::Vector4f transformed_vertex = screen_vertex;

                //MATH::Vector4f transformed_vertex = final_transform * homogeneous_vertex;
                vertex = MATH::Vector3f(transformed_vertex.X, transformed_vertex.Y, transformed_vertex.Z);
                // The vertex must be de-homogenized.
                vertex = MATH::Vector3f::Scale(1.0f / transformed_vertex.W, vertex);

                // @todo    I'm not sure this clipping is correct.  More study/testing needed.
                bool vertex_within_camera_z_boundaries = 
                    // The vertex must be within the boundaries of the camera's viewing planes.
                    // Since these boundaries are facing along the negative z axis,
                    // the far boundary is actually larger (less negative) than the near boundary.
                    ((FAR_Z_WORLD_BOUNDARY <= world_vertex.Z) && (world_vertex.Z <= NEAR_Z_WORLD_BOUNDARY)) &&
                    // Within the camera viewing plane's boundaries, the w coordinate should never be
                    // greater than the z coordinate - that would indicate that we've basically
                    // "flipped" the size of the viewing plane that we're on.  In those cases,
                    // we must prevent incorrect (flipped) rendering.
                    (std::abs(transformed_vertex.Z) >= std::abs(transformed_vertex.W));
                triangle_within_camera_z_boundaries = (triangle_within_camera_z_boundaries || vertex_within_camera_z_boundaries);

                // COMPUTE LIGHTING FOR THE VERTEX.
                // The initial color is based on the shading type.
                Color vertex_color = triangle_vertex_colors[vertex_index];
                switch (world_space_triangle.Material->Shading)
                {
                    case ShadingType::WIREFRAME:
                        vertex_color = screen_space_triangle.Material->WireframeColor;
                        break;
                    case ShadingType::WIREFRAME_VERTEX_COLOR_INTERPOLATION:
                        vertex_color = screen_space_triangle.Material->VertexWireframeColors[vertex_index];
                        break;
                    case ShadingType::FLAT:
                        vertex_color = screen_space_triangle.Material->FaceColor;
                        break;
                    case ShadingType::FACE_VERTEX_COLOR_INTERPOLATION:
                        vertex_color = screen_space_triangle.Material->VertexFaceColors[vertex_index];
                        break;
                    case ShadingType::GOURAUD:
                        vertex_color = screen_space_triangle.Material->VertexColors[vertex_index];
                        break;
                    case ShadingType::TEXTURED:
                        vertex_color = screen_space_triangle.Material->VertexColors[vertex_index];
                        break;
                    case ShadingType::MATERIAL:
                        /// @todo   Leave as white?  Currently done to have some lighting (as opposed to black).
                        vertex_color = Color(1.0f, 1.0f, 1.0f, 1.0f);
                        break;
                }

                Color light_total_color = Color::BLACK;
                for (const Light& light : lights)
                {
                    // COMPUTE SHADING BASED ON TYPE OF LIGHT.
                    if (LightType::AMBIENT == light.Type)
                    {
                        if (ShadingType::MATERIAL == world_space_triangle.Material->Shading)
                        {
                            light_total_color += Color::ComponentMultiplyRedGreenBlue(light.Color, world_space_triangle.Material->AmbientColor);
                        }
                        else
                        {
                            light_total_color += light.Color;
                        }
                    }
                    else
                    {
                        // COMPUTE THE SURFACE NORMAL.
                        /// @todo   Vertex normals?
                        MATH::Vector3f unit_surface_normal = world_space_triangle.SurfaceNormal();
                        
                        // GET THE DIRECTION OF THE LIGHT.
                        MATH::Vector3f current_world_vertex = MATH::Vector3f(world_vertex.X, world_vertex.Y, world_vertex.Z);
                        MATH::Vector3f direction_from_vertex_to_light;
                        if (LightType::DIRECTIONAL == light.Type)
                        {
                            // The computations are based on the opposite direction.
                            direction_from_vertex_to_light = MATH::Vector3f::Scale(-1.0f, light.DirectionalLightDirection);
                        }
                        else if (LightType::POINT == light.Type)
                        {
                            direction_from_vertex_to_light = light.PointLightWorldPosition - current_world_vertex;
                        }

                        // ADD DIFFUSE COLOR FROM THE CURRENT LIGHT.
                        // This is based on the Lambertian shading model.
                        // An object is maximally illuminated when facing toward the light.
                        // An object tangent to the light direction or facing away receives no illumination.
                        // In-between, the amount of illumination is proportional to the cosine of the angle between
                        // the light and surface normal (where the cosine can be computed via the dot product).
                        MATH::Vector3f unit_direction_from_point_to_light = MATH::Vector3f::Normalize(direction_from_vertex_to_light);
                        constexpr float NO_ILLUMINATION = 0.0f;
                        float illumination_proportion = MATH::Vector3f::DotProduct(unit_surface_normal, unit_direction_from_point_to_light);
                        illumination_proportion = std::max(NO_ILLUMINATION, illumination_proportion);
                        Color current_light_color = Color::ScaleRedGreenBlue(illumination_proportion, light.Color);
                        if (ShadingType::MATERIAL == world_space_triangle.Material->Shading)
                        {
                            light_total_color += Color::ComponentMultiplyRedGreenBlue(current_light_color, world_space_triangle.Material->DiffuseColor);
                        }
                        else
                        {
                            light_total_color += current_light_color;
                        }

                        // ADD SPECULAR COLOR FROM THE CURRENT LIGHT.
                        /// @todo   Is this how we want to handle specularity?
                        if (world_space_triangle.Material->SpecularPower > 1.0f)
                        {
                            MATH::Vector3f reflected_light_along_surface_normal = MATH::Vector3f::Scale(2.0f * illumination_proportion, unit_surface_normal);
                            MATH::Vector3f reflected_light_direction = reflected_light_along_surface_normal - unit_direction_from_point_to_light;
                            MATH::Vector3f unit_reflected_light_direction = MATH::Vector3f::Normalize(reflected_light_direction);

                            MATH::Vector3f ray_from_vertex_to_camera = Camera.WorldPosition - current_world_vertex;
                            MATH::Vector3f normalized_ray_from_vertex_to_camera = MATH::Vector3f::Normalize(ray_from_vertex_to_camera);
                            float specular_proportion = MATH::Vector3f::DotProduct(normalized_ray_from_vertex_to_camera, unit_reflected_light_direction);
                            specular_proportion = std::max(NO_ILLUMINATION, specular_proportion);
                            specular_proportion = std::pow(specular_proportion, world_space_triangle.Material->SpecularPower);

                            Color current_light_specular_color = Color::ScaleRedGreenBlue(specular_proportion, light.Color);

                            if (ShadingType::MATERIAL == world_space_triangle.Material->Shading)
                            {
                                light_total_color += Color::ComponentMultiplyRedGreenBlue(current_light_specular_color, world_space_triangle.Material->SpecularColor);
                            }
                            else
                            {
                                light_total_color += current_light_specular_color;
                            }
                        }
                    }
                }
                vertex_color = Color::ComponentMultiplyRedGreenBlue(vertex_color, light_total_color);
                vertex_color.Clamp();
                triangle_vertex_colors[vertex_index] = vertex_color;
            }

            /// @todo   This is a bit of hack for flat shading...
            if (ShadingType::FLAT == world_space_triangle.Material->Shading)
            {
                float total_red = 0.0f;
                float total_green = 0.0f;
                float total_blue = 0.0f;
                for (const Color& vertex_color : triangle_vertex_colors)
                {
                    total_red += vertex_color.Red;
                    total_green += vertex_color.Green;
                    total_blue += vertex_color.Blue;
                }
                float average_red = total_red / 3.0f;
                float average_green = total_green / 3.0f;
                float average_blue = total_blue / 3.0f;
                Color average_vertex_color(average_red, average_green, average_blue, 1.0f);
                triangle_vertex_colors[0] = average_vertex_color;
                triangle_vertex_colors[1] = average_vertex_color;
                triangle_vertex_colors[2] = average_vertex_color;
            }

            // RENDER THE SCREEN-SPACE TRIANGLE.
            if (triangle_within_camera_z_boundaries)
            {
                /// @todo   Collapse triangle + vertex colors into single data type?
                Render(screen_space_triangle, triangle_vertex_colors, render_target);
            }
        }
    }

    /// Renders a single triangle to the render target.
    /// @param[in]  triangle - The triangle to render (in screen-space coordinates).
    /// @param[in]  triangle_vertex_colors - The vertex colors of the triangle.
    /// @param[in,out]  render_target - The target to render to.
    void Renderer::Render(const Triangle& triangle, const std::array<GRAPHICS::Color, Triangle::VERTEX_COUNT>& triangle_vertex_colors, RenderTarget& render_target) const
    {
        // GET THE VERTICES.
        // They're needed for all kinds of shading.
        const MATH::Vector3f& first_vertex = triangle.Vertices[0];
        const MATH::Vector3f& second_vertex = triangle.Vertices[1];
        const MATH::Vector3f& third_vertex = triangle.Vertices[2];

        // RENDER THE TRIANGLE BASED ON SHADING TYPE.
        switch (triangle.Material->Shading)
        {
            case ShadingType::WIREFRAME:
            {
                // GET THE COLOR.
                /// @todo   Assuming all vertices have the same color here.
                Color wireframe_color = triangle_vertex_colors[0];

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
                Color vertex_0_wireframe_color = triangle_vertex_colors[0];
                Color vertex_1_wireframe_color = triangle_vertex_colors[1];
                Color vertex_2_wireframe_color = triangle_vertex_colors[2];

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

                // COLOR PIXELS WITHIN THE TRIANGLE.
                constexpr float ONE_PIXEL = 1.0f;
                for (float y = min_y; y <= max_y; y += ONE_PIXEL)
                {
                    for (float x = min_x; x <= max_x; x += ONE_PIXEL)
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
                            Color face_color = triangle_vertex_colors[0];

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

                // COLOR PIXELS WITHIN THE TRIANGLE.
                constexpr float ONE_PIXEL = 1.0f;
                for (float y = min_y; y <= max_y; y += ONE_PIXEL)
                {
                    for (float x = min_x; x <= max_x; x += ONE_PIXEL)
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

                            const Color& first_vertex_color = triangle_vertex_colors[0];
                            const Color& second_vertex_color = triangle_vertex_colors[1];
                            const Color& third_vertex_color = triangle_vertex_colors[2];
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
                                unsigned int texture_width_in_pixels = triangle.Material->Texture->Bitmap.GetWidthInPixels();                                
                                unsigned int texture_pixel_x_coordinate = static_cast<unsigned int>(texture_width_in_pixels * interpolated_texture_coordinate.X);

                                unsigned int texture_height_in_pixels = triangle.Material->Texture->Bitmap.GetHeightInPixels();
                                unsigned int texture_pixel_y_coordinate = static_cast<unsigned int>(texture_height_in_pixels * interpolated_texture_coordinate.Y);

                                Color texture_color = triangle.Material->Texture->Bitmap.GetPixel(texture_pixel_x_coordinate, texture_pixel_y_coordinate);

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

        /// @todo   Look at reducing duplication of the above!

#if 0
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

        // COLOR PIXELS WITHIN THE TRIANGLE.
        constexpr float ONE_PIXEL = 1.0f;
        for (float y = min_y; y <= max_y; y += ONE_PIXEL)
        {
            for (float x = min_x; x <= max_x; x += ONE_PIXEL)
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
                    // The coordinates need to be rounded to integer in order
                    // to plot a pixel on a fixed grid.
                    render_target.WritePixel(
                        static_cast<unsigned int>(std::round(x)),
                        static_cast<unsigned int>(std::round(y)),
                        Color(0.0f, 0.0f, 1.0f, 1.0f));
                }
                else
                {
                    // The coordinates need to be rounded to integer in order
                    // to plot a pixel on a fixed grid.
                    /*render_target.WritePixel(
                        static_cast<unsigned int>(std::round(x)),
                        static_cast<unsigned int>(std::round(y)),
                        Color(1.0f, 0.0f, 0.0f, 1.0f));*/
                }
            }
        }
#endif
    }

    /// Renders a line with the specified endpoints (in screen coordinates).
    /// @param[in]  start_x - The starting x coordinate of the line.
    /// @param[in]  start_y - The starting y coordinate of the line.
    /// @param[in]  end_x - The ending x coordinate of the line.
    /// @param[in]  end_y - The ending y coordinate of the line.
    /// @param[in]  color - The color of the line to draw.
    /// @param[in,out]  render_target - The target to render to.
    void Renderer::DrawLine(
        const float start_x,
        const float start_y,
        const float end_x,
        const float end_y,
        const Color& color,
        RenderTarget& render_target) const
    {
        // COMPUTE THE INCREMENTS ALONG EACH AXIS FOR EACH PIXEL.
        // Each time we draw a pixel, we need to move slightly
        // further along the axes.
        float delta_x = end_x - start_x;
        float delta_y = end_y - start_y;
        float length = std::max(std::abs(delta_x), std::abs(delta_y));
        float x_increment = delta_x / length;
        float y_increment = delta_y / length;

        // HAVE THE LINE START BEING DRAWN AT THE STARTING COORDINATES.
        float x = start_x;
        float y = start_y;

        // GET THE MAXIMUM POSSIBLE POSITION VALUES.
        float max_x_position = static_cast<float>(render_target.GetWidthInPixels() - 1);
        float max_y_position = static_cast<float>(render_target.GetHeightInPixels() - 1);

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
    void Renderer::DrawLineWithInterpolatedColor(
        const float start_x,
        const float start_y,
        const float end_x,
        const float end_y,
        const Color& start_color,
        const Color& end_color,
        RenderTarget& render_target) const
    {
        // COMPUTE THE LENGTH OF THE ENTIRE LINE.
        MATH::Vector2f vector_from_start_to_end(end_x - start_x, end_y - start_y);
        float line_length = vector_from_start_to_end.Length();

        // COMPUTE THE INCREMENTS ALONG EACH AXIS FOR EACH PIXEL.
        // Each time we draw a pixel, we need to move slightly
        // further along the axes.
        float delta_x = end_x - start_x;
        float delta_y = end_y - start_y;
        float length = std::max(std::abs(delta_x), std::abs(delta_y));
        float x_increment = delta_x / length;
        float y_increment = delta_y / length;

        // HAVE THE LINE START BEING DRAWN AT THE STARTING COORDINATES.
        float x = start_x;
        float y = start_y;

        // GET THE MAXIMUM POSSIBLE POSITION VALUES.
        float max_x_position = static_cast<float>(render_target.GetWidthInPixels() - 1);
        float max_y_position = static_cast<float>(render_target.GetHeightInPixels() - 1);

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
