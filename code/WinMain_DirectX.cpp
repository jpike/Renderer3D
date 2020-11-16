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
#include <d3d11.h>
#include <dxgi1_6.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <wrl/client.h>
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

static std::string VERTEX_SHADER = R"HLSL( 
cbuffer TransformationMatrices
{
    matrix WorldMatrix;
    matrix ViewMatrix;
    matrix ProjectionMatrix;
    float4 LightPosition;
    float4 InputLightColor;
    int2 IsTexturedAndLit;
};

struct VertexInput
{
    float4 Position: POSITION;
    float4 Color: COLOR;
    float4 Normal: NORMAL;
    float2 TextureCoordinates: TEXCOORD0;
};

struct PixelInput
{
    float4 Position: SV_POSITION;
    float4 Color: COLOR;
    float2 TextureCoordinates: TEXCOORD0;
    bool IsTextured: BOOL;
    float4 LightColor: COLOR1;
};

PixelInput VertexShaderEntryPoint(VertexInput vertex_input)
{
    PixelInput pixel_input;

    float4 world_position = mul(WorldMatrix, vertex_input.Position);
    float4 view_position = mul(ViewMatrix, world_position);
    float4 projected_position = mul(ProjectionMatrix, view_position);
    
    //pixel_input.Position = world_position;
    //pixel_input.Position = view_position;
    //pixel_input.Position = projected_position;

    //pixel_input.Position = vertex_input.Position;
    pixel_input.Position = float4(
        projected_position.x / projected_position.w,
        projected_position.y / projected_position.w, 
        -projected_position.z / projected_position.w, 
        1.0);

    pixel_input.TextureCoordinates = vertex_input.TextureCoordinates;
    pixel_input.IsTextured = (IsTexturedAndLit.x == 1);

    pixel_input.Color = vertex_input.Color;

    if (IsTexturedAndLit.y == 1)
    {
        float3 direction_from_vertex_to_light = LightPosition.xyz - world_position.xyz;
        float3 unit_direction_from_point_to_light = normalize(direction_from_vertex_to_light);
        float illumination_proportion = dot(vertex_input.Normal.xyz, unit_direction_from_point_to_light);
        float clamped_illumination = max(0, illumination_proportion);
        float3 scaled_light_color = clamped_illumination * InputLightColor.rgb;
        pixel_input.LightColor = float4(scaled_light_color.rgb, 1.0);
    }
    else
    {
        pixel_input.LightColor = float4(1.0, 1.0, 1.0, 1.0);
    }
    

    return pixel_input;
}
)HLSL";

static std::string PIXEL_SHADER = R"HLSL( 
Texture2D texture_image;
SamplerState texture_sampler_state;

struct PixelInput
{
    float4 Position: SV_POSITION;
    float4 Color: COLOR;
    float2 TextureCoordinates: TEXCOORD0;
    bool IsTextured: BOOL;
    float4 LightColor: COLOR1;
};

float4 PixelShaderEntryPoint(PixelInput pixel_input): SV_TARGET
{
    if (pixel_input.IsTextured)
    {
        float4 texture_color = texture_image.Sample(texture_sampler_state, pixel_input.TextureCoordinates);
        float4 lit_texture_color = texture_color * pixel_input.LightColor;
        return float4(lit_texture_color.rgb, 1.0);
    }
    else
    {
        float4 lit_color = pixel_input.Color * pixel_input.LightColor;
        return float4(lit_color.rgb, 1.0);
    }
}
)HLSL";

#define DEPTH_BUFFER 1

struct TransformationMatrixBuffer
{
    DirectX::XMMATRIX WorldMatrix;
    DirectX::XMMATRIX ViewMatrix;
    DirectX::XMMATRIX ProjectionMatrix;
    /// @todo   Remove hack for texturing/light here.
    DirectX::XMFLOAT4 LightPosition;
    DirectX::XMFLOAT4 InputLightColor;
    DirectX::XMINT2 IsTexturedAndIsLit;
};

