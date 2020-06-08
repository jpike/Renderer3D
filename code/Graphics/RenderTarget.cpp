#include "Graphics/RenderTarget.h"

namespace GRAPHICS
{
    /// Constructor.
    /// @param[in]  width_in_pixels - The width of the render target.
    /// @param[in]  height_in_pixels - The height of the render target.
    /// @param[in]  color_format - The color format of pixels in the render target.
    RenderTarget::RenderTarget(
        const unsigned int width_in_pixels,
        const unsigned int height_in_pixels,
        const GRAPHICS::ColorFormat color_format) :
        WidthInPixels(width_in_pixels),
        HeightInPixels(height_in_pixels),
        ColorFormat(color_format),
        Pixels(width_in_pixels, height_in_pixels)
    {}

    /// Gets the width of the render target.
    /// @return The width in pixels.
    unsigned int RenderTarget::GetWidthInPixels() const
    {
        return WidthInPixels;
    }

    /// Gets the height of the render target.
    /// @return The height in pixels.
    unsigned int RenderTarget::GetHeightInPixels() const
    {
        return HeightInPixels;
    }

    /// Retrieves a pointer to the raw pixel data of the render target.
    /// @return A pointer to the raw pixel data.
    const uint32_t* RenderTarget::GetRawData() const
    {
        return Pixels.ValuesInRowMajorOrder();
    }

    /// Retrieves the pixel color at the specified coordinates.
    /// @param[in]  x - The horizontal coordinate of the pixel.
    /// @param[in]  y - The vertical coorindate of the pixel.
    GRAPHICS::Color RenderTarget::GetPixel(const unsigned int x, const unsigned int y) const
    {
        // RETURN A DEFAULT COLOR IF THE PIXEL COORDINATES AREN'T VALID.
        bool pixel_coordinates_valid = Pixels.IndicesInRange(x, y);
        if (!pixel_coordinates_valid)
        {            
            return GRAPHICS::Color::BLACK;
        }

        // RETURN THE COLOR.
        uint32_t packed_color = Pixels(x, y);
        GRAPHICS::Color color = GRAPHICS::Color::Unpack(packed_color, ColorFormat);
        return color;
    }

    /// Fills in color of the pixel at the specified coordinates.
    /// @param[in]  x - The horizontal coordinate of the pixel.
    /// @param[in]  y - The vertical coorindate of the pixel.
    /// @param[in]  color - The color to write to the pixel, already in 32-bit packed
    ///     format according to the color format specified for the render target.
    void RenderTarget::WritePixel(const unsigned int x, const unsigned int y, const uint32_t& color)
    {
        // MAKE SURE THE PIXEL COORDINATES ARE VALID.
        bool pixel_coordinates_valid = Pixels.IndicesInRange(x, y);
        if (!pixel_coordinates_valid)
        {
            // The pixel can't be written.
            return;
        }

        // FILL IN THE COLOR COMPONENTS OF THE PIXEL.
        Pixels(x, y) = color;
    }

    /// Fills in color of the pixel at the specified coordinates.
    /// @param[in]  x - The horizontal coordinate of the pixel.
    /// @param[in]  y - The vertical coorindate of the pixel.
    /// @param[in]  color - The color to write to the pixel.
    void RenderTarget::WritePixel(const unsigned int x, const unsigned int y, const Color& color)
    {
        // MAKE SURE THE PIXEL COORDINATES ARE VALID.
        bool pixel_coordinates_valid = Pixels.IndicesInRange(x, y);
        if (!pixel_coordinates_valid)
        {
            // The pixel can't be written.
            return;
        }

        // FILL IN THE COLOR COMPONENTS OF THE PIXEL.
        uint32_t packed_color = color.Pack(ColorFormat);
        Pixels(x, y) = packed_color;
    }

    /// Fills all pixels in the render target with the specified color.
    /// @param[in]  color - The color to fill all pixels.
    void RenderTarget::FillPixels(const Color& color)
    {
        // FILL IN ALL PIXELS.
        for (unsigned int y = 0; y < HeightInPixels; ++y)
        {
            for (unsigned int x = 0; x < WidthInPixels; ++x)
            {
                WritePixel(x, y, color);
            }
        }
    }
}
