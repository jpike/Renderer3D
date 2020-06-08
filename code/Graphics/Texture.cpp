#include <fstream>
#include <Windows.h>
#include "Graphics/Texture.h"

namespace GRAPHICS
{
    /// Attempts to load the texture from the specified filepath.
    /// @param[in]  filepath - The path to the texture file to load.
    /// @return The texture, if loaded successfully; null otherwise.
    std::shared_ptr<Texture> Texture::Load(const std::filesystem::path& filepath)
    {
        // OPEN THE FILE.
        std::ifstream bitmap_file(filepath, std::ios::binary);
        bool bitmap_file_opened = bitmap_file.is_open();
        if (!bitmap_file_opened)
        {
            return nullptr;
        }

        // See https://en.wikipedia.org/wiki/BMP_file_format for the .bmp file format.
        // https://docs.microsoft.com/en-us/windows/win32/gdi/bitmap-storage

        // READ IN THE BITMAP FILE HEADER.
        BITMAPFILEHEADER bitmap_file_header = {};
        bitmap_file.read(reinterpret_cast<char*>(&bitmap_file_header), sizeof(bitmap_file_header));

        // READ IN THE BITMAP INFO HEADER.
        // This is assumed to be next based on our specific bitmaps, but it's not fully robust
        // against all bitmaps.
        BITMAPINFOHEADER bitmap_info_header = {};
        bitmap_file.read(reinterpret_cast<char*>(&bitmap_info_header), sizeof(bitmap_info_header));

        // CREATE THE EMPTY TEXTURE.
        auto texture = std::make_shared<Texture>(
            bitmap_info_header.biWidth,
            bitmap_info_header.biHeight,
            ColorFormat::RGBA); /// Seems to be RGB based on biCompression == 0

        // FILL IN ALL PIXELS OF THE TEXTURE.
        // When height is non-negative, then this is a bottom-up bitmap - hence why we
        // start at the last row of pixels.
        unsigned int current_pixel_x = 0;
        unsigned int current_pixel_y = bitmap_info_header.biHeight - 1;
        while (bitmap_file.good())
        {
            // READ IN THE CURRENT PIXEL'S COLOR COMPONENTS.
            // See https://docs.microsoft.com/en-us/previous-versions//dd183376(v=vs.85)?redirectedfrom=MSDN
            // about pixel color component ordering.
            uint8_t blue = 0;
            bitmap_file.read(reinterpret_cast<char*>(&blue), sizeof(blue));

            uint8_t green = 0;
            bitmap_file.read(reinterpret_cast<char*>(&green), sizeof(green));

            uint8_t red = 0;
            bitmap_file.read(reinterpret_cast<char*>(&red), sizeof(red));

            // If reading failed, then the above data isn't valid.
            if (!bitmap_file.good())
            {
                break;
            }

            // SET THE COLOR IN THE TEXTURE.
            constexpr uint8_t ALPHA_FOR_FULLY_OPAQUE = 1;
            Color color(red, green, blue, ALPHA_FOR_FULLY_OPAQUE);
            texture->Bitmap.WritePixel(current_pixel_x, current_pixel_y, color);

            // MOVE TO THE NEXT PIXEL.
            ++current_pixel_x;
            bool current_row_filled = (current_pixel_x >= static_cast<unsigned int>(bitmap_info_header.biWidth));
            if (current_row_filled)
            {
                // MOVE TO THE NEXT ROW.
                current_pixel_x = 0;
                --current_pixel_y;
            }
        }

        return texture;
    }

    Texture::Texture(
        const unsigned int width_in_pixels,
        const unsigned int height_in_pixels,
        const ColorFormat color_format) :
    Bitmap(width_in_pixels, height_in_pixels, color_format)
    {}
}
