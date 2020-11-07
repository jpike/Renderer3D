#pragma once

#include <memory>
#include <string>
#include <Windows.h>
#include "Graphics/Bitmap.h"

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
        void Display(const GRAPHICS::Bitmap& bitmap);
        void DisplayAt(const GRAPHICS::Bitmap& bitmap, int left_x, int top_y);

        // MEMBER VARIABLES.
        /// The handle to window.
        HWND WindowHandle;
    };
}