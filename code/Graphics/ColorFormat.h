#pragma once

namespace GRAPHICS
{
    /// An enumeration of supported color formats.
    /// Each value describes the order of color components
    /// from most significant byte to least significant byte
    /// in a 32-bit packed integer format.
    enum class ColorFormat
    {
        /// Color components are ordered from red, green, blue, alpha.
        RGBA = 0,
        /// Color components are ordered from alpha, red, green, blue.
        ARGB
    };
}