struct VertexInputBuffer
{
    DirectX::XMFLOAT4 Position;
    DirectX::XMFLOAT4 Color;
    DirectX::XMFLOAT4 Normal;
    DirectX::XMFLOAT2 TextureCoordinates;
};

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
                    if (g_scene.PointLights)
                    {
                        const GRAPHICS::Light& first_light = g_scene.PointLights->at(0);
                        std::string light_color_string =
                            "\nLight Color: " +
                            std::to_string(first_light.Color.Red) + ", " +
                            std::to_string(first_light.Color.Green) + ", " +
                            std::to_string(first_light.Color.Blue);
                        OutputDebugString(light_color_string.c_str());
                    }
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

    auto PrintResultIfFailed = [](const HRESULT result)
    {
        if (FAILED(result))
        {
            std::string error_message = "\nHRESULT = " + std::to_string(result);
            OutputDebugString(error_message.c_str());
        }
    };

    IDXGIFactory* factory = nullptr;
    HRESULT result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
    PrintResultIfFailed(result);

    std::optional<DXGI_MODE_DESC> smallest_matching_display_mode;
    std::vector<IDXGIAdapter*> adapters;
    IDXGIAdapter* adapter = nullptr;
    for (UINT adapter_index = 0;
        (result = factory->EnumAdapters(adapter_index, &adapter)) != DXGI_ERROR_NOT_FOUND;
        ++adapter_index)
    {
        if (FAILED(result))
        {
            PrintResultIfFailed(result);
            continue;
        }

        adapters.emplace_back(adapter);
        std::string adapter_message = "\nAdapter index = " + std::to_string(adapter_index);
        OutputDebugString(adapter_message.c_str());

        std::vector<IDXGIOutput*> adapter_outputs;
        IDXGIOutput* adapter_output = nullptr;
        for (UINT output_index = 0;
            (result = adapter->EnumOutputs(output_index, &adapter_output)) != DXGI_ERROR_NOT_FOUND;
            ++output_index)
        {
            if (FAILED(result))
            {
                PrintResultIfFailed(result);
                continue;
            }

            adapter_outputs.emplace_back(adapter_output);
            std::string output_message = "\nOutput index = " + std::to_string(output_index);
            OutputDebugString(output_message.c_str());

            std::vector<DXGI_FORMAT> display_formats =
            {
                //DXGI_FORMAT_UNKNOWN, // 0
                //DXGI_FORMAT_R8G8B8A8_TYPELESS, // 27 (41 results)
                DXGI_FORMAT_R8G8B8A8_UNORM, // 28 (41 results)
                //DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, // 29
                //DXGI_FORMAT_R8G8B8A8_UINT, // 30
                //DXGI_FORMAT_R8G8B8A8_SNORM, // 31
                //DXGI_FORMAT_R8G8B8A8_SINT, // 32 (41 results)
#if BGRA_FORMATS
                DXGI_FORMAT_B8G8R8A8_UNORM, // 87
                DXGI_FORMAT_B8G8R8X8_UNORM, // 88
                DXGI_FORMAT_B8G8R8A8_TYPELESS, // 90 (41 results)
                DXGI_FORMAT_B8G8R8A8_UNORM_SRGB, // 91
                DXGI_FORMAT_B8G8R8X8_TYPELESS, // 92
                DXGI_FORMAT_B8G8R8X8_UNORM_SRGB, // 93
#endif
            };

            for (const auto& display_format : display_formats)
            {
                UINT mode_count = 0;
                result = adapter_output->GetDisplayModeList(
                    display_format,
                    DXGI_ENUM_MODES_INTERLACED | DXGI_ENUM_MODES_SCALING,
                    &mode_count,
                    NULL // Get all
                );
                if (FAILED(result))
                {
                    PrintResultIfFailed(result);
                    continue;
                }
                std::string mode_count_message = "\nDisplay mode count = " + std::to_string(mode_count);
                OutputDebugString(mode_count_message.c_str());

                std::unique_ptr<DXGI_MODE_DESC[]> display_mode_descriptions = std::make_unique<DXGI_MODE_DESC[]>(mode_count);
                result = adapter_output->GetDisplayModeList(
                    display_format,
                    DXGI_ENUM_MODES_INTERLACED | DXGI_ENUM_MODES_SCALING,
                    &mode_count,
                    display_mode_descriptions.get());
                if (FAILED(result))
                {
                    PrintResultIfFailed(result);
                    continue;
                }
                for (std::size_t display_mode_index = 0; display_mode_index < mode_count; ++display_mode_index)
                {
                    const DXGI_MODE_DESC& display_mode_description = display_mode_descriptions[display_mode_index];
                    std::string display_mode_message =
                        "\nDisplay mode: " + std::to_string(display_mode_index) +
                        "\n\tWidth = " + std::to_string(display_mode_description.Width) +
                        "\n\tHeight = " + std::to_string(display_mode_description.Height) +
                        "\n\tRefreshRate = " +
                        std::to_string(display_mode_description.RefreshRate.Numerator) +
                        "/" +
                        std::to_string(display_mode_description.RefreshRate.Denominator) +
                        "\n\tFormat = " + std::to_string(display_mode_description.Format) +
                        "\n\tScanlineOrdering = " + std::to_string(display_mode_description.ScanlineOrdering) +
                        "\n\tScaling = " + std::to_string(display_mode_description.Scaling);
                    OutputDebugString(display_mode_message.c_str());

                    if (!smallest_matching_display_mode)
                    {
                        bool current_display_mode_big_enough = (
                            display_mode_description.Width >= SCREEN_WIDTH_IN_PIXELS &&
                            display_mode_description.Height >= SCREEN_HEIGHT_IN_PIXELS);
                        if (current_display_mode_big_enough)
                        {
                            smallest_matching_display_mode = display_mode_description;
                        }
                    }
                }
            }
            if (smallest_matching_display_mode)
            {
                std::string display_mode_message = std::string("\nMatching display mode: ") +
                    "\n\tWidth = " + std::to_string(smallest_matching_display_mode->Width) +
                    "\n\tHeight = " + std::to_string(smallest_matching_display_mode->Height) +
                    "\n\tRefreshRate = " +
                    std::to_string(smallest_matching_display_mode->RefreshRate.Numerator) +
                    "/" +
                    std::to_string(smallest_matching_display_mode->RefreshRate.Denominator) +
                    "\n\tFormat = " + std::to_string(smallest_matching_display_mode->Format) +
                    "\n\tScanlineOrdering = " + std::to_string(smallest_matching_display_mode->ScanlineOrdering) +
                    "\n\tScaling = " + std::to_string(smallest_matching_display_mode->Scaling);
                OutputDebugString(display_mode_message.c_str());
            }

            adapter_output->Release();
        }


        DXGI_ADAPTER_DESC adapter_description;
        result = adapter->GetDesc(&adapter_description);
        PrintResultIfFailed(result);
        // https://en.cppreference.com/w/cpp/locale/wstring_convert
        OutputDebugStringW(L"\n");
        OutputDebugStringW(adapter_description.Description);
        std::string adapter_description_message =
            "\n\tVendorId = " + std::to_string(adapter_description.VendorId) +
            "\n\tDeviceId = " + std::to_string(adapter_description.DeviceId) +
            "\n\tSubSysId = " + std::to_string(adapter_description.SubSysId) +
            "\n\tDedicatedVideoMemory = " + std::to_string(adapter_description.DedicatedVideoMemory) +
            "\n\tDedicatedSystemMemory = " + std::to_string(adapter_description.DedicatedSystemMemory) +
            "\n\tSharedSystemMemory = " + std::to_string(adapter_description.SharedSystemMemory) +
            "\n\tAdapterLuid high = " + std::to_string(adapter_description.AdapterLuid.HighPart) +
            "\n\tAdapterLuid low = " + std::to_string(adapter_description.AdapterLuid.LowPart);
        OutputDebugString(adapter_description_message.c_str());

        adapter->Release();
    }

    factory->Release();

    DXGI_MODE_DESC backbuffer_display_mode_description = *smallest_matching_display_mode;
    backbuffer_display_mode_description.Width = SCREEN_WIDTH_IN_PIXELS;
    backbuffer_display_mode_description.Height = SCREEN_HEIGHT_IN_PIXELS;

    DXGI_SAMPLE_DESC multisampling_parameters =
    {
        .Count = 1,
        .Quality = 0
    };
    // Had to tweak a lot of these parameters to get it to work.
    DXGI_SWAP_CHAIN_DESC swap_chain_description =
    {
        .BufferDesc = backbuffer_display_mode_description,
        .SampleDesc = multisampling_parameters,
        .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
        .BufferCount = 1,
        .OutputWindow = g_window->WindowHandle,
        .Windowed = TRUE,
        .SwapEffect = DXGI_SWAP_EFFECT_DISCARD,
        .Flags = 0
    };
    IDXGISwapChain* swap_chain = nullptr;;
    ID3D11Device* device = nullptr;
    D3D_FEATURE_LEVEL feature_level = (D3D_FEATURE_LEVEL)0;
    ID3D11DeviceContext* device_context = nullptr;
    result = D3D11CreateDeviceAndSwapChain(
        NULL, // default adapter
        D3D_DRIVER_TYPE_HARDWARE,
        NULL, // No software renderer
        0, // D3D11_CREATE_DEVICE_DEBUG | D3D11_CREATE_DEVICE_DEBUGGABLE,
        NULL, // Basically use whatever feature levels we can.
        0, // No feature levels provided.
        D3D11_SDK_VERSION,
        &swap_chain_description,
        &swap_chain,
        &device,
        &feature_level,
        &device_context);
    PrintResultIfFailed(result);

    ID3D11Texture2D* back_buffer = nullptr;
    constexpr UINT SINGLE_BUFFER_INDEX = 0;
    result = swap_chain->GetBuffer(SINGLE_BUFFER_INDEX, __uuidof(ID3D11Texture2D), (void**)&back_buffer);
    PrintResultIfFailed(result);

    ID3D11RenderTargetView* render_target_view = nullptr;
    constexpr D3D11_RENDER_TARGET_VIEW_DESC* USE_MIPMAP_LEVEL_0 = NULL;
    result = device->CreateRenderTargetView(back_buffer, USE_MIPMAP_LEVEL_0, &render_target_view);
    PrintResultIfFailed(result);
    back_buffer->Release();

