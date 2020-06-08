#pragma once

namespace MATH
{

    /// Utilities for working with numbers.
    /// Designed with templates to allow working with multiple different data types,
    /// but methods may not work as expected for all data types.
    class Number
    {
    public:
        template <typename NumericType>
        static NumericType Clamp(
            const NumericType number_to_clamp,
            const NumericType min_value,
            const NumericType max_value);
    };

    /// Clamps a numeric value to a range.
    /// \tparam NumericType - The type of the value being clamped.
    /// \param[in]  number_to_clamp - The number to clamp.
    /// \param[in]  min_value - The minimum (inclusive) value of the range to clamp to.
    /// \param[in]  max_value - The maximum (inclusive) value of the range to clamp to.
    /// \return The provided number clamped to the specified range.
    template <typename NumericType>
    NumericType Number::Clamp(
        const NumericType number_to_clamp,
        const NumericType min_value,
        const NumericType max_value)
    {
        // CLAMP THE NUMBER TO THE THE MIN VALUE IF IT'S TOO LOW.
        bool number_too_low = (number_to_clamp < min_value);
        if (number_too_low)
        {
            return min_value;
        }

        // CLAMP THE NUMBER TO THE MAX VALUE IF IT'S TOO HIGH.
        bool number_too_high = (number_to_clamp > max_value);
        if (number_too_high)
        {
            return max_value;
        }

        // RETURN THE NUMBER SINCE IT'S ALREADY CLAMPED WITHIN THE RANGE.
        return number_to_clamp;
    }
}
