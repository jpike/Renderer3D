#pragma once

#include <array>
#include <memory>
#include <Windows.h>
#include "Graphics/Gui/Glyph.h"
#include "Graphics/RenderTarget.h"

/// Holds code related to standard GUIs (graphical user interfaces - https://en.wikipedia.org/wiki/Graphical_user_interface).
namespace GRAPHICS::GUI
{
    /// A font is a particular styling of characters that can be used for rendering text.
    /// Each character is represented by a glyph within the font.
    /// 
    /// This class currently only supports bitmap fonts for ASCII characters 0-255.
    /// A font's bitmap is expected to have 16 rows and 16 columns of glyphs.
    /// The top-left glyph should be for ASCII character 0, and the bottom-right
    /// glyph should be for ASCII character 255.  Each glyph must be represented
    /// in a 16x16 pixel rectangle.
    class Font
    {
    public:
        // STATIC CONSTANTS.
        /// The total number of characters supported by the font.
        static constexpr unsigned int CHARACTER_COUNT = 256;
        /// The dimension (width or height) of a single glyph, in pixels.
        static constexpr unsigned int GLYPH_DIMENSION_IN_PIXELS = 16;
        /// The number of glyphs in a single row or column of the font's bitmap.
        static constexpr unsigned int GLYPH_COUNT_PER_ROW_OR_COLUMN = 16;
        /// The dimension (width or height) of the glyph bitmap, in pixels.
        static constexpr unsigned int GLYPH_BITMAP_DIMENSION_IN_PIXELS = GLYPH_DIMENSION_IN_PIXELS * GLYPH_COUNT_PER_ROW_OR_COLUMN;

        // CONSTRUCTION.
        static std::shared_ptr<Font> LoadSystemDefaultFixedFont();

        // MEMBER VARIABLES.
        /// The glyphs as stored by regular unsigned ASCII character value.
        std::array<Glyph, CHARACTER_COUNT> GlyphsByCharacter = {};
        /// The pixels for the font.
        /// The color format is chosen to be compatible with the format used by Windows by default.
        RenderTarget Pixels = RenderTarget(GLYPH_BITMAP_DIMENSION_IN_PIXELS, GLYPH_BITMAP_DIMENSION_IN_PIXELS, ColorFormat::ARGB);
    };
}
