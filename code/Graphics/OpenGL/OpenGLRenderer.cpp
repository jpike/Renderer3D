#include <algorithm>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <GL/gl3w.h>
#include "Graphics/OpenGL/OpenGLRenderer.h"

namespace GRAPHICS::OPEN_GL
{
    /// Renders an entire 3D scene.
    /// @param[in]  scene - The scene to render.
    /// @param[in]  camera - The camera to use to view the scene.
    void OpenGLRenderer::Render(const Scene& scene, const Camera& camera) const
    {
        glEnable(GL_DEPTH_TEST);

        ViewingTransformations viewing_transformations(camera);

        ClearScreen(scene.BackgroundColor);

        for (const auto& object_3D : scene.Objects)
        {            
            Render(object_3D, viewing_transformations);
        }

#if OLD_OPEN_GL
        // SET LIGHTING IF APPROPRIATE.
        /// @todo   Better way to control lighting than presence/absence of light?
        bool lighting_enabled = bool(scene.PointLights);
        if (lighting_enabled)
        {
            // ENABLE LIGHTING.
            glEnable(GL_LIGHTING);

            // SET EACH LIGHT.
            // While lights only up to GL_LIGHT7 are explicitly defined,
            // more lights are actually supported, and the greater lights
            // can be specified by simple addition (http://docs.gl/gl2/glLight).
            GLenum light_count = static_cast<GLenum>(scene.PointLights->size());
            GLenum max_light_index = std::min<GLenum>(light_count, GL_MAX_LIGHTS);
            for (GLenum light_index = 0; light_index < max_light_index; ++light_index)
            {
                // ENABLE THE CURRENT LIGHT.
                GLenum light_id = GL_LIGHT0 + light_index;
                glEnable(light_id);
                const float LIGHT_ATTENUATION = 2.0f;
                glLightfv(light_id, GL_QUADRATIC_ATTENUATION, &LIGHT_ATTENUATION);

                // SET PROPERTIES RELATED TO THE CURRENT TYPE OF LIGHT.
                const Light& current_light = (*scene.PointLights)[light_index];
                float light_color[] = { current_light.Color.Red, current_light.Color.Green, current_light.Color.Blue, current_light.Color.Alpha };
                const float NO_LIGHT_COLOR[] = { 0.0f, 0.0f, 0.0f, 1.0f };
                switch (current_light.Type)
                {
                    case LightType::AMBIENT:
                    {
                        glLightfv(light_id, GL_AMBIENT, light_color);
                        break;
                    }
                    case LightType::DIRECTIONAL:
                    {
                        glLightfv(light_id, GL_DIFFUSE, light_color);
                        glLightfv(light_id, GL_SPECULAR, light_color);
                        float light_direction[] = { current_light.DirectionalLightDirection.X, current_light.DirectionalLightDirection.Y, current_light.DirectionalLightDirection.Z };
                        glLightfv(light_id, GL_SPOT_DIRECTION, light_direction);
                        break;
                    }
                    case LightType::POINT:
                    {
                        glLightfv(light_id, GL_DIFFUSE, light_color);
                        glLightfv(light_id, GL_SPECULAR, light_color);
                        float light_position[] = { current_light.PointLightWorldPosition.X, current_light.PointLightWorldPosition.Y, current_light.PointLightWorldPosition.Z, 1.0f };
                        glLightfv(light_id, GL_POSITION, light_position);
                        break;
                    }
                }
            }
        }
        else
        {
            glDisable(GL_LIGHTING);
        }

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        if (ProjectionType::ORTHOGRAPHIC == camera.Projection)
        {
            /// @todo   Centralize screen dimensions!
            glOrtho(
                camera.WorldPosition.X - 200.0f,
                camera.WorldPosition.X + 200.0f,
                camera.WorldPosition.Y - 200.0f,
                camera.WorldPosition.Y + 200.0f,
                -1.0f,
                1.0f);
        }
        else if (ProjectionType::PERSPECTIVE == camera.Projection)
        {
            const float NEAR_Z_WORLD_BOUNDARY = 1.0f;
            const float FAR_Z_WORLD_BOUNDARY = 500.0f;
#define GLU 1
#if GLU
            const MATH::Angle<float>::Degrees VERTICAL_FIELD_OF_VIEW_IN_DEGREES(90.0f);
            const float ASPECT_RATIO_WIDTH_OVER_HEIGHT = 1.0f;
            gluPerspective(VERTICAL_FIELD_OF_VIEW_IN_DEGREES.Value, ASPECT_RATIO_WIDTH_OVER_HEIGHT, NEAR_Z_WORLD_BOUNDARY, FAR_Z_WORLD_BOUNDARY);
#else
            glFrustum(
                camera.WorldPosition.X - 200.0f,
                camera.WorldPosition.X + 200.0f,
                camera.WorldPosition.Y - 200.0f,
                camera.WorldPosition.Y + 200.0f,
                NEAR_Z_WORLD_BOUNDARY,
                FAR_Z_WORLD_BOUNDARY);
#endif
        }
        /// @todo   Perspective!
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        MATH::Matrix4x4f view_matrix = camera.ViewTransform();
        glLoadMatrixf(view_matrix.Elements.ValuesInColumnMajorOrder().data());

        // RENDER EACH OBJECT IN THE SCENE.
        for (const auto& object_3D : scene.Objects)
        {
            glPushMatrix();
            //glLoadIdentity();
            MATH::Matrix4x4f world_matrix = object_3D.WorldTransform();
            //glLoadMatrixf(world_matrix.Elements.ValuesInColumnMajorOrder().data());
            glMultMatrixf(world_matrix.Elements.ValuesInColumnMajorOrder().data());
            
            Render(object_3D);

            glPopMatrix();
        }
#endif
    }

