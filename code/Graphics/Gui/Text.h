#pragma once

#include <string>
#include "Graphics/Gui/Font.h"
#include "Math/Vector2.h"

namespace GRAPHICS::GUI
{
    /// Text that can be rendered on a graphical GUI.
    /// Units for this class are in screen/pixel units.
    class Text
    {
    public:
        /// The string of characters for the text.
        std::string String = "";
        /// The font to use for rendering the text.
        Font* Font = nullptr;
        /// The left, top (x, y) screen position at which the text should be rendered.
        MATH::Vector2f LeftTopPosition = MATH::Vector2f(0.0f, 0.0f);
    };
}