#if DEPTH_BUFFER
    D3D11_TEXTURE2D_DESC depth_buffer_description =
    {
        .Width = SCREEN_WIDTH_IN_PIXELS,
        .Height = SCREEN_HEIGHT_IN_PIXELS,
        .MipLevels = 1,
        .ArraySize = 1,
        .Format = DXGI_FORMAT_D24_UNORM_S8_UINT,
        .SampleDesc = {.Count = 1, .Quality = 0 },
        .Usage = D3D11_USAGE_DEFAULT,
        .BindFlags = D3D11_BIND_DEPTH_STENCIL,
        .CPUAccessFlags = 0,
        .MiscFlags = 0,
    };
    ID3D11Texture2D* depth_stencil_buffer = nullptr;
    result = device->CreateTexture2D(&depth_buffer_description, NULL, &depth_stencil_buffer);
    PrintResultIfFailed(result);

    D3D11_DEPTH_STENCIL_DESC depth_stencil_description =
    {
        .DepthEnable = TRUE,
        .DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL,
        .DepthFunc = D3D11_COMPARISON_LESS,
        .StencilEnable = TRUE,
        .StencilReadMask = 0xFF,
        .StencilWriteMask = 0xFF,
        .FrontFace =
        {
            .StencilFailOp = D3D11_STENCIL_OP_KEEP,
            .StencilDepthFailOp = D3D11_STENCIL_OP_INCR,
            .StencilPassOp = D3D11_STENCIL_OP_KEEP,
            .StencilFunc = D3D11_COMPARISON_ALWAYS
        },
        .BackFace =
        {
            .StencilFailOp = D3D11_STENCIL_OP_KEEP,
            .StencilDepthFailOp = D3D11_STENCIL_OP_DECR,
            .StencilPassOp = D3D11_STENCIL_OP_KEEP,
            .StencilFunc = D3D11_COMPARISON_ALWAYS
        },
    };

    ID3D11DepthStencilState* depth_stencil_state = nullptr;
    result = device->CreateDepthStencilState(&depth_stencil_description, &depth_stencil_state);
    PrintResultIfFailed(result);
    device_context->OMSetDepthStencilState(depth_stencil_state, 1);

    D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_description =
    {
        .Format = DXGI_FORMAT_D24_UNORM_S8_UINT,
        .ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D,
        .Texture2D =
        {
            .MipSlice = 0
        }
    };
    ID3D11DepthStencilView* depth_stencil_view = nullptr;
    result = device->CreateDepthStencilView(depth_stencil_buffer, &depth_stencil_view_description, &depth_stencil_view);
    PrintResultIfFailed(result);
