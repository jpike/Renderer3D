#pragma once

namespace MATH
{
    /// A mathematical angle, which exists between two lines with a common endpoint.
    /// @tparam ValueType - The underlying type for which to store angle values.
    template <typename ValueType>
    class Angle
    {
    public:
        // NESTED TYPES.
        /// A nested type to represent an angle value in radians,
        /// providing some type-checking.
        struct Radians
        {
            // MEMBER VARIABLES.
            /// The angle value in radians.
            ValueType Value;

            // CONSTRUCTION.
            explicit Radians(const ValueType value);

            // OPERATORS.
            bool operator==(const Radians rhs) const;
            Radians operator+(const Radians rhs) const;
            Radians operator-(const Radians rhs) const;
            Radians operator*(const Radians rhs) const;
            Radians operator/(const Radians rhs) const;
        };

        /// A nested type to represent an angle value in degrees,
        /// providing some type-checking.
        struct Degrees
        {
            // MEMBER VARIABLES.
            /// The angle value in degrees.
            ValueType Value;

            // CONSTRUCTION.
            explicit Degrees(const ValueType value);

            // OPERATORS.
            bool operator==(const Degrees rhs) const;
            Degrees operator+(const Degrees rhs) const;
            Degrees operator-(const Degrees rhs) const;
            Degrees operator*(const Degrees rhs) const;
            Degrees operator/(const Degrees rhs) const;
        };

        // STATIC METHODS.
        static Radians DegreesToRadians(const Degrees degrees);
    };
    
    /// Constructor.
    /// @param[in]  value - The angle value, in radians.
    template <typename ValueType>
    Angle<ValueType>::Radians::Radians(const ValueType value) :
        Value(value)
    {}

    /// Determines if two radian values are equal.
    /// @param[in]  rhs - The radian value to compare with.
    /// @return True if this radian value is equal with the provided radian value; false otherwise.
    template <typename ValueType>
    bool Angle<ValueType>::Radians::operator==(const typename Angle<ValueType>::Radians rhs) const
    {
        bool radians_equal = (this->Value == rhs.Value);
        return radians_equal;
    }

    /// Adds two radian values together.
    /// @param[in]  rhs - The radian value to add to this radian value.
    /// @return The sum of the two radian values.
    template <typename ValueType>
    typename Angle<ValueType>::Radians Angle<ValueType>::Radians::operator+(const typename Angle<ValueType>::Radians rhs) const
    {
        Angle<ValueType>::Radians radian_sum = this->Value + rhs.Value;
        return radian_sum;
    }

    /// Subtracts a radian value from this radian value.
    /// @param[in]  rhs - The radian value to subtract from this radian value.
    /// @return The difference of the two radian values.
    template <typename ValueType>
    typename Angle<ValueType>::Radians Angle<ValueType>::Radians::operator-(const typename Angle<ValueType>::Radians rhs) const
    {
        Angle<ValueType>::Radians radian_difference = this->Value - rhs.Value;
        return radian_difference;
    }

    /// Multiplies two radian values together.
    /// @param[in]  rhs - The radian value to multiply by.
    /// @return The product of the two radian values.
    template <typename ValueType>
    typename Angle<ValueType>::Radians Angle<ValueType>::Radians::operator*(const typename Angle<ValueType>::Radians rhs) const
    {
        Angle<ValueType>::Radians radian_product = this->Value * rhs.Value;
        return radian_product;
    }

    /// Divides this radian value by another radian value.
    /// @param[in]  rhs - The radian value to divide by.
    /// @return The quotient of the two radian values.
    template <typename ValueType>
    typename Angle<ValueType>::Radians Angle<ValueType>::Radians::operator/(const typename Angle<ValueType>::Radians rhs) const
    {
        Angle<ValueType>::Radians radian_quotient = this->Value / rhs.Value;
        return radian_quotient;
    }

    /// Constructor.
    /// @param[in]  value - The angle value, in degrees.
    template <typename ValueType>
    Angle<ValueType>::Degrees::Degrees(const ValueType value) :
        Value(value)
    {}

    /// Determines if two degree values are equal.
    /// @param[in]  rhs - The degree value to compare with.
    /// @return True if this degree value is equal with the provided degree value; false otherwise.
    template <typename ValueType>
    bool Angle<ValueType>::Degrees::operator==(const typename Angle<ValueType>::Degrees rhs) const
    {
        bool degrees_equal = (this->Value == rhs.Value);
        return degrees_equal;
    }

    /// Adds two degree values together.
    /// @param[in]  rhs - The degree value to add to this degree value.
    /// @return The sum of the two degree values.
    template <typename ValueType>
    typename Angle<ValueType>::Degrees Angle<ValueType>::Degrees::operator+(const typename Angle<ValueType>::Degrees rhs) const
    {
        Angle<ValueType>::Degrees degree_sum = this->Value + rhs.Value;
        return degree_sum;
    }

    /// Subtracts a degree value from this degree value.
    /// @param[in]  rhs - The degree value to subtract from this degree value.
    /// @return The difference of the two degree values.
    template <typename ValueType>
    typename Angle<ValueType>::Degrees Angle<ValueType>::Degrees::operator-(const typename Angle<ValueType>::Degrees rhs) const
    {
        Angle<ValueType>::Degrees degree_difference = this->Value - rhs.Value;
        return degree_difference;
    }

    /// Multiplies two degree values together.
    /// @param[in]  rhs - The degree value to multiply by.
    /// @return The product of the two degree values.
    template <typename ValueType>
    typename Angle<ValueType>::Degrees Angle<ValueType>::Degrees::operator*(const typename Angle<ValueType>::Degrees rhs) const
    {
        Angle<ValueType>::Degrees degree_product = this->Value * rhs.Value;
        return degree_product;
    }

    /// Divides this degree value by another degree value.
    /// @param[in]  rhs - The degree value to divide by.
    /// @return The quotient of the two degree values.
    template <typename ValueType>
    typename Angle<ValueType>::Degrees Angle<ValueType>::Degrees::operator/(const typename Angle<ValueType>::Degrees rhs) const
    {
        Angle<ValueType>::Degrees degree_quotient = this->Value / rhs.Value;
        return degree_quotient;
    }

    /// Converts an angle value from degrees to radians.
    /// @param[in]  degrees - The angle value in degrees.
    /// @return The angle value in radians.
    template <typename ValueType>
    typename Angle<ValueType>::Radians Angle<ValueType>::DegreesToRadians(const typename Angle<ValueType>::Degrees degrees)
    {
        /// @todo   More precision for pi.
        const ValueType HALF_CIRCLE_IN_RADIANS = static_cast<ValueType>(3.14159);
        const ValueType HALF_CIRCLE_IN_DEGREES = static_cast<ValueType>(180);
        ValueType radians_value = degrees.Value * HALF_CIRCLE_IN_RADIANS / HALF_CIRCLE_IN_DEGREES;
        Radians radians(radians_value);
        return radians;
    }
}
