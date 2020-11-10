#include <array>
#include <chrono>
#include <cstdlib>
#include <memory>
#include <optional>
#include <random>
#include <string>
#include <thread>
#include <vector>
#include <Windows.h>
#include "Graphics/Cube.h"
#include "Graphics/Modeling/WavefrontObjectModel.h"
#include "Graphics/Object3D.h"
#include "Graphics/OpenGL/GraphicsDevice.h"
#include "Graphics/OpenGL/OpenGL.h"
#include "Graphics/OpenGL/OpenGLRenderer.h"
#include "Graphics/RayTracing/RayTracingAlgorithm.h"
#include "Graphics/Scene.h"
#include "Graphics/Triangle.h"
#include "Windowing/Win32Window.h"

// ENUMS.
enum class RendererType
{
    SOFTWARE_RASTERIZER = 0,
    SOFTWARE_RAY_TRACER,
    OPEN_GL,
    DIRECT_X
};

// GLOBALS.
// Global to provide access to them within the window procedure.
/// The window for the application.
static std::unique_ptr<WINDOWING::Win32Window> g_window = nullptr;
/// The scene currently being rendered.
static std::size_t g_scene_index = 0;
static GRAPHICS::Scene g_scene;
static GRAPHICS::Camera g_camera;
/// Materials by shading type.
static std::array<std::shared_ptr<GRAPHICS::Material>, static_cast<std::size_t>(GRAPHICS::ShadingType::COUNT)> g_materials_by_shading_type;
/// Current material index into the array above.
static std::size_t g_current_material_index = 0;
/// The type of renderer currently being used.
static RendererType g_current_renderer_type = RendererType::SOFTWARE_RASTERIZER;
/// The software rasterizer.
/// @todo static std::unique_ptr<GRAPHICS::Renderer> g_software_rasterizer = nullptr;
static std::unique_ptr<GRAPHICS::Bitmap> g_software_render_target = nullptr;
/// The ray tracer.
static std::unique_ptr<GRAPHICS::RAY_TRACING::RayTracingAlgorithm> g_ray_tracer = nullptr;
/// The OpenGL renderer.
static std::unique_ptr<GRAPHICS::OPEN_GL::OpenGLRenderer> g_open_gl_renderer = nullptr;
std::shared_ptr<GRAPHICS::OPEN_GL::GraphicsDevice> g_open_gl_graphics_device = nullptr;
/// The available lighting configurations.
static std::vector< std::vector<GRAPHICS::Light> > g_light_configurations =
{
    // Full white ambient light.
    std::vector<GRAPHICS::Light>
    {
        GRAPHICS::Light
        {
            .Type = GRAPHICS::LightType::AMBIENT,
            .Color = GRAPHICS::Color(1.0f, 1.0f, 1.0f, 1.0f),
        },
    },
    // Half strength ambient light.
    std::vector<GRAPHICS::Light>
    {
        GRAPHICS::Light
        {
            .Type = GRAPHICS::LightType::AMBIENT,
            .Color = GRAPHICS::Color(0.5f, 0.5f, 0.5f, 1.0f)
        },
    },
    // Pitch-black ambient lighting.
    std::vector<GRAPHICS::Light>
    {
        GRAPHICS::Light
        {
            .Type = GRAPHICS::LightType::AMBIENT,
            .Color = GRAPHICS::Color(0.0f, 0.0f, 0.0f, 1.0f)
        },
    },
    // Red ambient light.
    std::vector<GRAPHICS::Light>
    {
        GRAPHICS::Light
        {
            .Type = GRAPHICS::LightType::AMBIENT,
            .Color = GRAPHICS::Color(1.0f, 0.0f, 0.0f, 1.0f)
        },
    },
    // Green ambient light.
    std::vector<GRAPHICS::Light>
    {
        GRAPHICS::Light
        {
            .Type = GRAPHICS::LightType::AMBIENT,
            .Color = GRAPHICS::Color(0.0f, 1.0f, 0.0f, 1.0f)
        },
    },
    // Blue ambient light.
    std::vector<GRAPHICS::Light>
    {
        GRAPHICS::Light
        {
            .Type = GRAPHICS::LightType::AMBIENT,
            .Color = GRAPHICS::Color(0.0f, 0.0f, 1.0f, 1.0f)
        },
    },
    // White directional light going left.
    std::vector<GRAPHICS::Light>
    {
        GRAPHICS::Light
        {
            .Type = GRAPHICS::LightType::DIRECTIONAL,
            .Color = GRAPHICS::Color(1.0f, 1.0f, 1.0f, 1.0f),
            .DirectionalLightDirection = MATH::Vector3f(-1.0f, 0.0f, 0.0f)
        },
    },
    // White directional light going right.
    std::vector<GRAPHICS::Light>
    {
        GRAPHICS::Light
        {
            .Type = GRAPHICS::LightType::DIRECTIONAL,
            .Color = GRAPHICS::Color(1.0f, 1.0f, 1.0f, 1.0f),
            .DirectionalLightDirection = MATH::Vector3f(1.0f, 0.0f, 0.0f)
        },
    },
    // White directional light going down.
    std::vector<GRAPHICS::Light>
    {
        GRAPHICS::Light
        {
            .Type = GRAPHICS::LightType::DIRECTIONAL,
            .Color = GRAPHICS::Color(1.0f, 1.0f, 1.0f, 1.0f),
            .DirectionalLightDirection = MATH::Vector3f(0.0f, -1.0f, 0.0f)
        },
    },
    // White directional light going up.
    std::vector<GRAPHICS::Light>
    {
        GRAPHICS::Light
        {
            .Type = GRAPHICS::LightType::DIRECTIONAL,
            .Color = GRAPHICS::Color(1.0f, 1.0f, 1.0f, 1.0f),
            .DirectionalLightDirection = MATH::Vector3f(0.0f, 1.0f, 0.0f)
        },
    },
    // Red directional light at an angle.
    std::vector<GRAPHICS::Light>
    {
        GRAPHICS::Light
        {
            .Type = GRAPHICS::LightType::DIRECTIONAL,
            .Color = GRAPHICS::Color(1.0f, 0.0f, 0.0f, 1.0f),
            .DirectionalLightDirection = MATH::Vector3f::Normalize(MATH::Vector3f(1.0f, 1.0f, 0.0f))
        },
    },
    // Green directional light at an angle.
    std::vector<GRAPHICS::Light>
    {
        GRAPHICS::Light
        {
            .Type = GRAPHICS::LightType::DIRECTIONAL,
            .Color = GRAPHICS::Color(0.0f, 1.0f, 0.0f, 1.0f),
            .DirectionalLightDirection = MATH::Vector3f::Normalize(MATH::Vector3f(0.0f, 1.0f, 1.0f))
        },
    },
    // Blue directional light at an angle.
    std::vector<GRAPHICS::Light>
    {
        GRAPHICS::Light
        {
            .Type = GRAPHICS::LightType::DIRECTIONAL,
            .Color = GRAPHICS::Color(0.0f, 0.0f, 1.0f, 1.0f),
            .DirectionalLightDirection = MATH::Vector3f::Normalize(MATH::Vector3f(1.0f, 0.0f, 1.0f))
        },
    },
    // White point light at center.
    std::vector<GRAPHICS::Light>
    {
        GRAPHICS::Light
        {
            .Type = GRAPHICS::LightType::POINT,
            .Color = GRAPHICS::Color(1.0f, 1.0f, 1.0f, 1.0f),
            .PointLightWorldPosition = MATH::Vector3f(0.0f, 0.0f, 0.0f)
        },
    },
    // Red-green light at left.
    std::vector<GRAPHICS::Light>
    {
        GRAPHICS::Light
        {
            .Type = GRAPHICS::LightType::POINT,
            .Color = GRAPHICS::Color(1.0f, 1.0f, 0.0f, 1.0f),
            .PointLightWorldPosition = MATH::Vector3f(-50.0f, 0.0f, 0.0f)
        },
    },
    // Green-blue light at right.
    std::vector<GRAPHICS::Light>
    {
        GRAPHICS::Light
        {
            .Type = GRAPHICS::LightType::POINT,
            .Color = GRAPHICS::Color(0.0f, 1.0f, 1.0f, 1.0f),
            .PointLightWorldPosition = MATH::Vector3f(50.0f, 0.0f, 0.0f)
        },
    },
    // Blue-red light at top.
    std::vector<GRAPHICS::Light>
    {
        GRAPHICS::Light
        {
            .Type = GRAPHICS::LightType::POINT,
            .Color = GRAPHICS::Color(1.0f, 0.0f, 1.0f, 1.0f),
            .PointLightWorldPosition = MATH::Vector3f(0.0f, 50.0f, 0.0f)
        },
    },
    // Green-blue light at bottom.
    std::vector<GRAPHICS::Light>
    {
        GRAPHICS::Light
        {
            .Type = GRAPHICS::LightType::POINT,
            .Color = GRAPHICS::Color(0.0f, 1.0f, 1.0f, 1.0f),
            .PointLightWorldPosition = MATH::Vector3f(0.0f, -50.0f, 0.0f)
        },
    },
};
/// Current light index into array above.
static std::size_t g_current_light_index = 0;

