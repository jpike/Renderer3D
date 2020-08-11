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
#include "Graphics/Object3D.h"
#include "Graphics/OpenGL/GraphicsDevice.h"
#include "Graphics/OpenGL/OpenGL.h"
#include "Graphics/OpenGL/OpenGLRenderer.h"
#include "Graphics/RayTracing/RayTracingAlgorithm.h"
#include "Graphics/Renderer.h"
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
/// The objects currently being rendered.
static std::vector<GRAPHICS::Object3D> g_objects;
/// The type of renderer currently being used.
static RendererType g_current_renderer_type = RendererType::SOFTWARE_RASTERIZER;
/// The software rasterizer.
static std::unique_ptr<GRAPHICS::Renderer> g_software_rasterizer = nullptr;
static std::unique_ptr<GRAPHICS::RenderTarget> g_software_render_target = nullptr;
/// The ray tracer.
static std::unique_ptr<GRAPHICS::RAY_TRACING::RayTracingAlgorithm> g_ray_tracer = nullptr;
/// The OpenGL renderer.
static std::unique_ptr<GRAPHICS::OPEN_GL::OpenGLRenderer> g_open_gl_renderer = nullptr;
std::shared_ptr<GRAPHICS::OPEN_GL::GraphicsDevice> g_open_gl_graphics_device = nullptr;


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
            int virtual_key_code = static_cast<int>(w_param);
            // https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
            switch (virtual_key_code)
            {
                /// @todo
                case 0x31: // 1
                {
                    bool software_rasterizer_already_being_used = (RendererType::SOFTWARE_RASTERIZER == g_current_renderer_type);
                    if (!software_rasterizer_already_being_used)
                    {
                        if (!g_software_rasterizer)
                        {
                            g_software_rasterizer = std::make_unique<GRAPHICS::Renderer>();
                        }

                        if (!g_software_render_target)
                        {
                            /// @todo   Centralize screen dimensions.
                            g_software_render_target = std::make_unique<GRAPHICS::RenderTarget>(400, 400, GRAPHICS::ColorFormat::ARGB);
                        }

                        /// @todo   Global camera?
                        g_software_rasterizer->Camera = GRAPHICS::Camera::LookAtFrom(MATH::Vector3f(0.0f, 0.0f, 0.0f), MATH::Vector3f(0.0f, 0.0f, 100.0f));

                        g_current_renderer_type = RendererType::SOFTWARE_RASTERIZER;
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

                        /// @todo   Global camera?
                        g_ray_tracer->Camera = GRAPHICS::Camera::LookAtFrom(MATH::Vector3f(0.0f, 0.0f, 0.0f), MATH::Vector3f(0.0f, 0.0f, 1.0f));

                        g_current_renderer_type = RendererType::SOFTWARE_RAY_TRACER;
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

                        glMatrixMode(GL_PROJECTION);
                        glLoadIdentity();
                        glOrtho(-200.0f, 200.0f, -200.0f, 200.0f, -1.0f, 1.0f);
                        glMatrixMode(GL_MODELVIEW);
                        glLoadIdentity();

                        g_current_renderer_type = RendererType::OPEN_GL;
                    }
                    break;
                }
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

    // INITIALIZE THE DEFAULT RENDERER.
    if (!g_software_rasterizer)
    {
        g_software_rasterizer = std::make_unique<GRAPHICS::Renderer>();
    }
    if (!g_software_render_target)
    {
        /// @todo   Centralize screen dimensions.
        g_software_render_target = std::make_unique<GRAPHICS::RenderTarget>(400, 400, GRAPHICS::ColorFormat::ARGB);
    }
    /// @todo   Global camera?
    g_software_rasterizer->Camera = GRAPHICS::Camera::LookAtFrom(MATH::Vector3f(0.0f, 0.0f, 0.0f), MATH::Vector3f(0.0f, 0.0f, 100.0f));
    g_current_renderer_type = RendererType::SOFTWARE_RASTERIZER;

    // CREATE EXAMPLE OBJECTS.
    std::vector<GRAPHICS::Object3D> objects;
    GRAPHICS::Object3D triangle_object;
    triangle_object.Triangles =
    {
        GRAPHICS::Triangle(
            nullptr,
            {
                MATH::Vector3f(0.0f, 200.0f, 0.0f),
                MATH::Vector3f(-200.0f, -200.0f, 0.0f),
                MATH::Vector3f(200.0f, -200.0f, 0.0f)
            })
    };
    objects.push_back(triangle_object);

    // RUN A MESSAGE LOOP.
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

        // RENDER THE SCENE BASED ON THE CURRENT RENDERER.
        switch (g_current_renderer_type)
        {
            case RendererType::SOFTWARE_RASTERIZER:
            {
                // CLEAR THE SCREEN FROM THE PREVIOUS FRAME.
                g_software_render_target->FillPixels(GRAPHICS::Color::BLACK);

                // RENDER ALL OBJECTS.
                for (auto object_3D : g_objects)
                {
                    const std::vector<GRAPHICS::Light> NO_LIGHTS_YET;
                    g_software_rasterizer->Render(object_3D, NO_LIGHTS_YET, *g_software_render_target);
                }

                // DISPLAY THE RENDERED OBJECTS IN THE WINDOW.
                g_window->Display(*g_software_render_target);

                break;
            }
            case RendererType::OPEN_GL:
            {
                g_open_gl_renderer->ClearScreen(GRAPHICS::Color::BLACK);
                for (const auto& object_3D : objects)
                {
                    g_open_gl_renderer->Render(object_3D);
                }

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

#define DISPLAY_FRAME_TIMES 1
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
