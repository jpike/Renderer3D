#include "Graphics/Color.h"
#include "Math/Number.h"

namespace GRAPHICS
{
    const Color Color::BLACK(0.0f, 0.0f, 0.0f, 1.0f);
    const Color Color::RED(1.0f, 0.0f, 0.0f, 1.0f);
    const Color Color::GREEN(0.0f, 1.0f, 0.0f, 1.0f);
    const Color Color::BLUE(0.0f, 0.0f, 1.0f, 1.0f);

    /// Unpacks a color from a packed color format.
    /// @param[in]  packed_color - The color to unpack.
    /// @param[in]  color_format - The format of data in the packed color.
    /// @return The unpacked color.
    Color Color::Unpack(const uint32_t packed_color, const ColorFormat color_format)
    {
        // UNPACK ACCORDING TO THE COLOR FORMAT.
        switch (color_format)
        {
            case ColorFormat::RGBA:
            {
                // UNPACK THE COLOR.
                uint8_t red = static_cast<uint8_t>((packed_color & 0xFF000000) >> 24);
                uint8_t green = static_cast<uint8_t>((packed_color & 0x00FF0000) >> 16);
                uint8_t blue = static_cast<uint8_t>((packed_color & 0x0000FF00) >> 8);
                uint8_t alpha = static_cast<uint8_t>(packed_color & 0x000000FF);
                Color unpacked_color(red, green, blue, alpha);
                return unpacked_color;
            }
            case ColorFormat::ARGB:
            {
                // UNPACK THE COLOR.
                uint8_t alpha = static_cast<uint8_t>((packed_color & 0xFF000000) >> 24);
                uint8_t red = static_cast<uint8_t>((packed_color & 0x00FF0000) >> 16);
                uint8_t green = static_cast<uint8_t>((packed_color & 0x0000FF00) >> 8);
                uint8_t blue = static_cast<uint8_t>(packed_color & 0x000000FF);
                Color unpacked_color(red, green, blue, alpha);
                return unpacked_color;
            }
            default:
                // RETURN A DEFAULT COLOR.
                return Color::BLACK;
        }
    }

    /// Scales the color by the specified factor, performing clamping.
    /// Only red, green, and blue components are scaled (alpha is left alone).
    /// @param[in]  scale_factor - The scaling factor to multiply the color by.
    /// @param[in]  color - The original color to scale.
    /// @return A copy of the color scaled as specified.
    Color Color::ScaleRedGreenBlue(const float scale_factor, const Color& color)
    {
        // COPY THE COLOR FOR MODIFICATIONS.
        Color scaled_color = color;

        // SCALE THE COLOR COMPONENTS.
        scaled_color.Red *= scale_factor;
        scaled_color.Green *= scale_factor;
        scaled_color.Blue *= scale_factor;
        
        // Clamping is performed to keep them within the valid range.
        scaled_color.Clamp();

        // RETURN THE SCALED COLOR.
        return scaled_color;
    }

    /// Performs component-wise multiplication of the colors (excluding alpha components),
    /// performing clamping.
    /// @param[in]  color_1 - One color to multiply by.
    /// @param[in]  color_2 - The other color to multiply by.
    /// @return The component-wise multiplied color.
    Color Color::ComponentMultiplyRedGreenBlue(const Color& color_1, const Color& color_2)
    {
        Color multiplied_color = Color::BLACK;

        // MULTIPLY THE COLOR COMPONENTS.
        multiplied_color.Red = color_1.Red * color_2.Red;
        multiplied_color.Green = color_1.Green * color_2.Green;
        multiplied_color.Blue = color_1.Blue * color_2.Blue;

        // Clamping is performed to keep them within the valid range.
        multiplied_color.Clamp();

        return multiplied_color;
    }