GRAPHICS::Scene CreateScene(const std::size_t scene_index)
{
    switch (scene_index)
    {
        case 0:
        {
            // BASIC WHITE TRIANGLE.
            OutputDebugString("\nBasic white triangle");
            std::shared_ptr<GRAPHICS::Material> material = std::make_shared<GRAPHICS::Material>();
            material->Shading = GRAPHICS::ShadingType::FLAT;
            material->FaceColor = GRAPHICS::Color(1.0f, 1.0f, 1.0f, 1.0f);
            GRAPHICS::Object3D triangle_object;
            triangle_object.Triangles =
            {
                GRAPHICS::Triangle(
                    material,
                    {
                        MATH::Vector3f(0.0f, 200.0f, 0.0f),
                        MATH::Vector3f(-200.0f, -200.0f, 0.0f),
                        MATH::Vector3f(200.0f, -200.0f, 0.0f)
                    })
            };
            GRAPHICS::Scene scene;
            scene.Objects.push_back(triangle_object);
            return scene;
        }
        case 1:
        {
            // OLDER BASIC TRIANGLE.
            OutputDebugString("\nOld basic triangle");
            const std::shared_ptr<GRAPHICS::Material>& material = g_materials_by_shading_type.at(g_current_material_index);
            GRAPHICS::Triangle triangle = GRAPHICS::Triangle::CreateEquilateral(material);
            GRAPHICS::Object3D larger_triangle;
            larger_triangle.Triangles = { triangle };
            constexpr float LARGER_TRIANGLE_SCALE = 50.0f;
            larger_triangle.Scale = MATH::Vector3f(LARGER_TRIANGLE_SCALE, LARGER_TRIANGLE_SCALE, 1.0f);
            larger_triangle.WorldPosition = MATH::Vector3f(0.0f, 0.0f, 0.0f);
            GRAPHICS::Scene scene;
            scene.Objects.push_back(larger_triangle);
            return scene;
        }
        case 2:
        {
            // MANY SMALL TRIANGLES.
            OutputDebugString("\nMany small triangles");
            constexpr std::size_t SMALL_TRIANGLE_COUNT = 300;
            std::random_device random_number_generator;
            GRAPHICS::Scene scene;
            const std::shared_ptr<GRAPHICS::Material>& material = g_materials_by_shading_type.at(g_current_material_index);
            GRAPHICS::Triangle triangle = GRAPHICS::Triangle::CreateEquilateral(material);
            while (scene.Objects.size() < SMALL_TRIANGLE_COUNT)
            {
                GRAPHICS::Object3D current_object_3D;
                current_object_3D.Triangles = { triangle };
                constexpr float OBJECT_SCALE = 30.0f;
                current_object_3D.Scale = MATH::Vector3f(OBJECT_SCALE, OBJECT_SCALE, OBJECT_SCALE);
                float x_position = static_cast<float>(random_number_generator() % 150) - 75.0f;
                float y_position = static_cast<float>(random_number_generator() % 150) - 75.0f;
                current_object_3D.WorldPosition = MATH::Vector3f(x_position, y_position, -100.0f);
                scene.Objects.push_back(current_object_3D);
            }
            return scene;
        }
        case 3:
        {
            OutputDebugString("\nCube");
            const std::shared_ptr<GRAPHICS::Material>& material = g_materials_by_shading_type.at(g_current_material_index);
            GRAPHICS::Object3D cube = GRAPHICS::Cube::Create(material);
            //cube.Scale = MATH::Vector3f(10.0f, 10.0f, 10.0f);
            cube.WorldPosition = MATH::Vector3f(0.0f, 0.0f, -2.0f);

            GRAPHICS::Scene scene;
            scene.Objects.push_back(cube);
            return scene;
        }
        case 4:
        {
            OutputDebugString("\nCube from file");
            GRAPHICS::Scene scene;
            const std::shared_ptr<GRAPHICS::Material>& material = g_materials_by_shading_type.at(g_current_material_index);
            std::optional<GRAPHICS::Object3D> cube_from_file = GRAPHICS::MODELING::WavefrontObjectModel::Load("../assets/default_cube.obj");
            if (cube_from_file)
            {
                /// @todo   Need to support proper material loading.
#if 1
                for (auto& loaded_triangle : cube_from_file->Triangles)
                {
                    loaded_triangle.Material = material;
                }
#endif
                cube_from_file->WorldPosition = MATH::Vector3f(0.0f, 0.0f, -2.0f);
                scene.Objects.push_back(*cube_from_file);
            }

            return scene;
        }
        default:
            return g_scene;
    }
}

