#pragma once

#include <cmath>

namespace MATH
{
    /// A 3D mathematical vector with both magnitude and direction.
    /// It currently only has the minimal functionality needed,
    /// so it cannot directly perform all common vector operations.
    ///
    /// The ComponentType template parameter is intended to be replaced with
    /// any numerical type that is typically used for vectors (int, float, etc.).
    template <typename ComponentType>
    class Vector3
    {
    public:
        // STATIC METHODS.
        static Vector3 Scale(const ComponentType scale_factor, const Vector3& vector);
        static Vector3 Normalize(const Vector3& vector);
        static ComponentType DotProduct(const Vector3& vector_1, const Vector3& vector_2);
        static Vector3 CrossProduct(const Vector3& lhs, const Vector3& rhs);

        // CONSTRUCTION.
        explicit Vector3(
            const ComponentType x = static_cast<ComponentType>(0), 
            const ComponentType y = static_cast<ComponentType>(0),
            const ComponentType z = static_cast<ComponentType>(0));

        // OPERATORS.
        bool operator== (const Vector3& rhs) const;
        bool operator!= (const Vector3& rhs) const;
        Vector3 operator+ (const Vector3& rhs) const;
        Vector3& operator+= (const Vector3& rhs);
        Vector3 operator- (const Vector3& rhs) const;
        Vector3 operator- () const;

        // OTHER OPERATIONS.
        ComponentType Length() const;

        // PUBLIC MEMBER VARIABLES FOR EASY ACCESS.
        /// The x component of the vector.
        ComponentType X;
        /// The y component of the vector.
        ComponentType Y;
        /// The z component of the vector.
        ComponentType Z;
    };

    // DEFINE COMMON VECTOR3 TYPES.
    /// A vector composed of 3 unsigned integer components.
    typedef Vector3<unsigned int> Vector3ui;
    /// A vector composed of 3 float components.
    typedef Vector3<float> Vector3f;

    /// Computes a scaled version of a vector.
    /// @param[in]  scale_factor - The scale factor to multiply each component of the vector by.
    /// @param[in]  vector - The vector to scale.
    /// @return The scaled version of the vector.
    template <typename ComponentType>
    Vector3<ComponentType> Vector3<ComponentType>::Scale(const ComponentType scale_factor, const Vector3<ComponentType>& vector)
    {
        Vector3<ComponentType> scaled_vector;
        scaled_vector.X = scale_factor * vector.X;
        scaled_vector.Y = scale_factor * vector.Y;
        scaled_vector.Z = scale_factor * vector.Z;
        return scaled_vector;
    }

    /// Normalizes a vector to be unit length (length of 1).
    /// @param[in]  vector - The vector to normalize.
    /// @return The normalized version of the vector.
    ///     If the vector is a zero vector, then a zero vector is returned.
    template <typename ComponentType>
    Vector3<ComponentType> Vector3<ComponentType>::Normalize(const Vector3<ComponentType>& vector)
    {
        // GET THE VECTOR'S LENGTH.
        ComponentType vector_length = vector.Length();

        // RETURN A ZERO VECTOR IF THE VECTOR'S LENGTH IS ZERO.
        bool vector_length_is_zero = (0 == vector_length);
        if (vector_length_is_zero)
        {
            return Vector3<ComponentType>(0, 0, 0);
        }

        // CREATE A NORMALIZED VERSION OF THE VECTOR.
        Vector3<ComponentType> normalized_vector;
        normalized_vector.X = vector.X / vector_length;
        normalized_vector.Y = vector.Y / vector_length;
        normalized_vector.Z = vector.Z / vector_length;
        return normalized_vector;
    }

    /// Computes the dot product between 2 vectors.
    /// @param[in]  vector_1 - One vector to use in the dot product.
    /// @param[in]  vector_2 - Another vector to use in the dot product.
    /// @return The dot product between the 2 vectors.
    template <typename ComponentType>
    ComponentType Vector3<ComponentType>::DotProduct(
        const Vector3<ComponentType>& vector_1,
        const Vector3<ComponentType>& vector_2)
    {
        ComponentType dot_product =
            (vector_1.X * vector_2.X) +
            (vector_1.Y * vector_2.Y) +
            (vector_1.Z * vector_2.Z);
        return dot_product;
    }