    /// Interpolates between 2 colors (excluding alpha components), performing clamping.
    /// @param[in]  start_color - The starting color to interpolate from (ratio = 0).
    /// @param[in]  end_color - The ending color to interpolate to (ratio = 1).
    /// @return The interpolated color.
    Color Color::InterpolateRedGreenBlue(const Color& start_color, const Color& end_color, float ratio_toward_end)
    {
        Color interpolated_color = start_color;

        float ratio_of_start = (1.0f - ratio_toward_end);

        // COMPUTE THE INTERPOLATED COLOR COMPONENTS.
        interpolated_color.Red = (start_color.Red * ratio_of_start) + (end_color.Red * ratio_toward_end);
        interpolated_color.Green = (start_color.Green * ratio_of_start) + (end_color.Green * ratio_toward_end);
        interpolated_color.Blue = (start_color.Blue * ratio_of_start) + (end_color.Blue * ratio_toward_end);

        // Clamping is performed to keep them within the valid range.
        interpolated_color.Clamp();

        return interpolated_color;
    }

    /// Constructor taking floating-point components.
    /// @param[in]  red - The red component of the color.
    /// @param[in]  green - The green component of the color.
    /// @param[in]  blue - The blue component of the color.
    /// @param[in]  alpha - The alpha component of the color.
    Color::Color(
        const float red,
        const float green,
        const float blue,
        const float alpha) :
        Red(red),
        Green(green),
        Blue(blue),
        Alpha(alpha)
    {}

    /// Constructor taking integer components.
    /// @param[in]  red - The red component of the color.
    /// @param[in]  green - The green component of the color.
    /// @param[in]  blue - The blue component of the color.
    /// @param[in]  alpha - The alpha component of the color.
    Color::Color(
        const uint8_t red,
        const uint8_t green,
        const uint8_t blue,
        const uint8_t alpha) :
        Red(0.0f),
        Green(0.0f),
        Blue(0.0f),
        Alpha(0.0f)
    {
        // CONVERT THE INTEGER COLORS TO FLOATING-POINT.
        Red = ToFloatColorComponent(red);
        Green = ToFloatColorComponent(green);
        Blue = ToFloatColorComponent(blue);
        Alpha = ToFloatColorComponent(alpha);
    }

    /// Determines if two colors are equal.
    /// @param[in]  rhs - The color to compare with.
    /// @return True if the provided color equals this color; false otherwise.
    bool Color::operator==(const Color& rhs) const
    {
        // COMPARE USING INTEGER FORMAT TO AVOID FLOATING-POINT ROUNDING ISSUES.
        // Early returns are used for each color component to avoid unnecessary
        // comparisons if unequal components are found.
        bool red_equal = (this->GetRedAsUint8() == rhs.GetRedAsUint8());
        if (!red_equal) return false;

        bool green_equal = (this->GetGreenAsUint8() == rhs.GetGreenAsUint8());
        if (!green_equal) return false;

        bool blue_equal = (this->GetBlueAsUint8() == rhs.GetBlueAsUint8());
        if (!blue_equal) return false;

        bool alpha_equal = (this->GetAlphaAsUint8() == rhs.GetAlphaAsUint8());
        if (!alpha_equal) return false;

        // All color components were equal.
        return true;
    }

    /// Adds a color's components to this color.
    /// Clamping is performed.
    /// @param[in]  rhs - The color to add to this color.
    /// @return The updated color after addition.
    Color& Color::operator+=(const Color& rhs)
    {
        // ADD THE COLOR COMPONENTS.
        Red += rhs.Red;
        Green += rhs.Green;
        Blue += rhs.Blue;
        Alpha += rhs.Alpha;

        // Clamping is performed to keep them within the valid range,
        // which is particularly important to avoid weird issues with the alpha channel.
        Clamp();

        // RETURN THE UPDATED COLOR.
        return (*this);
    }

    /// Returns the red component in 32-bit floating-point format.
    /// @return The red component as a float.
    float Color::GetRedAsFloat32() const
    {
        return Red;
    }

    /// Returns the green component in 32-bit floating-point format.
    /// @return The green component as a float.
    float Color::GetGreenAsFloat32() const
    {
        return Green;
    }

    /// Returns the blue component in 32-bit floating-point format.
    /// @return The blue component as a float.
    float Color::GetBlueAsFloat32() const
    {
        return Blue;
    }

    /// Returns the alpha component in 32-bit floating-point format.
    /// @return The alpha component as a float.
    float Color::GetAlphaAsFloat32() const
    {
        return Alpha;
    }