/// The main window callback procedure for processing messages sent to the main application window.
/// @param[in]  window - Handle to the window.
/// @param[in]  message - The message.
/// @param[in]  w_param - Additional information depending on the type of message.
/// @param[in]  l_param - Additional information depending on the type of message.
/// @return     The result of processing the message.
LRESULT CALLBACK MainWindowCallback(
    HWND window,
    UINT message,
    WPARAM w_param,
    LPARAM l_param)
{
    LRESULT messageProcessingResult = 0;

    switch (message)
    {
        case WM_CREATE:
            break;
        case WM_SIZE:
            break;
        case WM_DESTROY:
            break;
        case WM_CLOSE:
        {
            PostQuitMessage(EXIT_SUCCESS);
            break;
        }
        case WM_ACTIVATEAPP:
            break;
        case WM_SYSKEYDOWN:
            break;
        case WM_SYSKEYUP:
            break;
        case WM_KEYUP:
            break;
        case WM_KEYDOWN:
        {
#if 1
            constexpr float CAMERA_MOVEMENT_DISTANCE_PER_KEY_PRESS = 1.0f;
            constexpr float CAMERA_ROTATE_DEGREES_PER_KEY_PRESS = 1.0f;
            int virtual_key_code = static_cast<int>(w_param);
            // https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
            switch (virtual_key_code)
            {
                case VK_UP:
                    g_camera.WorldPosition.Y += CAMERA_MOVEMENT_DISTANCE_PER_KEY_PRESS;
                    break;
                case VK_DOWN:
                    g_camera.WorldPosition.Y -= CAMERA_MOVEMENT_DISTANCE_PER_KEY_PRESS;
                    break;
                case VK_LEFT:
                    g_camera.WorldPosition.X -= CAMERA_MOVEMENT_DISTANCE_PER_KEY_PRESS;
                    break;
                case VK_RIGHT:
                    g_camera.WorldPosition.X += CAMERA_MOVEMENT_DISTANCE_PER_KEY_PRESS;
                    break;
                case 0x57: // W
                    g_camera.WorldPosition.Z -= CAMERA_MOVEMENT_DISTANCE_PER_KEY_PRESS;
                    break;
                case 0x53: // S
                    g_camera.WorldPosition.Z += CAMERA_MOVEMENT_DISTANCE_PER_KEY_PRESS;
                    break;
                case 0x41: // A
                {
                    /// @todo   These rotation angles seem backward...maybe should more specifically work out math?
                    MATH::Angle<float>::Degrees degrees(CAMERA_ROTATE_DEGREES_PER_KEY_PRESS);
                    MATH::Matrix4x4f rotation_matrix = MATH::Matrix4x4f::RotateY(MATH::Angle<float>::DegreesToRadians(degrees));
                    MATH::Vector4f transformed_position = rotation_matrix * MATH::Vector4f::HomogeneousPositionVector(g_camera.WorldPosition);
                    /// @todo   This is a bit of a hack, but it works nicely.  Probably want to simplify camera.
                    g_camera.WorldPosition = MATH::Vector3f(transformed_position.X, transformed_position.Y, transformed_position.Z);
                    g_camera = GRAPHICS::Camera::LookAtFrom(MATH::Vector3f(), g_camera.WorldPosition);
                    break;
                }
                case 0x44: // D
                {
                    /// @todo   These rotation angles seem backward...maybe should more specifically work out math?
                    MATH::Angle<float>::Degrees degrees(-CAMERA_ROTATE_DEGREES_PER_KEY_PRESS);
                    MATH::Matrix4x4f rotation_matrix = MATH::Matrix4x4f::RotateY(MATH::Angle<float>::DegreesToRadians(degrees));
                    MATH::Vector4f transformed_position = rotation_matrix * MATH::Vector4f::HomogeneousPositionVector(g_camera.WorldPosition);
                    /// @todo   This is a bit of a hack, but it works nicely.  Probably want to simplify camera.
                    g_camera.WorldPosition = MATH::Vector3f(transformed_position.X, transformed_position.Y, transformed_position.Z);
                    g_camera = GRAPHICS::Camera::LookAtFrom(MATH::Vector3f(), g_camera.WorldPosition);
                    break;
                }
                case 0x51: // Q
                {
                    MATH::Angle<float>::Degrees degrees(CAMERA_ROTATE_DEGREES_PER_KEY_PRESS);
                    MATH::Matrix4x4f rotation_matrix = MATH::Matrix4x4f::RotateX(MATH::Angle<float>::DegreesToRadians(degrees));
                    MATH::Vector4f transformed_position = rotation_matrix * MATH::Vector4f::HomogeneousPositionVector(g_camera.WorldPosition);
                    /// @todo   This is a bit of a hack, but it works nicely.  Probably want to simplify camera.
                    g_camera.WorldPosition = MATH::Vector3f(transformed_position.X, transformed_position.Y, transformed_position.Z);
                    g_camera = GRAPHICS::Camera::LookAtFrom(MATH::Vector3f(), g_camera.WorldPosition);
                    break;
                }
                case 0x5A: // Z
                {
                    MATH::Angle<float>::Degrees degrees(-CAMERA_ROTATE_DEGREES_PER_KEY_PRESS);
                    MATH::Matrix4x4f rotation_matrix = MATH::Matrix4x4f::RotateX(MATH::Angle<float>::DegreesToRadians(degrees));
                    MATH::Vector4f transformed_position = rotation_matrix * MATH::Vector4f::HomogeneousPositionVector(g_camera.WorldPosition);
                    /// @todo   This is a bit of a hack, but it works nicely.  Probably want to simplify camera.
                    g_camera.WorldPosition = MATH::Vector3f(transformed_position.X, transformed_position.Y, transformed_position.Z);
                    g_camera = GRAPHICS::Camera::LookAtFrom(MATH::Vector3f(), g_camera.WorldPosition);
                    break;
                }
                case 0x30: // 0
                {
                    constexpr std::size_t MAX_SCENE_COUNT = 5;
                    ++g_scene_index;
                    g_scene_index = g_scene_index % MAX_SCENE_COUNT;
                    OutputDebugString(("\nScene index: " + std::to_string(g_scene_index)).c_str());
                    g_scene = CreateScene(g_scene_index);
                    break;
                }
                case 0x50: // P
                {
                    if (GRAPHICS::ProjectionType::ORTHOGRAPHIC == g_camera.Projection)
                    {
                        g_camera.Projection = GRAPHICS::ProjectionType::PERSPECTIVE;
                        OutputDebugString("\nPerspective projection");
                    }
                    else
                    {
                        g_camera.Projection = GRAPHICS::ProjectionType::ORTHOGRAPHIC;
                        OutputDebugString("\nOrthographic projection");
                    }
                    break;
                }
                case 0x4D: // M
                {
                    // SWITCH TO THE NEXT MATERIAL FOR ALL OBJECTS.
                    ++g_current_material_index;
                    g_current_material_index = g_current_material_index % static_cast<std::size_t>(GRAPHICS::ShadingType::COUNT);

                    OutputDebugString(("\nMaterial index: " + std::to_string(g_current_material_index)).c_str());

                    const std::shared_ptr<GRAPHICS::Material>& current_material = g_materials_by_shading_type.at(g_current_material_index);
                    for (auto& object_3D : g_scene.Objects)
                    {
                        for (auto& triangle : object_3D.Triangles)
                        {
                            triangle.Material = current_material;
                        }
                    }
                    break;
                }
                case 0x4C: // L
                {
                    // SWITCH TO THE NEXT LIGHTING CONFIGURATION.
                    ++g_current_light_index;
                    g_current_light_index = g_current_light_index % g_light_configurations.size();
                    g_scene.PointLights = g_light_configurations[g_current_light_index];

                    std::string light_index_string = "\nLight index: " + std::to_string(g_current_light_index);
                    OutputDebugString(light_index_string.c_str());
                    break;
                }

                /// @todo
                case 0x31: // 1
                {
                    bool software_rasterizer_already_being_used = (RendererType::SOFTWARE_RASTERIZER == g_current_renderer_type);
                    if (!software_rasterizer_already_being_used)
                    {
#if TODO
                        if (!g_software_rasterizer)
                        {
                            g_software_rasterizer = std::make_unique<GRAPHICS::Renderer>();
                        }
#endif

                        if (!g_software_render_target)
                        {
                            /// @todo   Centralize screen dimensions.
                            g_software_render_target = std::make_unique<GRAPHICS::Bitmap>(400, 400, GRAPHICS::ColorFormat::ARGB);
                        }

                        //g_camera = GRAPHICS::Camera::LookAtFrom(MATH::Vector3f(0.0f, 0.0f, 0.0f), MATH::Vector3f(0.0f, 0.0f, 2.0f));

                        g_current_renderer_type = RendererType::SOFTWARE_RASTERIZER;
                        OutputDebugString("\nSoftware rasterizer");
                    }
                    break;
                }
                case 0x32: // 2
                {
                    bool ray_tracer_already_being_used = (RendererType::SOFTWARE_RAY_TRACER == g_current_renderer_type);
                    if (!ray_tracer_already_being_used)
                    {
                        if (!g_ray_tracer)
                        {
                            g_ray_tracer = std::make_unique<GRAPHICS::RAY_TRACING::RayTracingAlgorithm>();
                        }

                        //g_camera = GRAPHICS::Camera::LookAtFrom(MATH::Vector3f(0.0f, 0.0f, 0.0f), MATH::Vector3f(0.0f, 0.0f, 2.0f));

                        g_current_renderer_type = RendererType::SOFTWARE_RAY_TRACER;
                        OutputDebugString("\nSoftware ray tracer");
                    }
                    break;
                }
                case 0x33: // 3
                {
                    bool open_gl_already_being_used = (RendererType::OPEN_GL == g_current_renderer_type);
                    if (!open_gl_already_being_used)
                    {
                        if (!g_open_gl_renderer)
                        {
                            g_open_gl_renderer = std::make_unique<GRAPHICS::OPEN_GL::OpenGLRenderer>();

                            // GET THE DEVICE CONTEXT OF THE WINDOW.
                            HDC device_context = GetDC(g_window->WindowHandle);
                            bool device_context_retrieved = (NULL != device_context);
                            if (!device_context_retrieved)
                            {
                                OutputDebugString("Failed to get window device context.");
                                break;
                            }

                            // INITIALIZE OPEN GL.
                            bool open_gl_initialized = GRAPHICS::OPEN_GL::Initialize(device_context);
                            if (!open_gl_initialized)
                            {
                                OutputDebugString("Failed to initialize OpenGL.");
                                break;
                            }

                            // CREATE THE GRAPHICS DEVICE.
                            g_open_gl_graphics_device = GRAPHICS::OPEN_GL::GraphicsDevice::Create(device_context);
                            bool graphics_device_created = (nullptr != g_open_gl_graphics_device);
                            if (!graphics_device_created)
                            {
                                OutputDebugString("Failed to create the graphics device.");
                                break;
                            }
                        }

                        /// @todo   Where to put these?
                        /// @todo   Centralize screen dimensions!
                        glViewport(0, 0, 400, 400);

                        g_current_renderer_type = RendererType::OPEN_GL;
                        OutputDebugString("\nOpenGL");
                    }
                    break;
                }
                default:
                    virtual_key_code;
                    break;
            }
            std::string camera_position = "\nCamera Position: ";
            camera_position += std::to_string(g_camera.WorldPosition.X) + ",";
            camera_position += std::to_string(g_camera.WorldPosition.Y) + ",";
            camera_position += std::to_string(g_camera.WorldPosition.Z) + "\n";
            OutputDebugString(camera_position.c_str());
#endif

            break;
        }
        /// @todo case WM_SETCURSOR:
        /// @todo break;
        case WM_PAINT:
        {
            PAINTSTRUCT paint;
            /// @todo HDC device_context = BeginPaint(window, &paint);
            BeginPaint(window, &paint);

            /// @todo   Copy rendered stuff to window here!

            EndPaint(window, &paint);
            break;
        }
        default:
            messageProcessingResult = DefWindowProcA(window, message, w_param, l_param);
            break;
}

    return messageProcessingResult;
}

