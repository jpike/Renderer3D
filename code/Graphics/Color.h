#pragma once

#include <cstdint>
#include "Graphics/ColorFormat.h"

namespace GRAPHICS
{
    /// An RGBA color.  Can be represented either with floating-point components
    /// between [0,1] or 8-bit integral components between [0,255].
    class Color
    {
    public:
        // STATIC CONSTANTS.
        /// The maximum integral color component, expressed as a float for easy conversion.
        static constexpr float MAX_INTEGRAL_COLOR_COMPONENT = 255.0f;
        /// The minimum floating-point color component.
        static constexpr float MIN_FLOAT_COLOR_COMPONENT = 0.0f;
        /// The maximum floating-point color component.
        static constexpr float MAX_FLOAT_COLOR_COMPONENT = 1.0f;
        /// The color black.
        static const Color BLACK;
        /// The color red.
        static const Color RED;
        /// The color green.
        static const Color GREEN;
        /// The color blue.
        static const Color BLUE;

        // CONSTRUCTION.
        static Color Unpack(const uint32_t packed_color, const ColorFormat color_format);
        static Color ScaleRedGreenBlue(const float scale_factor, const Color& color);
        static Color ComponentMultiplyRedGreenBlue(const Color& color_1, const Color& color_2);
        static Color InterpolateRedGreenBlue(const Color& start_color, const Color& end_color, float ratio_toward_end);
        explicit Color(
            const float red,
            const float green,
            const float blue,
            const float alpha);
        explicit Color(
            const uint8_t red,
            const uint8_t green,
            const uint8_t blue,
            const uint8_t alpha);

        // OPERATORS.
        bool operator==(const Color& rhs) const;
        Color& operator+=(const Color& rhs);

        // FLOATING-POINT ACCESS.
        float GetRedAsFloat32() const;
        float GetGreenAsFloat32() const;
        float GetBlueAsFloat32() const;
        float GetAlphaAsFloat32() const;

        // INTEGRAL ACCESS.
        uint8_t GetRedAsUint8() const;
        uint8_t GetGreenAsUint8() const;
        uint8_t GetBlueAsUint8() const;
        uint8_t GetAlphaAsUint8() const;

        // PACKING.
        uint32_t Pack(const ColorFormat color_format) const;

        // CLAMPING.
        void Clamp();

        // PUBLIC MEMBER VARIABLES FOR EASY ACCESS.
        /// The red component of the color.
        float Red;
        /// The blue component of the color.
        float Blue;
        /// The green component of the color.
        float Green;
        /// The alpha component of the color.
        float Alpha;

    private:
        // HELPER METHODS.
        float ToFloatColorComponent(const uint8_t color_component_as_uint8) const;
        uint8_t ToIntegerColorComponent(const float color_component_as_float) const;
    };
}