    /// Computes the cross product between 2 vectors.
    /// @param[in]  lhs - The vector on the left-hand side of the cross product operation.
    /// @param[in]  rhs - The vector on the right-hand side of the cross product operation.
    /// @return The cross product between the 2 vectors.
    template <typename ComponentType>
    Vector3<ComponentType> Vector3<ComponentType>::CrossProduct(
        const Vector3<ComponentType>& lhs,
        const Vector3<ComponentType>& rhs)
    {
        Vector3<ComponentType> cross_product;
        cross_product.X = (lhs.Y * rhs.Z) - (lhs.Z * rhs.Y);
        cross_product.Y = (lhs.Z * rhs.X) - (lhs.X * rhs.Z);
        cross_product.Z = (lhs.X * rhs.Y) - (lhs.Y * rhs.X);
        return cross_product;
    }

    /// Constructor that accepts initial values.
    /// @param[in]  x - The x component value.
    /// @param[in]  y - The y component value.
    /// @param[in]  z - The z component value.
    template <typename ComponentType>
    Vector3<ComponentType>::Vector3(
        const ComponentType x, 
        const ComponentType y,
        const ComponentType z) :
    X(x),
    Y(y),
    Z(z)
    {};

    /// Equality operator.  Direct equality comparison is used for components,
    /// so the precision of components types should be considered when using
    /// this operator.
    /// @param[in]  rhs - The vector on the right-hand side of the operator.
    /// @return True if the vectors are equal; false otherwise.
    template <typename ComponentType>
    bool Vector3<ComponentType>::operator== (const Vector3<ComponentType>& rhs) const
    {
        bool x_component_matches = (this->X == rhs.X);
        bool y_component_matches = (this->Y == rhs.Y);
        bool z_component_matches = (this->Z == rhs.Z);

        bool all_components_match = (x_component_matches && y_component_matches && z_component_matches);
        return all_components_match;
    }

    /// Inequality operator.  Direct equality comparison is used for components,
    /// so the precision of components types should be considered when using
    /// this operator.
    /// @param[in]  rhs - The vector on the right-hand side of the operator.
    /// @return True if the vectors are unequal; false otherwise.
    template <typename ComponentType>
    bool Vector3<ComponentType>::operator!= (const Vector3<ComponentType>& rhs) const
    {
        bool vectors_equal = ((*this) == rhs);
        return !vectors_equal;
    }

    /// Addition operator.
    /// @param[in]  rhs - The vector on the right-hand side of the operator to
    ///     add to this vector.
    /// @return A new vector created by adding the provided vector to this vector.
    template <typename ComponentType>
    Vector3<ComponentType> Vector3<ComponentType>::operator+ (const Vector3<ComponentType>& rhs) const
    {
        MATH::Vector3<ComponentType> resulting_vector;
        resulting_vector.X = this->X + rhs.X;
        resulting_vector.Y = this->Y + rhs.Y;
        resulting_vector.Z = this->Z + rhs.Z;
        return resulting_vector;
    }

    /// Addition assignment operator.
    /// @param[in]  rhs - The vector on the right-hand side of the operator to
    ///     add to this vector.
    /// @return This vector with the provided vector added to it.
    template <typename ComponentType>
    Vector3<ComponentType>& Vector3<ComponentType>::operator+= (const Vector3<ComponentType>& rhs)
    {
        this->X += rhs.X;
        this->Y += rhs.Y;
        this->Z += rhs.Z;
        return (*this);
    }

    /// Subtraction operator.
    /// @param[in]  rhs - The vector on the right-hand side of the operator to
    ///     subtract from this vector.
    /// @return A new vector created by subtracting the provided vector from this vector.
    template <typename ComponentType>
    Vector3<ComponentType> Vector3<ComponentType>::operator- (const Vector3<ComponentType>& rhs) const
    {
        Vector3<ComponentType> resulting_vector;
        resulting_vector.X = this->X - rhs.X;
        resulting_vector.Y = this->Y - rhs.Y;
        resulting_vector.Z = this->Z - rhs.Z;
        return resulting_vector;
    }

    /// Creates a negated version of this vector.
    /// @return A negated version of this vector.
    template <typename ComponentType>
    Vector3<ComponentType> Vector3<ComponentType>::operator- () const
    {
        Vector3<ComponentType> negated_vector;
        negated_vector.X = -1 * this->X;
        negated_vector.Y = -1 * this->Y;
        negated_vector.Z = -1 * this->Z;
        return negated_vector;
    }
    
    /// Gets the length (magnitude) of the vector.
    /// @return The length of the vector.
    template <typename ComponentType>
    ComponentType Vector3<ComponentType>::Length() const
    {
        // The dot product computes x*x + y*y + z*z.
        // The length is the square root of this (the distance formula).
        ComponentType length_squared = Vector3<ComponentType>::DotProduct(*this, *this);
        ComponentType length = sqrt(length_squared);
        return length;
    }
}