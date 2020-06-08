#include "Graphics/Gui/Glyph.h"

namespace GRAPHICS::GUI
{
    /// Gets the color of the specified pixel, if in range.
    /// If the specified coordinates are out-of-range, then an "invisible" color is returned.
    /// @param[in]  local_x - The local x coordinate of the pixel to get (in range of [0, glyph width - 1]).
    /// @param[in]  local_y - The local y coordinate of the pixel to get (in range of [0, glyph height - 1]).
    /// @return The color of the glyph at the specified pixel, if in range; an invisible color otherwise.
    Color Glyph::GetPixelColor(const unsigned int local_x, const unsigned int local_y) const
    {
        // MAKE SURE FONT PIXELS EXIST.
        /// @todo   Make GetPixel() return invisible color?
        const GRAPHICS::Color INVISIBLE_COLOR(0.0f, 0.0f, 0.0f, 0.0f);
        if (!FontPixels)
        {
            return INVISIBLE_COLOR;
        }

        // GET THE SPECIFIED PIXEL COLOR.
        unsigned int glyph_x_within_font = LeftTopOffsetInFontPixels.X + local_x;
        unsigned int glyph_y_within_font = LeftTopOffsetInFontPixels.Y + local_y;
        Color pixel_color = FontPixels->GetPixel(glyph_x_within_font, glyph_y_within_font);
        return pixel_color;
    }
}