#else
    ID3D11DepthStencilView* depth_stencil_view = nullptr;
#endif

    device_context->OMSetRenderTargets(1, &render_target_view, depth_stencil_view);
    D3D11_VIEWPORT viewport =
    {
        .TopLeftX = 0.0f,
        .TopLeftY = 0.0f,
        .Width = SCREEN_WIDTH_IN_PIXELS,
        .Height = SCREEN_HEIGHT_IN_PIXELS,
        .MinDepth = 0.0f,
        .MaxDepth = 1.0f
    };
    device_context->RSSetViewports(1, &viewport);

    D3D11_RASTERIZER_DESC rasterizer_description =
    {
        .FillMode = D3D11_FILL_SOLID,
        .CullMode = D3D11_CULL_BACK,
        .FrontCounterClockwise = TRUE,
        .DepthBias = 0,
        .DepthBiasClamp = 0.0f,
        .SlopeScaledDepthBias = 0.0f,
        .DepthClipEnable = FALSE, //TRUE,
        .ScissorEnable = FALSE,
        .MultisampleEnable = FALSE,
        .AntialiasedLineEnable = FALSE,
    };
    ID3D11RasterizerState* rasterizer_state = nullptr;
    result = device->CreateRasterizerState(&rasterizer_description, &rasterizer_state);
    PrintResultIfFailed(result);
    device_context->RSSetState(rasterizer_state);

    ID3DBlob* vertex_shader_compiled_code = nullptr;
    ID3DBlob* vertex_shader_error_messages = nullptr;
    result = D3DCompile(
        VERTEX_SHADER.data(),
        VERTEX_SHADER.size(),
        NULL,
        NULL,
        NULL,
        "VertexShaderEntryPoint",
        "vs_5_0",
        D3DCOMPILE_DEBUG | D3DCOMPILE_ENABLE_STRICTNESS,
        0,
        &vertex_shader_compiled_code,
        &vertex_shader_error_messages);
    PrintResultIfFailed(result);
    if (FAILED(result))
    {
        if (vertex_shader_error_messages)
        {
            OutputDebugString((char*)vertex_shader_error_messages->GetBufferPointer());
        }
    }

    ID3DBlob* pixel_shader_compiled_code = nullptr;
    ID3DBlob* pixel_shader_error_messages = nullptr;
    result = D3DCompile(
        PIXEL_SHADER.data(),
        PIXEL_SHADER.size(),
        NULL,
        NULL,
        NULL,
        "PixelShaderEntryPoint",
        "ps_5_0",
        D3DCOMPILE_DEBUG | D3DCOMPILE_ENABLE_STRICTNESS,
        0,
        &pixel_shader_compiled_code,
        &pixel_shader_error_messages);
    PrintResultIfFailed(result);
    if (FAILED(result))
    {
        if (pixel_shader_error_messages)
        {
            OutputDebugString((char*)pixel_shader_error_messages->GetBufferPointer());
        }
    }

    ID3D11VertexShader* vertex_shader = nullptr;
    result = device->CreateVertexShader(
        vertex_shader_compiled_code->GetBufferPointer(),
        vertex_shader_compiled_code->GetBufferSize(),
        NULL,
        &vertex_shader);
    PrintResultIfFailed(result);

    ID3D11PixelShader* pixel_shader = nullptr;
    result = device->CreatePixelShader(
        pixel_shader_compiled_code->GetBufferPointer(),
        pixel_shader_compiled_code->GetBufferSize(),
        NULL,
        &pixel_shader);
    PrintResultIfFailed(result);

    std::vector<D3D11_INPUT_ELEMENT_DESC> vertex_shader_input_description =
    {
        D3D11_INPUT_ELEMENT_DESC
        {
            .SemanticName = "POSITION",
            .SemanticIndex = 0,
            .Format = DXGI_FORMAT_R32G32B32A32_FLOAT,
            .InputSlot = 0,
            .AlignedByteOffset = 0,
            .InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
            .InstanceDataStepRate = 0,
        },
        D3D11_INPUT_ELEMENT_DESC
        {
            .SemanticName = "COLOR",
            .SemanticIndex = 0,
            .Format = DXGI_FORMAT_R32G32B32A32_FLOAT,
            .InputSlot = 0,
            .AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT,
            .InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
            .InstanceDataStepRate = 0,
        },
        D3D11_INPUT_ELEMENT_DESC
        {
            .SemanticName = "NORMAL",
            .SemanticIndex = 0,
            .Format = DXGI_FORMAT_R32G32B32A32_FLOAT,
            .InputSlot = 0,
            .AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT,
            .InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
            .InstanceDataStepRate = 0,
        },
        D3D11_INPUT_ELEMENT_DESC
        {
            .SemanticName = "TEXCOORD",
            .SemanticIndex = 0,
            .Format = DXGI_FORMAT_R32G32_FLOAT,
            .InputSlot = 0,
            .AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT,
            .InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
            .InstanceDataStepRate = 0,
        },
    };
    ID3D11InputLayout* vertex_input_layout = nullptr;
    result = device->CreateInputLayout(
        vertex_shader_input_description.data(),
        (UINT)vertex_shader_input_description.size(),
        vertex_shader_compiled_code->GetBufferPointer(),
        vertex_shader_compiled_code->GetBufferSize(),
        &vertex_input_layout);
    PrintResultIfFailed(result);
    pixel_shader_compiled_code->Release();
    vertex_shader_compiled_code->Release();

    D3D11_SAMPLER_DESC sampler_description =
    {
        .Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR,
        .AddressU = D3D11_TEXTURE_ADDRESS_WRAP,
        .AddressV = D3D11_TEXTURE_ADDRESS_WRAP,
        .AddressW = D3D11_TEXTURE_ADDRESS_WRAP,
        .MipLODBias = 0.0f,
        .MaxAnisotropy = 1,
        .ComparisonFunc = D3D11_COMPARISON_ALWAYS,
        .BorderColor = {},
        .MinLOD = 0,
        .MaxLOD = D3D11_FLOAT32_MAX
    };
    ID3D11SamplerState* sampler_state = nullptr;
    result = device->CreateSamplerState(&sampler_description, &sampler_state);
    PrintResultIfFailed(result);

    D3D11_BUFFER_DESC transformation_matrix_buffer_description =
    {
        .ByteWidth = sizeof(TransformationMatrixBuffer),
        .Usage = D3D11_USAGE_DYNAMIC,
        .BindFlags = D3D11_BIND_CONSTANT_BUFFER,
        .CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
        .MiscFlags = 0,
        .StructureByteStride = 0,
    };
    ID3D11Buffer* transformation_matrix_buffer = nullptr;
    result = device->CreateBuffer(&transformation_matrix_buffer_description, NULL, &transformation_matrix_buffer);
    PrintResultIfFailed(result);

    // LOAD THE DEFAULT FONT.
    std::shared_ptr<GRAPHICS::GUI::Font> font = GRAPHICS::GUI::Font::LoadSystemDefaultFixedFont();
    if (!font)
    {
        OutputDebugString("Failed to load default font.");
        return EXIT_FAILURE;
    }

    g_camera = GRAPHICS::Camera::LookAtFrom(MATH::Vector3f(0.0f, 0.0f, 0.0f), MATH::Vector3f(0.0f, 0.0f, 2.0f));
    g_camera.NearClipPlaneViewDistance = 1.0f;
    g_camera.FarClipPlaneViewDistance = 1000.0f;

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

        float background_color[] =
        {
            g_scene.BackgroundColor.Red,
            g_scene.BackgroundColor.Green,
            g_scene.BackgroundColor.Blue,
            g_scene.BackgroundColor.Alpha,
        };
        device_context->ClearRenderTargetView(render_target_view, background_color);
        device_context->ClearDepthStencilView(
            depth_stencil_view,
            D3D11_CLEAR_DEPTH,
            1.0f,
            0);

        float aspect_ratio = static_cast<float>(SCREEN_WIDTH_IN_PIXELS) / static_cast<float>(SCREEN_HEIGHT_IN_PIXELS);
        DirectX::XMMATRIX perspective_matrix = DirectX::XMMatrixPerspectiveFovLH(
            MATH::Angle<float>::DegreesToRadians(g_camera.FieldOfView).Value,
            aspect_ratio,
            g_camera.NearClipPlaneViewDistance,
            g_camera.FarClipPlaneViewDistance);
        DirectX::XMMATRIX orthographic_matrix = DirectX::XMMatrixOrthographicLH(
            (float)SCREEN_WIDTH_IN_PIXELS,
            (float)SCREEN_HEIGHT_IN_PIXELS,
            g_camera.NearClipPlaneViewDistance,
            g_camera.FarClipPlaneViewDistance);

