#pragma once

#include <memory>
#include <string>
#include <Windows.h>
#include "Graphics/RenderTarget.h"

/// Holds code for graphical user interface windows.
namespace WINDOWING
{
    /// A window implemented using the Win32 API for the Windows operating system.
    class Win32Window
    {
    public:
        // CONSTRUCTION.
        static std::unique_ptr<Win32Window> Create(
            const WNDCLASSEX& window_class,
            const std::string& window_title,
            const int width_in_pixels = CW_USEDEFAULT,
            const int height_in_pixels = CW_USEDEFAULT);
        explicit Win32Window(const HWND window_handle);

        // RENDERING.
        void Display(const GRAPHICS::RenderTarget& render_target);

        // MEMBER VARIABLES.
        /// The handle to window.
        HWND WindowHandle;
    };
}