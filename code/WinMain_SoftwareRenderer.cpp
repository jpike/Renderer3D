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
#include "Graphics/Camera.h"
#include "Graphics/Cube.h"
#include "Graphics/DepthBuffer.h"
#include "Graphics/FrameTimer.h"
#include "Graphics/Gui/Font.h"
#include "Graphics/Gui/Text.h"
#include "Graphics/Modeling/WavefrontObjectModel.h"
#include "Graphics/Object3D.h"
#include "Graphics/Scene.h"
#include "Graphics/SoftwareRasterizationAlgorithm.h"
#include "Graphics/Triangle.h"
#include "InputControl/Key.h"
#include "Windowing/Win32Window.h"

// GLOBALS.
// Global to provide access to them within the window procedure.
/// The window for the application.
static std::unique_ptr<WINDOWING::Win32Window> g_window = nullptr;

static GRAPHICS::Camera g_camera;

static bool g_depth_buffer_enabled = false;

static std::size_t g_scene_index = 0;
static std::string g_scene_title;
static GRAPHICS::Scene g_scene;

static std::vector<std::shared_ptr<GRAPHICS::Material>> g_materials;
static std::size_t g_current_material_index = 0;

static std::vector<std::string> g_material_names =
{
    "Wireframe green",
    "Wireframe RGB",
    "Flat blue",
    "Flat RGB",
    "Gouraud gray",
    "Textured white",
    "Material (ambient, diffuse)",
    "Material (ambient, diffuse, specular)",
};

static std::vector<std::string> g_light_configuration_names =
{
    "No lighting",
    "Single white ambient light",
    "Single gray ambient light",
    "Pitch black ambient light",
    "Red ambient light",
    "Green ambient light",
    "Blue ambient light",
    "White directional light going left",
    "White directional light going right",
    "White directional light going down",
    "White directional light going up",
    "Red directional light at angle",
    "Green directional light at angle",
    "Blue directional light at angle",
    "White point light at center",
    "Red-green point light at left",
    "Green-blue point light at right",
    "Blue-red point light at top",
    "Green-blue point light at bottom",
};