#define TRANSPOSE 0
#if TRANSPOSE
        MATH::Matrix4x4f camera_view_transform = g_camera.ViewTransform();
        DirectX::XMMATRIX view_matrix = DirectX::XMMATRIX(camera_view_transform.ElementsInRowMajorOrder());

        MATH::Matrix4x4f projection_transform = g_camera.ProjectionTransform();
        DirectX::XMMATRIX projection_matrix = DirectX::XMMATRIX(projection_transform.ElementsInRowMajorOrder());
#else
        MATH::Matrix4x4f camera_view_transform = g_camera.ViewTransform();
        DirectX::XMMATRIX view_matrix = DirectX::XMMATRIX(camera_view_transform.Elements.ValuesInColumnMajorOrder().data());

        MATH::Matrix4x4f projection_transform = g_camera.ProjectionTransform();
        DirectX::XMMATRIX projection_matrix = DirectX::XMMATRIX(projection_transform.Elements.ValuesInColumnMajorOrder().data());
#endif

        bool is_lit = g_scene.PointLights.has_value();

        for (const auto& object_3D : g_scene.Objects)
        {
            for (const auto& triangle : object_3D.Triangles)
            {
    #if TRANSPOSE
                MATH::Matrix4x4f world_transform = object_3D.WorldTransform();
                DirectX::XMMATRIX world_matrix = DirectX::XMMATRIX(world_transform.ElementsInRowMajorOrder());
    #else
                MATH::Matrix4x4f world_transform = object_3D.WorldTransform();
                DirectX::XMMATRIX world_matrix = DirectX::XMMATRIX(world_transform.Elements.ValuesInColumnMajorOrder().data());
    #endif

                D3D11_MAPPED_SUBRESOURCE mapped_matrix_buffer;
                result = device_context->Map(transformation_matrix_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_matrix_buffer);
                PrintResultIfFailed(result);
                TransformationMatrixBuffer* matrix_buffer = (TransformationMatrixBuffer*)mapped_matrix_buffer.pData;

                bool is_textured = !triangle.Material->VertexTextureCoordinates.empty();
                matrix_buffer->IsTexturedAndIsLit.x = is_textured;

                matrix_buffer->IsTexturedAndIsLit.y = is_lit;
                if (is_lit)
                {
                    const GRAPHICS::Light& first_light = g_scene.PointLights->at(0);
                    matrix_buffer->LightPosition = DirectX::XMFLOAT4(
                        first_light.PointLightWorldPosition.X,
                        first_light.PointLightWorldPosition.Y,
                        first_light.PointLightWorldPosition.Z,
                        1.0f);
                    matrix_buffer->InputLightColor = DirectX::XMFLOAT4(
                        first_light.Color.Red,
                        first_light.Color.Green,
                        first_light.Color.Blue,
                        first_light.Color.Alpha);
                }

    #if TRANSPOSE
                matrix_buffer->WorldMatrix = XMMatrixTranspose(world_matrix);
                matrix_buffer->ViewMatrix = XMMatrixTranspose(view_matrix);
                matrix_buffer->ProjectionMatrix = XMMatrixTranspose(projection_matrix);
    #else
                matrix_buffer->WorldMatrix = world_matrix;
                matrix_buffer->ViewMatrix = view_matrix;
                matrix_buffer->ProjectionMatrix = projection_matrix;
    #endif
                device_context->Unmap(transformation_matrix_buffer, 0);
                device_context->VSSetConstantBuffers(0, 1, &transformation_matrix_buffer);

                device_context->IASetInputLayout(vertex_input_layout);
                device_context->VSSetShader(vertex_shader, NULL, 0);
                device_context->PSSetShader(pixel_shader, NULL, 0);

                D3D11_BUFFER_DESC vertex_buffer_description
                {
                    .ByteWidth = sizeof(VertexInputBuffer) * GRAPHICS::Triangle::VERTEX_COUNT,
                    .Usage = D3D11_USAGE_DEFAULT,
                    .BindFlags = D3D11_BIND_VERTEX_BUFFER,
                    .CPUAccessFlags = 0,
                    .MiscFlags = 0,
                    .StructureByteStride = 0,
                };

                ID3D11Texture2D* object_texture = nullptr;
                ID3D11ShaderResourceView* texture_view = nullptr;
                std::vector<MATH::Vector2f> texture_coordinates;
                if (is_textured)
                {
                    texture_coordinates = triangle.Material->VertexTextureCoordinates;

                    D3D11_TEXTURE2D_DESC texture_description =
                    {
                        .Width = triangle.Material->Texture->GetWidthInPixels(),
                        .Height = triangle.Material->Texture->GetHeightInPixels(),
                        .MipLevels = 0,
                        .ArraySize = 1,
                        .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
                        .SampleDesc = {.Count = 1, .Quality = 0 },
                        .Usage = D3D11_USAGE_DEFAULT,
                        .BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET,
                        .CPUAccessFlags = 0,
                        .MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS
                    };
                    result = device->CreateTexture2D(&texture_description, NULL, &object_texture);
                    PrintResultIfFailed(result);
                    UINT texture_row_pitch = (4 * texture_description.Width) * sizeof(uint8_t);
                    device_context->UpdateSubresource(
                        object_texture,
                        0,
                        NULL,
                        triangle.Material->Texture->GetRawData(),
                        texture_row_pitch,
                        0);

                    D3D11_SHADER_RESOURCE_VIEW_DESC texture_shader_resource_description =
                    {
                        .Format = texture_description.Format,
                        .ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D,
                        .Texture2D = 
                        {
                            .MostDetailedMip = 0,
                            .MipLevels = 1
                        }
                    };
                    result = device->CreateShaderResourceView(object_texture, &texture_shader_resource_description, &texture_view);
                    PrintResultIfFailed(result);
                    device_context->GenerateMips(texture_view);
                }
                else
                {
                    texture_coordinates.emplace_back(MATH::Vector2f());
                    texture_coordinates.emplace_back(MATH::Vector2f());
                    texture_coordinates.emplace_back(MATH::Vector2f());
                }

                GRAPHICS::Triangle world_space_triangle = triangle;
                for (auto& vertex : world_space_triangle.Vertices)
                {
                    MATH::Vector4f homogeneous_vertex = MATH::Vector4f::HomogeneousPositionVector(vertex);
                    MATH::Vector4f world_homogeneous_vertex = world_transform * homogeneous_vertex;
                    vertex = MATH::Vector3f(world_homogeneous_vertex.X, world_homogeneous_vertex.Y, world_homogeneous_vertex.Z);
                }

                MATH::Vector3f surface_normal = world_space_triangle.SurfaceNormal();
                VertexInputBuffer vertices[] =
                {
                    VertexInputBuffer
                    {
                        .Position = DirectX::XMFLOAT4(triangle.Vertices[0].X, triangle.Vertices[0].Y, triangle.Vertices[0].Z, 1.0f),
                        .Color = DirectX::XMFLOAT4(
                            triangle.Material->VertexColors[0].Red,
                            triangle.Material->VertexColors[0].Green,
                            triangle.Material->VertexColors[0].Blue,
                            triangle.Material->VertexColors[0].Alpha),
                        .Normal = DirectX::XMFLOAT4(surface_normal.X, surface_normal.Y, surface_normal.Z, 1.0f),
                        .TextureCoordinates = DirectX::XMFLOAT2(
                            texture_coordinates[0].X,
                            texture_coordinates[0].Y),
                    },
                    VertexInputBuffer
                    {
                        .Position = DirectX::XMFLOAT4(triangle.Vertices[1].X, triangle.Vertices[1].Y, triangle.Vertices[1].Z, 1.0f),
                        .Color = DirectX::XMFLOAT4(
                            triangle.Material->VertexColors[1].Red,
                            triangle.Material->VertexColors[1].Green,
                            triangle.Material->VertexColors[1].Blue,
                            triangle.Material->VertexColors[1].Alpha),
                        .Normal = DirectX::XMFLOAT4(surface_normal.X, surface_normal.Y, surface_normal.Z, 1.0f),
                        .TextureCoordinates = DirectX::XMFLOAT2(
                            texture_coordinates[1].X,
                            texture_coordinates[1].Y),
                    },
                    VertexInputBuffer
                    {
                        .Position = DirectX::XMFLOAT4(triangle.Vertices[2].X, triangle.Vertices[2].Y, triangle.Vertices[2].Z, 1.0f),
                        .Color = DirectX::XMFLOAT4(
                            triangle.Material->VertexColors[2].Red,
                            triangle.Material->VertexColors[2].Green,
                            triangle.Material->VertexColors[2].Blue,
                            triangle.Material->VertexColors[2].Alpha),
                        .Normal = DirectX::XMFLOAT4(surface_normal.X, surface_normal.Y, surface_normal.Z, 1.0f),
                        .TextureCoordinates = DirectX::XMFLOAT2(
                            texture_coordinates[2].X,
                            texture_coordinates[2].Y),
                    },
                };
                D3D11_SUBRESOURCE_DATA vertex_data
                {
                    .pSysMem = vertices,
                    .SysMemPitch = 0,
                    .SysMemSlicePitch = 0,
                };
                ID3D11Buffer* vertex_buffer = nullptr;
                result = device->CreateBuffer(&vertex_buffer_description, &vertex_data, &vertex_buffer);
                PrintResultIfFailed(result);
                
                unsigned int stride = sizeof(VertexInputBuffer);
                unsigned int offset = 0;
                device_context->IASetVertexBuffers(0, 1, &vertex_buffer, &stride, &offset);
                device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

                if (is_textured)
                {
                    device_context->PSSetSamplers(0, 1, &sampler_state);
                    device_context->PSSetShaderResources(0, 1, &texture_view);
                }

                UINT vertex_count = GRAPHICS::Triangle::VERTEX_COUNT;
                constexpr UINT START_VERTEX_INDEX = 0;
                device_context->Draw(vertex_count, START_VERTEX_INDEX);

                vertex_buffer->Release();

                if (is_textured)
                {
                    texture_view->Release();
                    object_texture->Release();
                }
            }
        }

        swap_chain->Present(1, 0);
     
        // DISPLAY STATISICS ABOUT FRAME TIMING.
        frame_timer.EndTimingFrame();

        // DISPLAY THE RENDERED IMAGE IN THE WINDOW.
        

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

    sampler_state->Release();
    transformation_matrix_buffer->Release();
    vertex_input_layout->Release();
    pixel_shader->Release();
    vertex_shader->Release();
    rasterizer_state->Release();
#if DEPTH_BUFFER
    depth_stencil_view->Release();
    depth_stencil_state->Release();
    depth_stencil_buffer->Release();
#endif
    render_target_view->Release();
    swap_chain->Release();
    device_context->Release();
    device->Release();

    return EXIT_SUCCESS;
}
