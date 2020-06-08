#pragma once

#include <cmath>
#include "Math/Vector3.h"

namespace MATH
{
    /// A 4D mathematical vector with both magnitude and direction.
    /// The 4th component is largely intended for homogeneous coordinates.
    /// It currently only has the minimal functionality needed,
    /// so it cannot directly perform all common vector operations.
    ///
    /// The ComponentType template parameter is intended to be replaced with
    /// any numerical type that is typically used for vectors (int, float, etc.).
    template <typename ComponentType>
    class Vector4
    {
    public:
        // STATIC METHODS.
        static Vector4 Normalize(const Vector4& vector);
        static ComponentType DotProduct(const Vector4& vector_1, const Vector4& vector_2);

        // CONSTRUCTION.
        static Vector4 HomogeneousPositionVector(const Vector3<ComponentType>& vector_3);
        explicit Vector4(
            const ComponentType x = static_cast<ComponentType>(0),
            const ComponentType y = static_cast<ComponentType>(0),
            const ComponentType z = static_cast<ComponentType>(0),
            const ComponentType w = static_cast<ComponentType>(0));

        // OPERATORS.
        bool operator== (const Vector4& rhs) const;
        bool operator!= (const Vector4& rhs) const;
        Vector4 operator+ (const Vector4& rhs) const;
        Vector4& operator+= (const Vector4& rhs);
        Vector4 operator- (const Vector4& rhs) const;
        Vector4 operator- () const;

        // OTHER OPERATIONS.
        ComponentType Length() const;

        // PUBLIC MEMBER VARIABLES FOR EASY ACCESS.
        /// The x component of the vector.
        ComponentType X;
        /// The y component of the vector.
        ComponentType Y;
        /// The z component of the vector.
        ComponentType Z;
        /// The w component of the vector.
        ComponentType W;
    };

    // DEFINE COMMON VECTOR4 TYPES.
    /// A vector composed of 4 float components.
    typedef Vector4<float> Vector4f;

    /// Normalizes a vector to be unit length (length of 1).
    /// @param[in]  vector - The vector to normalize.
    /// @return The normalized version of the vector.
    ///     If the vector is a zero vector, then a zero vector is returned.
    template <typename ComponentType>
    Vector4<ComponentType> Vector4<ComponentType>::Normalize(const Vector4<ComponentType>& vector)
    {
        // GET THE VECTOR'S LENGTH.
        ComponentType vector_length = vector.Length();

        // RETURN A ZERO VECTOR IF THE VECTOR'S LENGTH IS ZERO.
        bool vector_length_is_zero = (0 == vector_length);
        if (vector_length_is_zero)
        {
            return Vector4<ComponentType>(0, 0, 0, 0);
        }

        // CREATE A NORMALIZED VERSION OF THE VECTOR.
        Vector4<ComponentType> normalized_vector;
        normalized_vector.X = vector.X / vector_length;
        normalized_vector.Y = vector.Y / vector_length;
        normalized_vector.Z = vector.Z / vector_length;
        normalized_vector.W = vector.W / vector_length;
        return normalized_vector;
    }

    /// Computes the dot product between 2 vectors.
    /// @param[in]  vector_1 - One vector to use in the dot product.
    /// @param[in]  vector_2 - Another vector to use in the dot product.
    /// @return The dot product between the 2 vectors.
    template <typename ComponentType>
    ComponentType Vector4<ComponentType>::DotProduct(
        const Vector4<ComponentType>& vector_1,
        const Vector4<ComponentType>& vector_2)
    {
        ComponentType dot_product =
            (vector_1.X * vector_2.X) +
            (vector_1.Y * vector_2.Y) +
            (vector_1.Z * vector_2.Z) +
            (vector_1.W * vector_2.W);
        return dot_product;
    }

    /// Creates a homongeneous 4D position vector for a 3D vector.
    /// @param[in]  vector_3 - The 3D vector for which to create a 4D vector.
    /// @return The homogeneous 4D position vector for the 3D vector.
    template <typename ComponentType>
    Vector4<ComponentType> Vector4<ComponentType>::HomogeneousPositionVector(const Vector3<ComponentType>& vector_3)
    {
        Vector4<ComponentType> homogeneous_position_vector(
            vector_3.X,
            vector_3.Y,
            vector_3.Z,
            // The w component is always 1 for a homogeneous position vector.
            static_cast<ComponentType>(1));

        return homogeneous_position_vector;
    }