    /// Clears the screen to the specified color.
    /// @param[in]  color - The color to clear to.
    void OpenGLRenderer::ClearScreen(const Color& color) const
    {
        GLfloat background_color[] = { color.Red, color.Green, color.Blue, color.Alpha };
        const GLint NO_SPECIFIC_DRAW_BUFFER = 0;
        glClearBufferfv(GL_COLOR, NO_SPECIFIC_DRAW_BUFFER, background_color);

        /// @todo   Not sure if this is fully correct.
        GLfloat max_depth[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glClearBufferfv(GL_DEPTH, NO_SPECIFIC_DRAW_BUFFER, max_depth);
    }

    /// Renders the specified object.
    /// @param[in]  object_3D - The 3D object to render.
    /// @param[in]  viewing_transformations - The viewing transformations.
    void OpenGLRenderer::Render(const Object3D& object_3D, const ViewingTransformations& viewing_transformations) const
    {
        // USE THE OBJECT'S SHADER PROGRAM.
        glUseProgram(object_3D.ShaderProgram->Id);

        // SET UNIFORMS.
        GLint world_matrix_variable = glGetUniformLocation(object_3D.ShaderProgram->Id, "world_transform");
        MATH::Matrix4x4f world_transform = object_3D.WorldTransform();
        const float* world_matrix_elements_in_row_major_order = world_transform.ElementsInRowMajorOrder();
        const GLsizei ONE_MATRIX = 1;
        const GLboolean ROW_MAJOR_ORDER = GL_TRUE;
        glUniformMatrix4fv(world_matrix_variable, ONE_MATRIX, ROW_MAJOR_ORDER, world_matrix_elements_in_row_major_order);

        GLint view_matrix_variable = glGetUniformLocation(object_3D.ShaderProgram->Id, "view_transform");
        const float* view_matrix_elements_in_row_major_order = viewing_transformations.CameraViewTransform.ElementsInRowMajorOrder();
        glUniformMatrix4fv(view_matrix_variable, ONE_MATRIX, ROW_MAJOR_ORDER, view_matrix_elements_in_row_major_order);

        GLint projection_matrix_variable = glGetUniformLocation(object_3D.ShaderProgram->Id, "projection_transform");
        const float* projection_matrix_elements_in_row_major_order = viewing_transformations.CameraProjectionTransform.ElementsInRowMajorOrder();
        glUniformMatrix4fv(projection_matrix_variable, ONE_MATRIX, ROW_MAJOR_ORDER, projection_matrix_elements_in_row_major_order);

        GLint texture_sampler_variable = glGetUniformLocation(object_3D.ShaderProgram->Id, "texture_sampler");
        glUniform1i(texture_sampler_variable, 0);

        /// @todo   Pass vertices for entire object at once!
        /// @todo   Look at https://github.com/jpike/OpenGLEngine/ for possible better handling of some stuff?
        for (const auto& triangle : object_3D.Triangles)
        {
            // ALLOCATE A TEXTURE IF APPLICABLE.
            // Must be done outside of glBegin()/glEnd() (http://docs.gl/gl2/glGenTextures).
#if 1
            GLuint texture = 0;
            bool is_textured = (ShadingType::TEXTURED == triangle.Material->Shading);
            if (is_textured)
            {
                //glEnable(GL_TEXTURE_2D);

                glGenTextures(1, &texture);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, texture);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

                glTexImage2D(
                    GL_TEXTURE_2D,
                    0, // level of detail
                    GL_RGBA, // this is the only thing we currently support
                    triangle.Material->Texture->GetWidthInPixels(),
                    triangle.Material->Texture->GetHeightInPixels(),
                    0, // no border
                    GL_RGBA,
                    GL_UNSIGNED_BYTE, // one byte per color component
                    triangle.Material->Texture->GetRawData());
            }
            GLint is_textured_variable = glGetUniformLocation(object_3D.ShaderProgram->Id, "is_textured");
            glUniform1i(is_textured_variable, is_textured);
#endif

            // ALLOCATE A VERTEX ARRAY/BUFFER.
            const GLsizei ONE_VERTEX_ARRAY = 1;
            GLuint vertex_array_id = 0;
            glGenVertexArrays(ONE_VERTEX_ARRAY, &vertex_array_id);
            glBindVertexArray(vertex_array_id);

            const GLsizei ONE_VERTEX_BUFFER = 1;
            GLuint vertex_buffer_id = 0;
            glGenBuffers(ONE_VERTEX_BUFFER, &vertex_buffer_id);

            // FILL THE BUFFER WITH THE VERTEX DATA.
            constexpr std::size_t POSITION_COORDINATE_COUNT_PER_VERTEX = 4;
            constexpr std::size_t VERTEX_POSITION_COORDINATE_TOTAL_COUNT = POSITION_COORDINATE_COUNT_PER_VERTEX * Triangle::VERTEX_COUNT;
            constexpr std::size_t COLOR_COMPONENT_COUNT_PER_VERTEX = 4;
            constexpr std::size_t VERTEX_COLOR_COMPONENT_TOTAL_COUNT = COLOR_COMPONENT_COUNT_PER_VERTEX * Triangle::VERTEX_COUNT;
            constexpr std::size_t TEXTURE_COORDINATE_COMPONENT_COUNT_PER_VERTEX = 2;
            constexpr std::size_t TEXTURE_COORDINATE_COMPONENT_TOTAL_COUNT = TEXTURE_COORDINATE_COMPONENT_COUNT_PER_VERTEX * Triangle::VERTEX_COUNT;
            constexpr std::size_t VERTEX_ATTRIBUTE_TOTAL_VALUE_COUNT = (
                VERTEX_POSITION_COORDINATE_TOTAL_COUNT + 
                VERTEX_COLOR_COMPONENT_TOTAL_COUNT + 
                TEXTURE_COORDINATE_COMPONENT_TOTAL_COUNT);
            
            std::vector<float> vertex_attribute_values;
            vertex_attribute_values.reserve(VERTEX_ATTRIBUTE_TOTAL_VALUE_COUNT);

            for (std::size_t vertex_index = 0; vertex_index < Triangle::VERTEX_COUNT; ++vertex_index)
            {
                const MATH::Vector3f& vertex = triangle.Vertices[vertex_index];
                vertex_attribute_values.emplace_back(vertex.X);
                vertex_attribute_values.emplace_back(vertex.Y);
                vertex_attribute_values.emplace_back(vertex.Z);
                constexpr float HOMOGENEOUS_VERTEX_W = 1.0f;
                vertex_attribute_values.emplace_back(HOMOGENEOUS_VERTEX_W);

                const GRAPHICS::Color& vertex_color = triangle.Material->VertexColors[vertex_index];
                vertex_attribute_values.emplace_back(vertex_color.Red);
                vertex_attribute_values.emplace_back(vertex_color.Green);
                vertex_attribute_values.emplace_back(vertex_color.Blue);
                vertex_attribute_values.emplace_back(vertex_color.Alpha);

                if (!triangle.Material->VertexTextureCoordinates.empty())
                {
                    const MATH::Vector2f& current_vertex_texture_coordinates = triangle.Material->VertexTextureCoordinates[vertex_index];
                    vertex_attribute_values.emplace_back(current_vertex_texture_coordinates.X);
                    vertex_attribute_values.emplace_back(current_vertex_texture_coordinates.Y);
                }
                else
                {
                    vertex_attribute_values.emplace_back(0.0f);
                    vertex_attribute_values.emplace_back(0.0f);
                }
            }

            GLsizeiptr vertex_data_size_in_bytes = sizeof(float) * VERTEX_ATTRIBUTE_TOTAL_VALUE_COUNT;
            glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);
            glBufferData(GL_ARRAY_BUFFER, vertex_data_size_in_bytes, vertex_attribute_values.data(), GL_STATIC_DRAW);

            // SPECIFY HOW VERTEX BUFFER DATA MAPS TO SHADER INPUTS.
            constexpr GLboolean NO_NORMALIZATION = GL_FALSE;
            constexpr GLsizei SINGLE_VERTEX_ATTRIBUTE_VALUE_COUNT = (
                POSITION_COORDINATE_COUNT_PER_VERTEX + 
                COLOR_COMPONENT_COUNT_PER_VERTEX + 
                TEXTURE_COORDINATE_COMPONENT_COUNT_PER_VERTEX);
            constexpr GLsizei SINGLE_VERTEX_ENTIRE_DATA_SIZE_IN_BYTES = sizeof(float) * SINGLE_VERTEX_ATTRIBUTE_VALUE_COUNT;
            constexpr uint64_t VERTEX_POSITION_STARTING_OFFSET_IN_BYTES = 0;
            GLint local_vertex_position_variable_id = glGetAttribLocation(object_3D.ShaderProgram->Id, "local_vertex");
            glVertexAttribPointer(
                local_vertex_position_variable_id,
                POSITION_COORDINATE_COUNT_PER_VERTEX,
                GL_FLOAT,
                NO_NORMALIZATION,
                SINGLE_VERTEX_ENTIRE_DATA_SIZE_IN_BYTES,
                (void*)VERTEX_POSITION_STARTING_OFFSET_IN_BYTES);
            glEnableVertexAttribArray(local_vertex_position_variable_id);

            const uint64_t VERTEX_COLOR_STARTING_OFFSET_IN_BYTES = sizeof(float) * POSITION_COORDINATE_COUNT_PER_VERTEX;
            GLint vertex_color_variable_id = glGetAttribLocation(object_3D.ShaderProgram->Id, "input_vertex_color");
            glVertexAttribPointer(
                vertex_color_variable_id,
                COLOR_COMPONENT_COUNT_PER_VERTEX,
                GL_FLOAT,
                NO_NORMALIZATION,
                SINGLE_VERTEX_ENTIRE_DATA_SIZE_IN_BYTES,
                (void*)VERTEX_COLOR_STARTING_OFFSET_IN_BYTES);
            glEnableVertexAttribArray(vertex_color_variable_id);

#if 1
            constexpr std::size_t VERTEX_COLOR_SIZE_IN_BYTES = sizeof(float) * COLOR_COMPONENT_COUNT_PER_VERTEX;
            const uint64_t TEXTURE_COORDINATE_STARTING_OFFSET_IN_BYTES = VERTEX_COLOR_STARTING_OFFSET_IN_BYTES + VERTEX_COLOR_SIZE_IN_BYTES;
            GLint texture_coordinates_variable_id = glGetAttribLocation(object_3D.ShaderProgram->Id, "input_texture_coordinates");
            glVertexAttribPointer(
                texture_coordinates_variable_id,
                TEXTURE_COORDINATE_COMPONENT_COUNT_PER_VERTEX,
                GL_FLOAT,
                NO_NORMALIZATION,
                SINGLE_VERTEX_ENTIRE_DATA_SIZE_IN_BYTES,
                (void*)TEXTURE_COORDINATE_STARTING_OFFSET_IN_BYTES);
            glEnableVertexAttribArray(texture_coordinates_variable_id);
#endif

            // DRAW THE TRIANGLE.
            const unsigned int FIRST_VERTEX = 0;
            GLsizei vertex_count = static_cast<GLsizei>(Triangle::VERTEX_COUNT);
            glDrawArrays(GL_TRIANGLES, FIRST_VERTEX, vertex_count);

            /// @todo   Encapsulate this vertex array stuff with object?
            glDeleteBuffers(ONE_VERTEX_BUFFER, &vertex_buffer_id);
            glDeleteVertexArrays(ONE_VERTEX_ARRAY, &vertex_array_id);

#if 1
            if (is_textured)
            {
                glDeleteTextures(1, &texture);
                //glDisable(GL_TEXTURE_2D);
            }
#endif
        }
        
#if OLD_OPEN_GL
        for (const auto& triangle : object_3D.Triangles)
        {
            // ALLOCATE A TEXTURE IF APPLICABLE.
            // Must be done outside of glBegin()/glEnd() (http://docs.gl/gl2/glGenTextures).
            GLuint texture = 0;
            bool is_textured = (ShadingType::TEXTURED == triangle.Material->Shading);
            if (is_textured)
            {
                glEnable(GL_TEXTURE_2D);

                glGenTextures(1, &texture);
                glBindTexture(GL_TEXTURE_2D, texture);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

                glTexImage2D(
                    GL_TEXTURE_2D,
                    0, // level of detail
                    GL_RGBA, // this is the only thing we currently support
                    triangle.Material->Texture->GetWidthInPixels(),
                    triangle.Material->Texture->GetHeightInPixels(),
                    0, // no border
                    GL_RGBA,
                    GL_UNSIGNED_BYTE, // one byte per color component
                    triangle.Material->Texture->GetRawData());
            }

            // START RENDERING THE APPROPRIATE TYPE OF PRIMITIVE.
            bool is_wireframe = (ShadingType::WIREFRAME == triangle.Material->Shading);
            if (is_wireframe)
            {
                // RENDER LINES BETWEEN EACH VERTEX.
                glBegin(GL_LINE_LOOP);
            }
            else
            {
                // RENDER NORMAL TRIANGLES.
                glBegin(GL_TRIANGLES);
            }

            // SET THE APPROPRIATE TYPE OF SHADING.
            bool is_flat = (
                ShadingType::WIREFRAME == triangle.Material->Shading ||
                ShadingType::FLAT == triangle.Material->Shading);
            if (is_flat)
            {
                glShadeModel(GL_FLAT);
            }
            else
            {
                glShadeModel(GL_SMOOTH);
            }

            // SET THE SURFACE NORMAL.
            MATH::Vector3f surface_normal = triangle.SurfaceNormal();
            glNormal3f(surface_normal.X, surface_normal.Y, surface_normal.Z);

            // RENDER EACH VERTEX.
            for (std::size_t vertex_index = 0; vertex_index < triangle.Vertices.size(); ++vertex_index)
            {
#if TODO_THIS_CAUSES_TOO_MUCH_INTERFERENCE_HARD_TO_MANAGE_STATE
                // CLEAR ANY PREVIOUSLY SET MATERIAL PROPERTIES TO AVOID INTERFERENCE.
                // They're reset to defaults (http://docs.gl/gl2/glMaterial) to allow other lighting to continue.
                const float NO_MATERIAL_COLOR[] = { 0.0f, 0.0f, 0.0f, 1.0f };
                const float DEFAULT_AMBIENT_MATERIAL_COLOR[] = { 0.2f, 0.2f, 0.2f, 1.0f };
                const float DEFAULT_DIFFUSE_MATERIAL_COLOR[] = { 0.8f, 0.8f, 0.8f, 1.0f };
                glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, DEFAULT_AMBIENT_MATERIAL_COLOR);
                glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, DEFAULT_DIFFUSE_MATERIAL_COLOR);
                /// @todo   Clear shininess too?
                glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, NO_MATERIAL_COLOR);
                glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, NO_MATERIAL_COLOR);
