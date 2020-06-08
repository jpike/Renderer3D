#pragma once

#include "Graphics/Color.h"
#include "Graphics/RenderTarget.h"
#include "MATH/Vector2.h"

namespace GRAPHICS::GUI
{
    /// A glyph is a graphical representation of a single character within a font.
    class Glyph
    {
    public:
        // PUBLIC METHODS.
        Color GetPixelColor(const unsigned int local_x, const unsigned int local_y) const;

        // PUBLIC MEMBER VARIABLES FOR EASY ACCESS.
        /// @todo   Make some member variables private?
        /// The width of the glyph, in pixels.
        unsigned int WidthInPixels = 0;
        /// The height of the glyph, in pixels.
        unsigned int HeightInPixels = 0;
        /// The left/top offset within the entire font pixels where this glyph starts.
        MATH::Vector2ui LeftTopOffsetInFontPixels = MATH::Vector2ui(0, 0);
        /// The entire set of pixels for the from from which this glyph came.
        const RenderTarget* FontPixels = nullptr;
    };
}