/// The entry point to the application.
/// @param[in]  application_instance - A handle to the current instance of the application.
/// @param[in]  previous_application_instance - Always NULL.
/// @param[in]  command_line_string - The command line parameters for the application.
/// @param[in]  window_show_code - Controls how the window is to be shown.
/// @return     An exit code.  0 for success.
int CALLBACK WinMain(
    HINSTANCE application_instance,
    HINSTANCE previous_application_instance,
    LPSTR command_line_string,
    int window_show_code)
{
    // REFERENCE UNUSED PARAMETERS TO PREVENT COMPILER WARNINGS.
    previous_application_instance;
    command_line_string;
    window_show_code;

    // DEFINE PARAMETERS FOR THE WINDOW TO BE CREATED.
    // The structure is zeroed-out initially since it isn't necessary to set all fields.
    WNDCLASSEX window_class = {};

    window_class.cbSize = sizeof(window_class);

    // Ensure that the window gets re-drawn when the client area is moved or re-sized.
    // A unique device context is allocated for each window in the class which can
    // potentially provide performance improvements by Windows not having to re-obtain
    // or re-allocate a device context.
    window_class.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;

    window_class.lpfnWndProc = MainWindowCallback;
    window_class.hInstance = application_instance;
    window_class.lpszClassName = "WindowClass";

    // CREATE THE WINDOW.
    constexpr unsigned int SCREEN_WIDTH_IN_PIXELS = 400;
    constexpr unsigned int SCREEN_HEIGHT_IN_PIXELS = 400;
    g_window = WINDOWING::Win32Window::Create(
        window_class,
        "Window Title",
        static_cast<int>(SCREEN_WIDTH_IN_PIXELS),
        static_cast<int>(SCREEN_HEIGHT_IN_PIXELS));
    bool window_created = (nullptr != g_window);
    if (!window_created)
    {
        OutputDebugString("Failed to create window.");
        return EXIT_FAILURE;
    }

    // INITIALIZE THE DEFAULT RENDERER.
#if TODO
    if (!g_software_rasterizer)
    {
        g_software_rasterizer = std::make_unique<GRAPHICS::Renderer>();
    }
#endif
    if (!g_software_render_target)
    {
        /// @todo   Centralize screen dimensions.
        g_software_render_target = std::make_unique<GRAPHICS::Bitmap>(400, 400, GRAPHICS::ColorFormat::ARGB);
    }
    
    g_current_renderer_type = RendererType::SOFTWARE_RASTERIZER;

#if OPEN_GL_DEFAULT
    // Using OpenGL as default now for testing.
    bool open_gl_already_being_used = (RendererType::OPEN_GL == g_current_renderer_type);
    if (!open_gl_already_being_used)
    {
        if (!g_open_gl_renderer)
        {
            g_open_gl_renderer = std::make_unique<GRAPHICS::OPEN_GL::OpenGLRenderer>();

            // GET THE DEVICE CONTEXT OF THE WINDOW.
            HDC device_context = GetDC(g_window->WindowHandle);
            bool device_context_retrieved = (NULL != device_context);
            if (!device_context_retrieved)
            {
                OutputDebugString("Failed to get window device context.");
                return EXIT_FAILURE;
            }

            // INITIALIZE OPEN GL.
            bool open_gl_initialized = GRAPHICS::OPEN_GL::Initialize(device_context);
            if (!open_gl_initialized)
            {
                OutputDebugString("Failed to initialize OpenGL.");
                return EXIT_FAILURE;
            }

            // CREATE THE GRAPHICS DEVICE.
            g_open_gl_graphics_device = GRAPHICS::OPEN_GL::GraphicsDevice::Create(device_context);
            bool graphics_device_created = (nullptr != g_open_gl_graphics_device);
            if (!graphics_device_created)
            {
                OutputDebugString("Failed to create the graphics device.");
                return EXIT_FAILURE;
            }
        }

        /// @todo   Where to put these?
        /// @todo   Centralize screen dimensions!
        glViewport(0, 0, 400, 400);

        g_current_renderer_type = RendererType::OPEN_GL;
    }
#endif

    g_camera = GRAPHICS::Camera::LookAtFrom(MATH::Vector3f(0.0f, 0.0f, 0.0f), MATH::Vector3f(0.0f, 0.0f, 2.0f));
    g_camera.Projection = GRAPHICS::ProjectionType::PERSPECTIVE;

    // LOAD A TEXTURE FOR TESTING.
    std::shared_ptr<GRAPHICS::Bitmap> texture = GRAPHICS::Bitmap::Load("../assets/test_texture1.bmp");
    if (!texture)
    {
        OutputDebugString("Failed to load test texture.");
        return EXIT_FAILURE;
    }

    // DEFINE A VARIETY OF MATERIALS.
    // These can't be initialized statically since some of the color constants are also static,
    // and initialization order isn't clearly defined.
    g_materials_by_shading_type =
    {
        std::make_shared<GRAPHICS::Material>(GRAPHICS::Material
        {
            .Shading = GRAPHICS::ShadingType::WIREFRAME,
            .WireframeColor = GRAPHICS::Color::GREEN
        }),
        std::make_shared<GRAPHICS::Material>(GRAPHICS::Material
        {
            .Shading = GRAPHICS::ShadingType::WIREFRAME_VERTEX_COLOR_INTERPOLATION,
            .VertexWireframeColors =
            {
                GRAPHICS::Color::RED,
                GRAPHICS::Color::GREEN,
                GRAPHICS::Color::BLUE,
            }
        }),
        std::make_shared<GRAPHICS::Material>(GRAPHICS::Material
        {
            .Shading = GRAPHICS::ShadingType::FLAT,
            .FaceColor = GRAPHICS::Color::BLUE
        }),
        std::make_shared<GRAPHICS::Material>(GRAPHICS::Material
        {
            .Shading = GRAPHICS::ShadingType::FACE_VERTEX_COLOR_INTERPOLATION,
            .VertexFaceColors =
            {
                GRAPHICS::Color(1.0f, 0.0f, 0.0f, 1.0f),
                GRAPHICS::Color(0.0f, 1.0f, 0.0f, 1.0f),
                GRAPHICS::Color(0.0f, 0.0f, 1.0f, 1.0f),
            }
        }),
        std::make_shared<GRAPHICS::Material>(GRAPHICS::Material
        {
            .Shading = GRAPHICS::ShadingType::GOURAUD,
            .VertexColors =
            {
                // Basic grayscale.
                GRAPHICS::Color(0.5f, 0.5f, 0.5f, 1.0f),
                GRAPHICS::Color(0.5f, 0.5f, 0.5f, 1.0f),
                GRAPHICS::Color(0.5f, 0.5f, 0.5f, 1.0f),
            },
            .SpecularPower = 20.0f
        }),
        std::make_shared<GRAPHICS::Material>(GRAPHICS::Material
        {
            .Shading = GRAPHICS::ShadingType::TEXTURED,
            .VertexColors =
            {
                GRAPHICS::Color(1.0f, 1.0f, 1.0f, 1.0f),
                GRAPHICS::Color(1.0f, 1.0f, 1.0f, 1.0f),
                GRAPHICS::Color(1.0f, 1.0f, 1.0f, 1.0f),
            },
            .Texture = texture,
            .VertexTextureCoordinates =
            {
                MATH::Vector2f(0.0f, 0.0f),
                MATH::Vector2f(1.0f, 0.0f),
                MATH::Vector2f(0.0f, 1.0f)
            }
        }),
        std::make_shared<GRAPHICS::Material>(GRAPHICS::Material
        {
            /// @todo   Make this get values directly from the "material".
            .Shading = GRAPHICS::ShadingType::MATERIAL,
            .AmbientColor = GRAPHICS::Color(0.2f, 0.2f, 0.2f, 1.0f),
            .DiffuseColor = GRAPHICS::Color(0.8f, 0.8f, 0.8f, 1.0f),
        })
    };

    // CREATE EXAMPLE OBJECTS.
    g_scene_index = 3;
    g_current_material_index = 3;
    g_current_light_index = 0;
    g_scene = CreateScene(g_scene_index);

    // RUN A MESSAGE LOOP.
    float object_rotation_angle_in_radians = 0.0f;
    constexpr float TARGET_FRAMES_PER_SECOND = 60.0f;
    constexpr std::chrono::duration<float, std::chrono::seconds::period> TARGET_SECONDS_PER_FRAME(1.0f / TARGET_FRAMES_PER_SECOND);
    auto start_time = std::chrono::high_resolution_clock::now();
    bool running = true;
    while (running)
    {
        auto frame_start_time = std::chrono::high_resolution_clock::now();

        // PROCESS ANY MESSAGES FOR THE APPLICATION WINDOW.
        MSG message;
        auto message_received = [&]()
        {
            const HWND ALL_MESSAGES_FOR_CURRENT_THREAD = NULL;
            const UINT NO_MIN_MESSAGE_RESTRICTION = 0;
            const UINT NO_MAX_MESSAGE_RESTRICTION = 0;

            return PeekMessageA(
                &message,
                ALL_MESSAGES_FOR_CURRENT_THREAD,
                NO_MIN_MESSAGE_RESTRICTION,
                NO_MAX_MESSAGE_RESTRICTION,
                PM_REMOVE);
        };
        while (message_received())
        {
            // STOP RUNNING THE APPLICATION IF THE USER DECIDED TO QUIT.
            if (message.message == WM_QUIT)
            {
                running = false;
                break;
            }

            // TRANSLATE VIRTUAL KEY MESSAGES TO CHARACTER MESSAGES.
            // The return value is not checked since failure is not problematic
            // and the only thing that could be done is logging.
            TranslateMessage(&message);

            // DISPATCH THE MESSAGE TO THE WINDOW PROCEDURE.
            // The return value is simply the return value from the window procedure.
            // Nothing value could be done with it besides logging, so it is ignored.
            DispatchMessage(&message);
        }

        // ROTATE ANY OBJECT IN THE SCENE.
        auto current_time = std::chrono::high_resolution_clock::now();
        auto total_elapsed_time = std::chrono::duration_cast<std::chrono::duration<float>>(current_time - start_time).count();
        object_rotation_angle_in_radians = 0.5f * total_elapsed_time;
        for (auto& object_3D : g_scene.Objects)
        {
            object_3D;
#define ROTATE_OBJECTS 1
#if ROTATE_OBJECTS
            object_3D.RotationInRadians.X = MATH::Angle<float>::Radians(object_rotation_angle_in_radians);
            object_3D.RotationInRadians.Y = MATH::Angle<float>::Radians(object_rotation_angle_in_radians);
            object_3D.RotationInRadians.Z = MATH::Angle<float>::Radians(object_rotation_angle_in_radians);
#endif
        }

        // RENDER THE SCENE BASED ON THE CURRENT RENDERER.
        switch (g_current_renderer_type)
        {
            case RendererType::SOFTWARE_RASTERIZER:
            {
                // RENDER THE SCENE.
                //g_scene.BackgroundColor = GRAPHICS::Color(1.0f, 0.0f, 0.0f, 1.0f);
#if TODO
                g_software_rasterizer->Render(g_scene, g_camera, *g_software_render_target);
#endif

                // DISPLAY THE RENDERED OBJECTS IN THE WINDOW.
                g_window->Display(*g_software_render_target);

                break;
            }
            case RendererType::SOFTWARE_RAY_TRACER:
            {
                //g_scene.BackgroundColor = GRAPHICS::Color(0.0f, 1.0f, 0.0f, 1.0f);
                g_camera.ViewingPlane.Width = static_cast<float>(SCREEN_WIDTH_IN_PIXELS);
                g_camera.ViewingPlane.Height = static_cast<float>(SCREEN_HEIGHT_IN_PIXELS);
                g_ray_tracer->Render(g_scene, g_camera, *g_software_render_target);

                // DISPLAY THE RENDERED OBJECTS IN THE WINDOW.
                g_window->Display(*g_software_render_target);

                break;
            }
            case RendererType::OPEN_GL:
            {
                //g_scene.BackgroundColor = GRAPHICS::Color(0.0f, 0.0f, 1.0f, 1.0f);
                g_open_gl_renderer->Render(g_scene, g_camera);

                glFlush();

                GLenum error = glGetError();
                if (error != GL_NO_ERROR)
                {
                    error = error;
                }

                SwapBuffers(g_open_gl_graphics_device->DeviceContext);

                break;
            }
        }

#define DISPLAY_FRAME_TIMES 0
#if DISPLAY_FRAME_TIMES
        // DISPLAY STATISICS ABOUT FRAME TIMING.
        auto frame_end_time = std::chrono::high_resolution_clock::now();
        auto frame_elapsed_time = frame_end_time - frame_start_time;

        auto frame_elapsed_time_milliseconds = std::chrono::duration_cast<std::chrono::duration<float, std::chrono::milliseconds::period>>(frame_elapsed_time);
        std::string frame_elapsed_time_milliseconds_string = std::to_string(frame_elapsed_time_milliseconds.count()) + " ms\t";
        OutputDebugStringA(frame_elapsed_time_milliseconds_string.c_str());

        auto frame_elapsed_time_seconds = std::chrono::duration_cast<std::chrono::duration<float, std::chrono::seconds::period>>(frame_elapsed_time);
        std::string frame_elapsed_time_seconds_string = std::to_string(frame_elapsed_time_seconds.count()) + " s\t";
        OutputDebugStringA(frame_elapsed_time_seconds_string.c_str());

        float frames_per_second = 1.0f / frame_elapsed_time_seconds.count();
        std::string frames_per_second_string = std::to_string(frames_per_second) + " fps\n";
        OutputDebugStringA(frames_per_second_string.c_str());

        /// @todo   Add some buffer time to keep frame rate from dipping too low?
        bool frame_finished_early = frame_elapsed_time_seconds < TARGET_SECONDS_PER_FRAME;
        if (frame_finished_early)
        {
            auto remaining_time_for_frame = TARGET_SECONDS_PER_FRAME - frame_elapsed_time_seconds;
            std::this_thread::sleep_for(remaining_time_for_frame);
        }
#endif
    }

    return EXIT_SUCCESS;
}