    /// Constructor that accepts initial values.
    /// @param[in]  x - The x component value.
    /// @param[in]  y - The y component value.
    /// @param[in]  z - The z component value.
    /// @param[in]  w - The w component value.
    template <typename ComponentType>
    Vector4<ComponentType>::Vector4(
        const ComponentType x,
        const ComponentType y,
        const ComponentType z,
        const ComponentType w) :
        X(x),
        Y(y),
        Z(z),
        W(w)
    {};

    /// Equality operator.  Direct equality comparison is used for components,
    /// so the precision of components types should be considered when using
    /// this operator.
    /// @param[in]  rhs - The vector on the right-hand side of the operator.
    /// @return True if the vectors are equal; false otherwise.
    template <typename ComponentType>
    bool Vector4<ComponentType>::operator== (const Vector4<ComponentType>& rhs) const
    {
        bool x_component_matches = (this->X == rhs.X);
        bool y_component_matches = (this->Y == rhs.Y);
        bool z_component_matches = (this->Z == rhs.Z);
        bool w_component_matches = (this->W == rhs.W);

        bool all_components_match = (x_component_matches && y_component_matches && z_component_matches && w_component_matches);
        return all_components_match;
    }

    /// Inequality operator.  Direct equality comparison is used for components,
    /// so the precision of components types should be considered when using
    /// this operator.
    /// @param[in]  rhs - The vector on the right-hand side of the operator.
    /// @return True if the vectors are unequal; false otherwise.
    template <typename ComponentType>
    bool Vector4<ComponentType>::operator!= (const Vector4<ComponentType>& rhs) const
    {
        bool vectors_equal = ((*this) == rhs);
        return !vectors_equal;
    }

    /// Addition operator.
    /// @param[in]  rhs - The vector on the right-hand side of the operator to
    ///     add to this vector.
    /// @return A new vector created by adding the provided vector to this vector.
    template <typename ComponentType>
    Vector4<ComponentType> Vector4<ComponentType>::operator+ (const Vector4<ComponentType>& rhs) const
    {
        MATH::Vector4<ComponentType> resulting_vector;
        resulting_vector.X = this->X + rhs.X;
        resulting_vector.Y = this->Y + rhs.Y;
        resulting_vector.Z = this->Z + rhs.Z;
        resulting_vector.W = this->W + rhs.W;
        return resulting_vector;
    }

    /// Addition assignment operator.
    /// @param[in]  rhs - The vector on the right-hand side of the operator to
    ///     add to this vector.
    /// @return This vector with the provided vector added to it.
    template <typename ComponentType>
    Vector4<ComponentType>& Vector4<ComponentType>::operator+= (const Vector4<ComponentType>& rhs)
    {
        this->X += rhs.X;
        this->Y += rhs.Y;
        this->Z += rhs.Z;
        this->W += rhs.W;
        return (*this);
    }

    /// Subtraction operator.
    /// @param[in]  rhs - The vector on the right-hand side of the operator to
    ///     subtract from this vector.
    /// @return A new vector created by subtracting the provided vector from this vector.
    template <typename ComponentType>
    Vector4<ComponentType> Vector4<ComponentType>::operator- (const Vector4<ComponentType>& rhs) const
    {
        Vector4<ComponentType> resulting_vector;
        resulting_vector.X = this->X - rhs.X;
        resulting_vector.Y = this->Y - rhs.Y;
        resulting_vector.Z = this->Z - rhs.Z;
        resulting_vector.W = this->Z - rhs.W;
        return resulting_vector;
    }

    /// Creates a negated version of this vector.
    /// @return A negated version of this vector.
    template <typename ComponentType>
    Vector4<ComponentType> Vector4<ComponentType>::operator- () const
    {
        Vector4<ComponentType> negated_vector;
        negated_vector.X = -1 * this->X;
        negated_vector.Y = -1 * this->Y;
        negated_vector.Z = -1 * this->Z;
        negated_vector.W = -1 * this->W;
        return negated_vector;
    }

    /// Gets the length (magnitude) of the vector.
    /// @return The length of the vector.
    template <typename ComponentType>
    ComponentType Vector4<ComponentType>::Length() const
    {
        // The dot product computes x*x + y*y + z*z + w*w.
        // The length is the square root of this (the distance formula).
        ComponentType length_squared = Vector4<ComponentType>::DotProduct(*this, *this);
        ComponentType length = sqrt(length_squared);
        return length;
    }
}
