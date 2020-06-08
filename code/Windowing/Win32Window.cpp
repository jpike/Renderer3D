#include "Windowing/Win32Window.h"

namespace WINDOWING
{
    /// Attempts to create a new window based on the provided description.
    /// The window will be displayed if successfully created.
    /// @param[in]  window_class - The class describing the type of window to create.
    /// @param[in]  window_title - The text to display in the window's title bar.
    /// @param[in]  width_in_pixels - The width of the client area within the window, in pixels.
    /// @param[in]  height_in_pixels - The height of the client area within the window, in pixels.
    /// @return     The window if successfully created; null otherwise.
    std::unique_ptr<Win32Window> Win32Window::Create(
        const WNDCLASSEXA& window_class,
        const std::string& window_title,
        const int width_in_pixels,
        const int height_in_pixels)
    {
        // REGISTER THE WINDOW CLASS SO IT CAN BE USED.
        ATOM window_class_id = RegisterClassEx(&window_class);
        const ATOM FAILED_WINDOW_CLASS_ID = 0;
        bool registration_successful = (FAILED_WINDOW_CLASS_ID != window_class_id);
        if (!registration_successful)
        {
            return nullptr;
        }

        // CALCULATE THE CLIENT RECTANGLE FOR THE WINDOW.
        RECT client_rectangle;
        client_rectangle.left = 0;
        client_rectangle.top = 0;
        client_rectangle.right = width_in_pixels;
        client_rectangle.bottom = height_in_pixels;
        BOOL result = AdjustWindowRect(&client_rectangle, WS_OVERLAPPEDWINDOW | WS_VISIBLE, FALSE);
        &result;
        /// @todo   Error-handling.
        // Note that due to DPI scaling, this will commonly result in a window that is actually 125%
        // bigger unless a user has adjusted his/her DPI scaling to be smaller than Windows' default.
        int window_with_borders_width_in_pixels = client_rectangle.right - client_rectangle.left;
        int window_with_borders_height_in_pixels = client_rectangle.bottom - client_rectangle.top;

        // CREATE THE WINDOW.
        const DWORD NO_EXTENDED_STYLE = 0;
        const DWORD WINDOW_STYLE = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
        const int DEFAULT_LEFT_X_POSITION = CW_USEDEFAULT;
        const int DEFAULT_TOP_Y_POSITION = CW_USEDEFAULT;
        const HWND NO_PARENT_WINDOW = NULL;
        const HMENU NO_WINDOW_MENU = NULL;
        const LPVOID NO_ADDITIONAL_CREATE_PARAMETERS = NULL;
        HWND window_handle = CreateWindowEx(
            NO_EXTENDED_STYLE,
            window_class.lpszClassName,
            window_title.c_str(),
            WINDOW_STYLE,
            DEFAULT_LEFT_X_POSITION,
            DEFAULT_TOP_Y_POSITION,
            window_with_borders_width_in_pixels,
            window_with_borders_height_in_pixels,
            NO_PARENT_WINDOW,
            NO_WINDOW_MENU,
            window_class.hInstance,
            NO_ADDITIONAL_CREATE_PARAMETERS);
        bool window_created = (NULL != window_handle);
        if (!window_created)
        {
            return nullptr;
        }

        // WRAP THE WINDOW HANDLE IN THE WINDOW OBJECT.
        std::unique_ptr<Win32Window> window = std::make_unique<Win32Window>(window_handle);
        return window;
    }

    /// Constructor.
    /// @param[in]  window_handle - The operating system handle to the window
    ///     wrapped by this class.
    Win32Window::Win32Window(const HWND window_handle) :
        WindowHandle(window_handle)
    {}

    /// Displays the provided render target in the window.
    /// @param[in]  render_target - The render target to display.
    void Win32Window::Display(const GRAPHICS::RenderTarget& render_target)
    {
        // GET THE DEVICE CONTEXT.
        HDC device_context = GetDC(WindowHandle);
        bool device_context_retrieved = (nullptr != device_context);
        if (!device_context_retrieved)
        {
            // The render target can't be copied without a valid device context.
            return;
        }

        // POPULATE THE BITMAP INFO DESCRIBING THE RENDER TARGET.
        BITMAPINFO bitmap_info = {};
        bitmap_info.bmiHeader.biSize = sizeof(bitmap_info.bmiHeader);
        int render_target_width = render_target.GetWidthInPixels();
        bitmap_info.bmiHeader.biWidth = static_cast<LONG>(render_target_width);
        // To ensure that the bitmap for the render target has an origin
        // at the top-left corner, the height needs to be made negative
        // to ensure the device independent bitmap is top-down.
        int render_target_height = render_target.GetHeightInPixels();
        bitmap_info.bmiHeader.biHeight = -1 * static_cast<LONG>(render_target_height);
        // The number of planes must always be 1.
        bitmap_info.bmiHeader.biPlanes = 1;
        // The pixel format is a 32-bit RGB format.
        bitmap_info.bmiHeader.biBitCount = 32;
        bitmap_info.bmiHeader.biCompression = BI_RGB;
        // BI_RGB bitmaps can set the image size to 0.
        bitmap_info.bmiHeader.biSizeImage = 0;
        // Meter-based information (used for devices like printing) is not
        // known or needed.
        bitmap_info.bmiHeader.biXPelsPerMeter = 0;
        bitmap_info.bmiHeader.biYPelsPerMeter = 0;
        // Require all colors be supported by the bitmap.
        bitmap_info.bmiHeader.biClrUsed = 0;
        bitmap_info.bmiHeader.biClrImportant = 0;

        // COPY AND SCALE THE RENDER TARGET DATA TO FILL THE ENTIRE CLIENT AREA OF THE WINDOW.
        RECT client_rectangle = {};
        GetClientRect(WindowHandle, &client_rectangle);
        constexpr int RENDER_TARGET_LEFT_X_POSITION = 0;
        constexpr int RENDER_TARGET_TOP_Y_POSITION = 0;
        const uint32_t* pixel_data = render_target.GetRawData();
        // The return value is ignored since there isn't much that can be meaningfully done
        // if copying fails.
        StretchDIBits(
            device_context,
            /// @todo   Perform proper scaling later.  Right now, the pixels
            /// are rendered to scale to make debugging easier.
            client_rectangle.left,
            client_rectangle.top,
            client_rectangle.right,
            client_rectangle.bottom,
            /*0,
            0,
            render_target_width,
            render_target_height,*/
            RENDER_TARGET_LEFT_X_POSITION,
            RENDER_TARGET_TOP_Y_POSITION,
            render_target_width,
            render_target_height,
            pixel_data,
            &bitmap_info,
            DIB_RGB_COLORS,
            SRCCOPY);

        // RELEASE THE DEVICE CONTEXT.
        ReleaseDC(WindowHandle, device_context);
    }
}