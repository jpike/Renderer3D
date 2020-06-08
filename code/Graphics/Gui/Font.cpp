#include "Graphics/Gui/Font.h"

namespace GRAPHICS::GUI
{
    /// Loads the system's default fixed-width font.
    /// @return The system's default fixed-width font, if successfully loaded;
    ///     std::nullopt otherwise.
    std::shared_ptr<Font> Font::LoadSystemDefaultFixedFont()
    {
        /// @todo   Error handling for return values?  Thus far, hasn't been too important...

        // GET A DEVICE CONTEXT IN WHICH TO LOAD THE FONT.
        HDC entire_screen_device_context = GetDC(NULL);
        HDC font_device_context = CreateCompatibleDC(entire_screen_device_context);

        // CREATE A BITMAP FOR RENDERING THE FONT.
        // This is necessary to be able to successfully write and retrieve pixels.
        BITMAPINFO bitmap_info = {};
        bitmap_info.bmiHeader.biSize = sizeof(bitmap_info.bmiHeader);
        bitmap_info.bmiHeader.biWidth = GLYPH_BITMAP_DIMENSION_IN_PIXELS;
        // Height is negative to use a typical top-down bitmap.
        bitmap_info.bmiHeader.biHeight = -static_cast<long>(GLYPH_BITMAP_DIMENSION_IN_PIXELS);
        // Must be 1 according to Microsoft documentation.
        bitmap_info.bmiHeader.biPlanes = 1;
        // We use 32-bit color.
        bitmap_info.bmiHeader.biBitCount = 32;
        // Uncompressed, raw RGB colors are used.
        bitmap_info.bmiHeader.biCompression = BI_RGB;
        // Microsoft documentation say this "may be 0" for RGB bitmaps.
        bitmap_info.bmiHeader.biSizeImage = 0;
        // 0 indicates using the maximum number of colors.
        bitmap_info.bmiHeader.biClrUsed = 0;
        // 0 indicates all colors are required.
        bitmap_info.bmiHeader.biClrImportant = 0;

        // We don't actually need these raw bit values, but this parameter is required for the function below.
        void* font_bits_unused = nullptr;
        constexpr HANDLE ALLOCATE_MEMORY_RATHER_THAN_USE_FILE_MAPPING = nullptr;
        constexpr DWORD NO_FILE_MAPPING_OFFSET = 0;
        HBITMAP font_bitmap = CreateDIBSection(
            font_device_context, 
            &bitmap_info, 
            DIB_RGB_COLORS, 
            &font_bits_unused,
            ALLOCATE_MEMORY_RATHER_THAN_USE_FILE_MAPPING,
            NO_FILE_MAPPING_OFFSET);
        SelectObject(font_device_context, font_bitmap);

        // SET A BACKGROUND COLOR FOR THE BITMAP.
        // Black is arbitrarily chosen for now.
        const COLORREF BACKGROUND_COLOR = RGB(0, 0, 0);
        SetBkColor(font_device_context, BACKGROUND_COLOR);

        // GET A HANDLE TO THE SYSTEM FIXED FONT.
        std::shared_ptr<Font> system_fixed_font = std::make_shared<Font>();
        HGDIOBJ font_handle = GetStockObject(SYSTEM_FIXED_FONT);

        // ENABLE USING THE FONT.
        SelectObject(font_device_context, font_handle);

        // GET THE BASIC TEXT METRICS FOR THE FONT.
        TEXTMETRIC text_metrics;
        GetTextMetrics(font_device_context, &text_metrics);
        long glyph_width = text_metrics.tmMaxCharWidth;
        long glyph_height = text_metrics.tmHeight;

        // RENDER ALL GLYPHS FOR THE FONT.
        // White is used as a contrast that allows easily altering colors.
        const COLORREF FONT_COLOR = RGB(255, 255, 255);
        SetTextColor(font_device_context, FONT_COLOR);
        RECT current_glyph_rectangle =
        {
            .left = 0,
            .top = 0,
            .right = glyph_width,
            .bottom = glyph_height
        };
        // Unsigned ints are used to avoid infinite wraparound that would occur if using chars directly.
        for (unsigned int character_value = 0; character_value < CHARACTER_COUNT; ++character_value)
        {
            // DRAW THE CURRENT CHARACTER.
            constexpr int SINGLE_CHARACTER_COUNT = 1;
            char character = static_cast<char>(character_value);
            DrawText(font_device_context, &character, SINGLE_CHARACTER_COUNT, &current_glyph_rectangle, DT_CENTER);

            // GET ALL PIXELS FOR THE CURRENT GLYPH.
            for (int y = current_glyph_rectangle.top; y <= current_glyph_rectangle.bottom; ++y)
            {
                for (int x = current_glyph_rectangle.left; x <= current_glyph_rectangle.right; ++x)
                {
                    // ONLY STORE THE PIXEL COLOR IF SOMETHING WAS DRAWN.
                    // If the pixel wasn't written at all, the result will be all zeroes.
                    // Since it only contains RGB colors, that means the alpha will also be 0.
                    COLORREF pixel_color = GetPixel(font_device_context, x, y);
                    if (pixel_color)
                    {
                        // The COLORREF structure by default is only an RGB color,
                        // so to ensure that proper alpha is used, it must be added in.
                        constexpr DWORD ADD_IN_ALPHA_FOR_OPAQUENESS = 0xFF000000;
                        pixel_color |= ADD_IN_ALPHA_FOR_OPAQUENESS;
                        system_fixed_font->Pixels.WritePixel(x, y, pixel_color);
                    }
                }
            }

            // STORE THE GLYPH FOR THE CURRENT CHARACTER.
            system_fixed_font->GlyphsByCharacter[static_cast<unsigned char>(character)] = Glyph
            {
                .WidthInPixels = static_cast<unsigned int>(glyph_width),
                .HeightInPixels = static_cast<unsigned int>(glyph_height),
                .LeftTopOffsetInFontPixels = MATH::Vector2ui(current_glyph_rectangle.left, current_glyph_rectangle.top),
                .FontPixels = &system_fixed_font->Pixels
            };

            // MOVE TO THE NEXT CHARACTER.
            current_glyph_rectangle.left += glyph_width;
            current_glyph_rectangle.right += glyph_width;
            bool current_row_of_glyphs_filled = current_glyph_rectangle.right >= GLYPH_BITMAP_DIMENSION_IN_PIXELS;
            if (current_row_of_glyphs_filled)
            {
                // MOVE TO THE NEXT ROW FOR DRAWING.
                current_glyph_rectangle.left = 0;
                current_glyph_rectangle.right = glyph_width;
                current_glyph_rectangle.top += glyph_height;
                current_glyph_rectangle.bottom += glyph_height;
            }
        }

        // FREE ALLOCATED RESOURCES.
        DeleteObject(font_bitmap);
        DeleteDC(font_device_context);

        // RETURN THE FONT.
        return system_fixed_font;
    }
}