static std::vector< std::optional<std::vector<GRAPHICS::Light>> > g_light_configurations =
{
    // No lights,
    std::nullopt,

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
static std::size_t g_current_light_index = 0;

static MATH::Vector3<bool> g_rotation_enabled;
static bool g_backface_culling = false;

GRAPHICS::Scene CreateScene(const std::size_t scene_index)
{
    switch (scene_index)
    {
        case 0:
        {
            // BASIC WHITE TRIANGLE.
            g_scene_title = "Basic white triangle";
            std::shared_ptr<GRAPHICS::Material>& material = g_materials.at(g_current_material_index);
            GRAPHICS::Object3D triangle_object;
            triangle_object.Triangles =
            {
                GRAPHICS::Triangle(
                    material,
                    {
                        MATH::Vector3f(0.0f, 1.0f, 0.0f),
                        MATH::Vector3f(-1.0f, -1.0f, 0.0f),
                        MATH::Vector3f(1.0f, -1.0f, 0.0f)
                    })
            };
            GRAPHICS::Scene scene;
            scene.Objects.push_back(triangle_object);
            return scene;
        }
        case 1:
        {
            // OLDER BASIC TRIANGLE.
            g_scene_title = "Old basic triangle";
            const std::shared_ptr<GRAPHICS::Material>& material = g_materials.at(g_current_material_index);
            GRAPHICS::Triangle triangle = GRAPHICS::Triangle::CreateEquilateral(material);
            GRAPHICS::Object3D larger_triangle;
            larger_triangle.Triangles = { triangle };
            //constexpr float LARGER_TRIANGLE_SCALE = 50.0f;
            //larger_triangle.Scale = MATH::Vector3f(LARGER_TRIANGLE_SCALE, LARGER_TRIANGLE_SCALE, 1.0f);
            larger_triangle.WorldPosition = MATH::Vector3f(0.0f, 0.0f, 0.0f);
            GRAPHICS::Scene scene;
            scene.Objects.push_back(larger_triangle);
            return scene;
        }
        case 2:
        {
            // MANY SMALL TRIANGLES.
            g_scene_title = "Many small triangles";
            constexpr std::size_t SMALL_TRIANGLE_COUNT = 50;
            std::random_device random_number_generator;
            GRAPHICS::Scene scene;
            const std::shared_ptr<GRAPHICS::Material>& material = g_materials.at(g_current_material_index);
            GRAPHICS::Triangle triangle = GRAPHICS::Triangle::CreateEquilateral(material);
            while (scene.Objects.size() < SMALL_TRIANGLE_COUNT)
            {
                GRAPHICS::Object3D current_object_3D;
                current_object_3D.Triangles = { triangle };
                //constexpr float OBJECT_SCALE = 30.0f;
                //current_object_3D.Scale = MATH::Vector3f(OBJECT_SCALE, OBJECT_SCALE, OBJECT_SCALE);
                float x_position = static_cast<float>(random_number_generator() % 16) - 8.0f;
                float y_position = static_cast<float>(random_number_generator() % 16) - 8.0f;
                current_object_3D.WorldPosition = MATH::Vector3f(x_position, y_position, -8.0f);
                scene.Objects.push_back(current_object_3D);
            }
            return scene;
        }
        case 3:
        {
            g_scene_title = "Cube";
            const std::shared_ptr<GRAPHICS::Material>& material = g_materials.at(g_current_material_index);
            GRAPHICS::Object3D cube = GRAPHICS::Cube::Create(material);
            //cube.Scale = MATH::Vector3f(10.0f, 10.0f, 10.0f);
            cube.WorldPosition = MATH::Vector3f(0.0f, 0.0f, -2.0f);

            GRAPHICS::Scene scene;
            scene.Objects.push_back(cube);
            return scene;
        }
        case 4:
        {
            g_scene_title = "Cube from file";
            GRAPHICS::Scene scene;
            const std::shared_ptr<GRAPHICS::Material>& material = g_materials.at(g_current_material_index);
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
            /// @todo   GetKeyState vs GetAsyncKeyState()?
            bool shift_down = GetAsyncKeyState(VK_SHIFT) & 0x8000;

            constexpr float CAMERA_MOVEMENT_DISTANCE_PER_KEY_PRESS = 1.0f;
            constexpr float CAMERA_ROTATE_DEGREES_PER_KEY_PRESS = 1.0f;
            int virtual_key_code = static_cast<int>(w_param);
            // https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
            switch (virtual_key_code)
            {
                case INPUT_CONTROL::Key::UP_ARROW:
                    g_camera.WorldPosition.Y += CAMERA_MOVEMENT_DISTANCE_PER_KEY_PRESS;
                    break;
                case INPUT_CONTROL::Key::DOWN_ARROW:
                    g_camera.WorldPosition.Y -= CAMERA_MOVEMENT_DISTANCE_PER_KEY_PRESS;
                    break;
                case INPUT_CONTROL::Key::LEFT_ARROW:
                    g_camera.WorldPosition.X -= CAMERA_MOVEMENT_DISTANCE_PER_KEY_PRESS;
                    break;
                case INPUT_CONTROL::Key::RIGHT_ARROW:
                    g_camera.WorldPosition.X += CAMERA_MOVEMENT_DISTANCE_PER_KEY_PRESS;
                    break;
                case INPUT_CONTROL::Key::D: // D (depth)
                {
                    if (shift_down)
                    {
                        g_camera.WorldPosition.Z -= CAMERA_MOVEMENT_DISTANCE_PER_KEY_PRESS;
                    }
                    else
                    {
                        g_camera.WorldPosition.Z += CAMERA_MOVEMENT_DISTANCE_PER_KEY_PRESS;
                    }
                    break;
                }
                case INPUT_CONTROL::Key::N:
                {
                    if (shift_down)
                    {
                        g_camera.NearClipPlaneViewDistance += CAMERA_MOVEMENT_DISTANCE_PER_KEY_PRESS;
                    }
                    else
                    {
                        g_camera.NearClipPlaneViewDistance -= CAMERA_MOVEMENT_DISTANCE_PER_KEY_PRESS;
                    }
                    break;
                }
                case INPUT_CONTROL::Key::F:
                {
                    if (shift_down)
                    {
                        g_camera.FarClipPlaneViewDistance += CAMERA_MOVEMENT_DISTANCE_PER_KEY_PRESS;
                    }
                    else
                    {
                        g_camera.FarClipPlaneViewDistance -= CAMERA_MOVEMENT_DISTANCE_PER_KEY_PRESS;
                    }
                    break;
                }
                case INPUT_CONTROL::Key::V:
                {
                    if (shift_down)
                    {
                        g_camera.FieldOfView.Value += CAMERA_ROTATE_DEGREES_PER_KEY_PRESS;
                    }
                    else
                    {
                        g_camera.FieldOfView.Value -= CAMERA_ROTATE_DEGREES_PER_KEY_PRESS;
                    }
                    break;
                }
                case INPUT_CONTROL::Key::X:
                {
                    g_rotation_enabled.X = !g_rotation_enabled.X;
                    break;
                }
                case INPUT_CONTROL::Key::Y: // Y
                {
                    g_rotation_enabled.Y = !g_rotation_enabled.Y;
                    break;
                }
                case INPUT_CONTROL::Key::Z:
                {
                    g_rotation_enabled.Z = !g_rotation_enabled.Z;
                    break;
                }
                case INPUT_CONTROL::Key::B:
                {
                    g_backface_culling = !g_backface_culling;
                    break;
                };
                case INPUT_CONTROL::Key::W: // Not sure what other key to use for this.
                {
                    g_depth_buffer_enabled = !g_depth_buffer_enabled;
                    break;
                }
                case INPUT_CONTROL::Key::S:
                {
                    constexpr std::size_t MAX_SCENE_COUNT = 5;
                    ++g_scene_index;
                    g_scene_index = g_scene_index % MAX_SCENE_COUNT;
                    g_scene = CreateScene(g_scene_index);
                    break;
                };
                case INPUT_CONTROL::Key::M:
                {
                    // SWITCH TO THE NEXT MATERIAL FOR ALL OBJECTS.
                    ++g_current_material_index;
                    g_current_material_index = g_current_material_index % g_materials.size();

                    const std::shared_ptr<GRAPHICS::Material>& current_material = g_materials.at(g_current_material_index);
                    for (auto& object_3D : g_scene.Objects)
                    {
                        for (auto& triangle : object_3D.Triangles)
                        {
                            triangle.Material = current_material;
                        }
                    }
                    break;
                };
                case INPUT_CONTROL::Key::L:
                {
                    // SWITCH TO THE NEXT LIGHTING CONFIGURATION.
                    ++g_current_light_index;
                    g_current_light_index = g_current_light_index % g_light_configurations.size();
                    g_scene.PointLights = g_light_configurations[g_current_light_index];
                    break;
                };
                default:
                    virtual_key_code;
                    break;
            }
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
    constexpr unsigned int SCREEN_WIDTH_IN_PIXELS = 800;
    constexpr unsigned int SCREEN_HEIGHT_IN_PIXELS = 600;
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

    // LOAD THE DEFAULT FONT.
    std::shared_ptr<GRAPHICS::GUI::Font> font = GRAPHICS::GUI::Font::LoadSystemDefaultFixedFont();
    if (!font)
    {
        OutputDebugString("Failed to load default font.");
        return EXIT_FAILURE;
    }

    GRAPHICS::Bitmap perspective_projected_drawing(SCREEN_WIDTH_IN_PIXELS / 2, 400, GRAPHICS::ColorFormat::ARGB);
    GRAPHICS::Bitmap orthographic_projected_drawing(SCREEN_WIDTH_IN_PIXELS / 2, 400, GRAPHICS::ColorFormat::ARGB);
    GRAPHICS::Bitmap debug_text_drawing(SCREEN_WIDTH_IN_PIXELS, 200, GRAPHICS::ColorFormat::ARGB);

    GRAPHICS::DepthBuffer perspective_depth_buffer(perspective_projected_drawing.GetWidthInPixels(), perspective_projected_drawing.GetHeightInPixels());
    GRAPHICS::DepthBuffer orthographic_depth_buffer(orthographic_projected_drawing.GetWidthInPixels(), orthographic_projected_drawing.GetWidthInPixels());

    g_camera = GRAPHICS::Camera::LookAtFrom(MATH::Vector3f(0.0f, 0.0f, 0.0f), MATH::Vector3f(0.0f, 0.0f, 2.0f));
    g_camera.NearClipPlaneViewDistance = 1.0f;
    g_camera.FarClipPlaneViewDistance = 100.0f;

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
    g_materials =
    {
        std::make_shared<GRAPHICS::Material>(GRAPHICS::Material
        {
            .Shading = GRAPHICS::ShadingType::WIREFRAME,
            .VertexColors = { GRAPHICS::Color::GREEN, GRAPHICS::Color::GREEN, GRAPHICS::Color::GREEN }
        }),
        std::make_shared<GRAPHICS::Material>(GRAPHICS::Material
        {
            .Shading = GRAPHICS::ShadingType::WIREFRAME,
            .VertexColors =
            {
                GRAPHICS::Color::RED,
                GRAPHICS::Color::GREEN,
                GRAPHICS::Color::BLUE,
            }
        }),
        std::make_shared<GRAPHICS::Material>(GRAPHICS::Material
        {
            .Shading = GRAPHICS::ShadingType::FLAT,
            .VertexColors = { GRAPHICS::Color::BLUE, GRAPHICS::Color::BLUE, GRAPHICS::Color::BLUE }
        }),
        std::make_shared<GRAPHICS::Material>(GRAPHICS::Material
        {
            .Shading = GRAPHICS::ShadingType::FACE_VERTEX_COLOR_INTERPOLATION,
            .VertexColors =
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
            .VertexColors = 
            {
                GRAPHICS::Color(0.5f, 0.5f, 0.5f, 1.0f),
                GRAPHICS::Color(0.5f, 0.5f, 0.5f, 1.0f),
                GRAPHICS::Color(0.5f, 0.5f, 0.5f, 1.0f),
            },
            .AmbientColor = GRAPHICS::Color(0.2f, 0.2f, 0.2f, 1.0f),
            .DiffuseColor = GRAPHICS::Color(0.8f, 0.8f, 0.8f, 1.0f),
        }),
        std::make_shared<GRAPHICS::Material>(GRAPHICS::Material
        {
            /// @todo   Make this get values directly from the "material".
            .Shading = GRAPHICS::ShadingType::MATERIAL,
            .VertexColors =
            {
                GRAPHICS::Color(0.5f, 0.5f, 0.5f, 1.0f),
                GRAPHICS::Color(0.0f, 0.0f, 0.0f, 1.0f),
                GRAPHICS::Color(0.0f, 0.0f, 0.0f, 1.0f),
            },
            .AmbientColor = GRAPHICS::Color(0.2f, 0.2f, 0.2f, 1.0f),
            .DiffuseColor = GRAPHICS::Color(0.8f, 0.8f, 0.8f, 1.0f),
            .SpecularColor = GRAPHICS::Color(0.8f, 0.8f, 0.8f, 1.0f),
            .SpecularPower = 16.0f,
        }),
    };

    g_scene_index = 0;
    g_current_material_index = 0;
    g_scene = CreateScene(g_scene_index);

    // RUN A MESSAGE LOOP.
    float object_rotation_angle_in_radians = 0.0f;
    auto start_time = std::chrono::high_resolution_clock::now();

    constexpr float TARGET_FRAMES_PER_SECOND = 60.0f;
    constexpr std::chrono::duration<float, std::chrono::seconds::period> TARGET_SECONDS_PER_FRAME(1.0f / TARGET_FRAMES_PER_SECOND);
    GRAPHICS::FrameTimer frame_timer;
    bool running = true;
    while (running)
    {
        frame_timer.StartTimingFrame();

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

        auto current_time = std::chrono::high_resolution_clock::now();
        auto total_elapsed_time = std::chrono::duration_cast<std::chrono::duration<float>>(current_time - start_time).count();
        object_rotation_angle_in_radians = 0.5f * total_elapsed_time;
        for (auto& object_3D : g_scene.Objects)
        {
            object_3D;
            if (g_rotation_enabled.X)
            {
                object_3D.RotationInRadians.X = MATH::Angle<float>::Radians(object_rotation_angle_in_radians);
            }
            if (g_rotation_enabled.Y)
            {
                object_3D.RotationInRadians.Y = MATH::Angle<float>::Radians(object_rotation_angle_in_radians);
            }
            if (g_rotation_enabled.Z)
            {
                object_3D.RotationInRadians.Z = MATH::Angle<float>::Radians(object_rotation_angle_in_radians);
            }
        }

        // RENDER THE 3D SCENE.
        g_camera.Projection = GRAPHICS::ProjectionType::PERSPECTIVE;
        g_scene.BackgroundColor = GRAPHICS::Color(0.1f, 0.1f, 0.1f, 1.0f);
        if (g_depth_buffer_enabled)
        {
            GRAPHICS::SoftwareRasterizationAlgorithm::Render(g_scene, g_camera, g_backface_culling, perspective_projected_drawing, &perspective_depth_buffer);
        }
        else
        {
            GRAPHICS::SoftwareRasterizationAlgorithm::Render(g_scene, g_camera, g_backface_culling, perspective_projected_drawing, nullptr);
        }
        g_camera.Projection = GRAPHICS::ProjectionType::ORTHOGRAPHIC;
        g_scene.BackgroundColor = GRAPHICS::Color(0.2f, 0.2f, 0.2f, 1.0f);
        if (g_depth_buffer_enabled)
        {
            GRAPHICS::SoftwareRasterizationAlgorithm::Render(g_scene, g_camera, g_backface_culling, orthographic_projected_drawing, &orthographic_depth_buffer);
        }
        else
        {
            GRAPHICS::SoftwareRasterizationAlgorithm::Render(g_scene, g_camera, g_backface_culling, orthographic_projected_drawing, nullptr);
        }

        // RENDER DEBUG TEXT.
        debug_text_drawing.FillPixels(GRAPHICS::Color::BLACK);

        // DISPLAY STATISICS ABOUT FRAME TIMING.
        frame_timer.EndTimingFrame();
        float debug_text_top_y_position = 0.0f;

        GRAPHICS::GUI::Text control_help_text =
        {
            .String = "CamP=Arrow,D|Clip=N,F|FOV=V|B=Backface|XYZ=Rotate|S=Scene|M=Mat|L=Light",
            .Font = font.get(),
            .LeftTopPosition = MATH::Vector2f(0.0f, debug_text_top_y_position)
        };
        GRAPHICS::SoftwareRasterizationAlgorithm::Render(control_help_text, debug_text_drawing);

        debug_text_top_y_position += GRAPHICS::GUI::Font::GLYPH_DIMENSION_IN_PIXELS;
        GRAPHICS::GUI::Text backface_culling_text =
        {
            .String = "Backface Culling: " + std::to_string(g_backface_culling) + " Depth Buffer: " + std::to_string(g_depth_buffer_enabled),
            .Font = font.get(),
            .LeftTopPosition = MATH::Vector2f(0.0f, debug_text_top_y_position)
        };
        GRAPHICS::SoftwareRasterizationAlgorithm::Render(backface_culling_text, debug_text_drawing);

        debug_text_top_y_position += GRAPHICS::GUI::Font::GLYPH_DIMENSION_IN_PIXELS;
        GRAPHICS::GUI::Text scene_text =
        {
            .String = "Scene: " + g_scene_title + " (" + std::to_string(g_scene_index) + ")",
            .Font = font.get(),
            .LeftTopPosition = MATH::Vector2f(0.0f, debug_text_top_y_position)
        };
        GRAPHICS::SoftwareRasterizationAlgorithm::Render(scene_text, debug_text_drawing);

        debug_text_top_y_position += GRAPHICS::GUI::Font::GLYPH_DIMENSION_IN_PIXELS;
        GRAPHICS::GUI::Text material_text =
        {
            .String = "Material: " + std::to_string(g_current_material_index) + " " + g_material_names[g_current_material_index],
            .Font = font.get(),
            .LeftTopPosition = MATH::Vector2f(0.0f, debug_text_top_y_position)
        };
        GRAPHICS::SoftwareRasterizationAlgorithm::Render(material_text, debug_text_drawing);

        debug_text_top_y_position += GRAPHICS::GUI::Font::GLYPH_DIMENSION_IN_PIXELS;
        GRAPHICS::GUI::Text lighting_text =
        {
            .String = "Lighting: " + std::to_string(g_current_light_index) + " " + g_light_configuration_names[g_current_light_index],
            .Font = font.get(),
            .LeftTopPosition = MATH::Vector2f(0.0f, debug_text_top_y_position)
        };
        GRAPHICS::SoftwareRasterizationAlgorithm::Render(lighting_text, debug_text_drawing);

        debug_text_top_y_position += GRAPHICS::GUI::Font::GLYPH_DIMENSION_IN_PIXELS;
        GRAPHICS::GUI::Text frame_timing_text =
        {
            .String = frame_timer.GetFrameTimingText(),
            .Font = font.get(),
            .LeftTopPosition = MATH::Vector2f(0.0f, debug_text_top_y_position)
        };
        GRAPHICS::SoftwareRasterizationAlgorithm::Render(frame_timing_text, debug_text_drawing);

        debug_text_top_y_position += GRAPHICS::GUI::Font::GLYPH_DIMENSION_IN_PIXELS;
        GRAPHICS::GUI::Text camera_position_text =
        {
            .String = "Camera World Position = " + g_camera.WorldPosition.ToString(),
            .Font = font.get(),
            .LeftTopPosition = MATH::Vector2f(0.0f, debug_text_top_y_position)
        };
        GRAPHICS::SoftwareRasterizationAlgorithm::Render(camera_position_text, debug_text_drawing);

        debug_text_top_y_position += GRAPHICS::GUI::Font::GLYPH_DIMENSION_IN_PIXELS;
        GRAPHICS::GUI::Text camera_right_text =
        {
            .String = "Camera Right = " + g_camera.CoordinateFrame.Right.ToString(),
            .Font = font.get(),
            .LeftTopPosition = MATH::Vector2f(0.0f, debug_text_top_y_position)
        };
        GRAPHICS::SoftwareRasterizationAlgorithm::Render(camera_right_text, debug_text_drawing);

        debug_text_top_y_position += GRAPHICS::GUI::Font::GLYPH_DIMENSION_IN_PIXELS;
        GRAPHICS::GUI::Text camera_up_text =
        {
            .String = "Camera Up = " + g_camera.CoordinateFrame.Up.ToString(),
            .Font = font.get(),
            .LeftTopPosition = MATH::Vector2f(0.0f, debug_text_top_y_position)
        };
        GRAPHICS::SoftwareRasterizationAlgorithm::Render(camera_up_text, debug_text_drawing);

        debug_text_top_y_position += GRAPHICS::GUI::Font::GLYPH_DIMENSION_IN_PIXELS;
        GRAPHICS::GUI::Text camera_forward_text =
        {
            // Note that this is actually "backwards" from the view direction.
            .String = "Camera Forward = " + g_camera.CoordinateFrame.Forward.ToString(),
            .Font = font.get(),
            .LeftTopPosition = MATH::Vector2f(0.0f, debug_text_top_y_position)
        };
        GRAPHICS::SoftwareRasterizationAlgorithm::Render(camera_forward_text, debug_text_drawing);

        debug_text_top_y_position += GRAPHICS::GUI::Font::GLYPH_DIMENSION_IN_PIXELS;
        GRAPHICS::GUI::Text camera_clip_planes_text =
        {
            .String = "Camera Near/Far Clip Distances = " + std::to_string(g_camera.NearClipPlaneViewDistance) + ", " + std::to_string(g_camera.FarClipPlaneViewDistance),
            .Font = font.get(),
            .LeftTopPosition = MATH::Vector2f(0.0f, debug_text_top_y_position)
        };
        GRAPHICS::SoftwareRasterizationAlgorithm::Render(camera_clip_planes_text, debug_text_drawing);

        debug_text_top_y_position += GRAPHICS::GUI::Font::GLYPH_DIMENSION_IN_PIXELS;
        GRAPHICS::GUI::Text camera_fov_text =
        {
            .String = "Camera FOV = " + std::to_string(g_camera.FieldOfView.Value),
            .Font = font.get(),
            .LeftTopPosition = MATH::Vector2f(0.0f, debug_text_top_y_position)
        };
        GRAPHICS::SoftwareRasterizationAlgorithm::Render(camera_fov_text, debug_text_drawing);

        // DISPLAY THE RENDERED IMAGE IN THE WINDOW.
        g_window->DisplayAt(perspective_projected_drawing, 0, 0);
        g_window->DisplayAt(orthographic_projected_drawing, perspective_projected_drawing.GetWidthInPixels(), 0);
        g_window->DisplayAt(debug_text_drawing, 0, orthographic_projected_drawing.GetHeightInPixels());

#define FRAME_RATE_CAP 0
#if FRAME_RATE_CAP
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