#endif

                // SPECIFY THE VERTEX COLOR.
                switch (triangle.Material->Shading)
                {
                    case ShadingType::WIREFRAME:
                    {
                        glColor3f(
                            triangle.Material->VertexColors[vertex_index].Red,
                            triangle.Material->VertexColors[vertex_index].Green,
                            triangle.Material->VertexColors[vertex_index].Blue);
                        break;
                    }
                    case ShadingType::FLAT:
                    {
                        glColor3f(
                            triangle.Material->VertexColors[vertex_index].Red,
                            triangle.Material->VertexColors[vertex_index].Green,
                            triangle.Material->VertexColors[vertex_index].Blue);
                        break;
                    }
                    case ShadingType::FACE_VERTEX_COLOR_INTERPOLATION:
                    {
                        const Color& vertex_color = triangle.Material->VertexColors[vertex_index];
                        glColor3f(
                            vertex_color.Red,
                            vertex_color.Green,
                            vertex_color.Blue);
                        break;
                    }
                    case ShadingType::GOURAUD:
                    {
                        const Color& vertex_color = triangle.Material->VertexColors[vertex_index];
                        glColor3f(
                            vertex_color.Red,
                            vertex_color.Green,
                            vertex_color.Blue);
                        break;
                    }
                    case ShadingType::TEXTURED:
                    {
                        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
                        const MATH::Vector2f& current_vertex_texture_coordinates = triangle.Material->VertexTextureCoordinates[vertex_index];
                        glTexCoord2f(current_vertex_texture_coordinates.X, current_vertex_texture_coordinates.Y);
                        break;
                    }
                    case ShadingType::MATERIAL:
                    {
#if TODO_THIS_CAUSES_TOO_MUCH_INTERFERENCE_HARD_TO_MANAGE_STATE
                        float ambient_color[] = { triangle.Material->AmbientColor.Red, triangle.Material->AmbientColor.Green, triangle.Material->AmbientColor.Blue, triangle.Material->AmbientColor.Alpha };
                        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient_color);

                        float diffuse_color[] = { triangle.Material->DiffuseColor.Red, triangle.Material->DiffuseColor.Green, triangle.Material->DiffuseColor.Blue, triangle.Material->DiffuseColor.Alpha };
                        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse_color);

                        float specular_color[] = { triangle.Material->SpecularColor.Red, triangle.Material->SpecularColor.Green, triangle.Material->SpecularColor.Blue, triangle.Material->SpecularColor.Alpha };
                        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular_color);

                        glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &triangle.Material->SpecularPower);

                        float emissive_color[] = { triangle.Material->EmissiveColor.Red, triangle.Material->EmissiveColor.Green, triangle.Material->EmissiveColor.Blue, triangle.Material->EmissiveColor.Alpha };
                        glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emissive_color);
#else
                        glColor3f(0.5f, 0.5f, 0.5f);
#endif
                        break;
                    }
                }

                // SPECIFY THE VERTEX POSITION.
                const auto& vertex = triangle.Vertices[vertex_index];
                glVertex3f(vertex.X, vertex.Y, vertex.Z);
            }

            // FINISH RENDERING THE TRIANGLE.
            glEnd();

            if (is_textured)
            {
                glDeleteTextures(1, &texture);
                glDisable(GL_TEXTURE_2D);
            }
        }
#endif
    }
}
