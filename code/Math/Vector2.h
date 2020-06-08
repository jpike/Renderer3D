#pragma once

#include <cmath>

/// Holds code related to math.
namespace MATH
{
    /// A 2D mathematical vector with both magnitude and direction.
    /// It currently only has the minimal functionality needed,
    /// so it cannot directly perform all common vector operations.
    ///
    /// The ComponentType template parameter is intended to be replaced with
    /// any numerical type that is typically used for vectors (int, float, etc.).
    template <typename ComponentType>
    class Vector2
    {
    public:
        // STATIC METHODS.
        static Vector2 Scale(const ComponentType scale_factor, const Vector2& vector);
        static Vector2 Normalize(const Vector2& vector);
        static ComponentType DotProduct(const Vector2& vector_1, const Vector2& vector_2);

        // CONSTRUCTION.
        explicit Vector2(const ComponentType x = 0, const ComponentType y = 0);

        // OPERATORS.
        bool operator== (const Vector2& rhs) const;
        bool operator!= (const Vector2& rhs) const;
        Vector2 operator+ (const Vector2& rhs) const;
        Vector2& operator+= (const Vector2& rhs);
        Vector2 operator- (const Vector2& rhs) const;

        // OTHER OPERATIONS.
        ComponentType Length() const;

        // PUBLIC MEMBER VARIABLES FOR EASY ACCESS.
        /// The x component of the vector.
        ComponentType X;
        /// The y component of the vector.
        ComponentType Y;
    };

    // DEFINE COMMON VECTOR2 TYPES.
    /// A vector composed of 2 unsigned integer components.
    typedef Vector2<unsigned int> Vector2ui;
    /// A vector composed of 2 float components.
    typedef Vector2<float> Vector2f;

    /// Computes a scaled version of a vector.
    /// @param[in]  scale_factor - The scale factor to multiply each component of the vector by.
    /// @param[in]  vector - The vector to scale.
    /// @return The scaled version of the vector.
    template <typename ComponentType>
    Vector2<ComponentType> Vector2<ComponentType>::Scale(const ComponentType scale_factor, const Vector2<ComponentType>& vector)
    {
        Vector2<ComponentType> scaled_vector;
        scaled_vector.X = scale_factor * vector.X;
        scaled_vector.Y = scale_factor * vector.Y;
        return scaled_vector;
    }

    /// Normalizes a vector to be unit length (length of 1).
    /// @param[in]  vector - The vector to normalize.
    /// @return The normalized version of the vector.
    ///     If the vector is a zero vector, then a zero vector is returned.
    template <typename ComponentType>
    Vector2<ComponentType> Vector2<ComponentType>::Normalize(const Vector2<ComponentType>& vector)
    {
        // GET THE VECTOR'S LENGTH.
        ComponentType vector_length = vector.Length();

        // RETURN A ZERO VECTOR IF THE VECTOR'S LENGTH IS ZERO.
        bool vector_length_is_zero = (0 == vector_length);
        if (vector_length_is_zero)
        {
            return Vector2<ComponentType>(0, 0);
        }

        // CREATE A NORMALIZED VERSION OF THE VECTOR.
        Vector2<ComponentType> normalized_vector;
        normalized_vector.X = vector.X / vector_length;
        normalized_vector.Y = vector.Y / vector_length;
        return normalized_vector;
    }

    /// Computes the dot product between 2 vectors.
    /// @param[in]  vector_1 - One vector to use in the dot product.
    /// @param[in]  vector_2 - Another vector to use in the dot product.
    /// @return The dot product between the 2 vectors.
    template <typename ComponentType>
    ComponentType Vector2<ComponentType>::DotProduct(
        const Vector2<ComponentType>& vector_1,
        const Vector2<ComponentType>& vector_2)
    {
        ComponentType dot_product =
            (vector_1.X * vector_2.X) +
            (vector_1.Y * vector_2.Y);
        return dot_product;
    }

    /// Constructor that accepts initial values.
    /// @param[in]  x - The x component value.
    /// @param[in]  y - The y component value.
    template <typename ComponentType>
    Vector2<ComponentType>::Vector2(const ComponentType x, const ComponentType y) :
        X(x),
        Y(y)
    {};

    /// Equality operator.  Direct equality comparison is used for components,
    /// so the precision of components types should be considered when using
    /// this operator.
    /// @param[in]  rhs - The vector on the right-hand side of the operator.
    /// @return True if the vectors are equal; false otherwise.
    template <typename ComponentType>
    bool Vector2<ComponentType>::operator== (const Vector2<ComponentType>& rhs) const
    {
        bool x_component_matches = (this->X == rhs.X);
        bool y_component_matches = (this->Y == rhs.Y);

        bool both_components_match = (x_component_matches && y_component_matches);
        return both_components_match;
    }

    /// Inequality operator.  Direct equality comparison is used for components,
    /// so the precision of components types should be considered when using
    /// this operator.
    /// @param[in]  rhs - The vector on the right-hand side of the operator.
    /// @return True if the vectors are unequal; false otherwise.
    template <typename ComponentType>
    bool Vector2<ComponentType>::operator!= (const Vector2<ComponentType>& rhs) const
    {
        bool vectors_equal = ((*this) == rhs);
        return !vectors_equal;
    }

    /// Addition operator.
    /// @param[in]  rhs - The vector on the right-hand side of the operator to
    ///     add to this vector.
    /// @return A new vector created by adding the provided vector to this vector.
    template <typename ComponentType>
    Vector2<ComponentType> Vector2<ComponentType>::operator+ (const Vector2<ComponentType>& rhs) const
    {
        MATH::Vector2f resulting_vector;
        resulting_vector.X = this->X + rhs.X;
        resulting_vector.Y = this->Y + rhs.Y;
        return resulting_vector;
    }

    /// Addition assignment operator.
    /// @param[in]  rhs - The vector on the right-hand side of the operator to
    ///     add to this vector.
    /// @return This vector with the provided vector added to it.
    template <typename ComponentType>
    Vector2<ComponentType>& Vector2<ComponentType>::operator+= (const Vector2<ComponentType>& rhs)
    {
        this->X += rhs.X;
        this->Y += rhs.Y;
        return (*this);
    }

    /// Subtraction operator.
    /// @param[in]  rhs - The vector on the right-hand side of the operator to
    ///     subtract from this vector.
    /// @return A new vector created by subtracting the provided vector from this vector.
    template <typename ComponentType>
    Vector2<ComponentType> Vector2<ComponentType>::operator- (const Vector2<ComponentType>& rhs) const
    {
        MATH::Vector2f resulting_vector;
        resulting_vector.X = this->X - rhs.X;
        resulting_vector.Y = this->Y - rhs.Y;
        return resulting_vector;
    }

    /// Gets the length (magnitude) of the vector.
    /// @return The length of the vector.
    template <typename ComponentType>
    ComponentType Vector2<ComponentType>::Length() const
    {
        // The dot product computes x*x + y*y.
        // The length is the square root of this (the distance formula).
        ComponentType length_squared = Vector2<ComponentType>::DotProduct(*this, *this);
        ComponentType length = sqrt(length_squared);
        return length;
    }
}