    /// Returns the red component in 8-bit integral format.
    /// @return The red component as a 8-bit integer.
    uint8_t Color::GetRedAsUint8() const
    {
        uint8_t red_as_uint8 = ToIntegerColorComponent(Red);
        return red_as_uint8;
    }

    /// Returns the green component in 8-bit integral format.
    /// @return The green component as a 8-bit integer.
    uint8_t Color::GetGreenAsUint8() const
    {
        uint8_t green_as_uint8 = ToIntegerColorComponent(Green);
        return green_as_uint8;
    }

    /// Returns the blue component in 8-bit integral format.
    /// @return The blue component as a 8-bit integer.
    uint8_t Color::GetBlueAsUint8() const
    {
        uint8_t blue_as_uint8 = ToIntegerColorComponent(Blue);
        return blue_as_uint8;
    }

    /// Returns the alpha component in 8-bit integral format.
    /// @return The alpha component as a 8-bit integer.
    uint8_t Color::GetAlphaAsUint8() const
    {
        uint8_t alpha_as_uint8 = ToIntegerColorComponent(Alpha);
        return alpha_as_uint8;
    }

    uint32_t Color::Pack(const ColorFormat color_format) const
    {
        // GET THE COLOR COMPONENTS.
        uint8_t red = GetRedAsUint8();
        uint8_t green = GetGreenAsUint8();
        uint8_t blue = GetBlueAsUint8();
        uint8_t alpha = GetRedAsUint8();

        // PACK ACCORDING TO THE COLOR FORMAT.
        switch (color_format)
        {
            case ColorFormat::RGBA:
            {
                // PACK THE COLOR.
                uint32_t packed_color =
                    (red << 24) |
                    (green << 16) |
                    (blue << 8) |
                    alpha;
                return packed_color;
            }
            case ColorFormat::ARGB:
            {
                // PACK THE COLOR.
                uint32_t packed_color =
                    (alpha << 24) |
                    (red << 16) |
                    (green << 8) |
                    blue;
                return packed_color;
            }
            default:
                // RETURN A DEFAULT COLOR.
                return 0x00000000;
        }
    }

    /// Clamps all color components to the valid range,
    /// which needs to be done after many operations.
    void Color::Clamp()
    {
        Red = MATH::Number::Clamp(Red, MIN_FLOAT_COLOR_COMPONENT, MAX_FLOAT_COLOR_COMPONENT);
        Green = MATH::Number::Clamp(Green, MIN_FLOAT_COLOR_COMPONENT, MAX_FLOAT_COLOR_COMPONENT);
        Blue = MATH::Number::Clamp(Blue, MIN_FLOAT_COLOR_COMPONENT, MAX_FLOAT_COLOR_COMPONENT);
        Alpha = MATH::Number::Clamp(Alpha, MIN_FLOAT_COLOR_COMPONENT, MAX_FLOAT_COLOR_COMPONENT);
    }

    /// Converts an 8-bit integral color component to floating-point format.
    /// @param[in]  color_component_as_uint8 - The color component to convert.
    /// @return The floating-point version of the color component.
    float Color::ToFloatColorComponent(const uint8_t color_component_as_uint8) const
    {
        // CAST THE INTEGRAL COMPONENT TO FLOATING-POINT TO AVOID TRUNCATION.
        float original_color_component = static_cast<float>(color_component_as_uint8);

        // SCALE THE COLOR COMPONENT FROM RANGE [0,255] to [0,1].
        float color_component_as_float = (original_color_component / MAX_INTEGRAL_COLOR_COMPONENT);

        return color_component_as_float;
    }

    /// Converts a 32-bit color component to 8-bit integral format.
    /// @param[in]  color_component_as_float - The color component to convert.
    /// @return The 8-bit integer version of the color component.
    uint8_t Color::ToIntegerColorComponent(const float color_component_as_float) const
    {
        // SCALE THE COLOR COMPONENT FROM RANGE [0,1] TO [0,255].
        float scaled_color_component = (color_component_as_float * MAX_INTEGRAL_COLOR_COMPONENT);

        // CAST THE SCALED COLOR COMPONENT TO AN 8-BIT INTEGER.
        uint8_t color_component_as_uint8 = static_cast<uint8_t>(scaled_color_component);
        return color_component_as_uint8;
    }
}
