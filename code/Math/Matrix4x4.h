#pragma once

#include <array>
#include <cmath>
#include "Containers/Array2D.h"
#include "Math/Angle.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"

namespace MATH
{
    /// A 2D mathematical matrix with 4 rows and 4 columns.
    /// It currently only has the minimal functionality needed,
    /// so it cannot directly perform all common matrix operations.
    ///
    /// The ElementType template parameter is intended to be replaced with
    /// any numerical type that is typically used for matrices (int, float, etc.).
    template <typename ElementType>
    class Matrix4x4
    {
    public:
        // STATIC CONSTANTS.
        /// 4 elements exist per dimension.
        static const unsigned int ELEMENT_COUNT_PER_DIMENSION = 4;
        /// 4 columns exist.
        static const unsigned int COLUMN_COUNT = ELEMENT_COUNT_PER_DIMENSION;
        /// 4 rows exist.
        static const unsigned int ROW_COUNT = ELEMENT_COUNT_PER_DIMENSION;

        // CONSTRUCTION.
        static Matrix4x4 Identity();
        static Matrix4x4 Translation(const Vector3<ElementType>& translation_vector);
        static Matrix4x4 Scale(const Vector3<ElementType>& scale_vector);
        static Matrix4x4 RotateX(const typename Angle<ElementType>::Radians angle_in_radians);
        static Matrix4x4 RotateY(const typename Angle<ElementType>::Radians angle_in_radians);
        static Matrix4x4 RotateZ(const typename Angle<ElementType>::Radians angle_in_radians);
        static Matrix4x4 Rotation(const typename Vector3< typename Angle<ElementType>::Radians >& angles_in_radians);

        // OPERATORS.
        Matrix4x4 operator* (const Matrix4x4& rhs) const;
        Vector4<ElementType> operator* (const Vector4<ElementType>& vector) const;

        // ELEMENT RETRIEVAL.
        const ElementType* ElementsInRowMajorOrder() const;

        // ELEMENT SETTING.
        void SetRow(const unsigned int row_index, const Vector3<ElementType>& vector);

        // MEMBER VARIABLES.
        /// The underlying 4x4 array of elements.
        CONTAINERS::Array2D<ElementType> Elements = CONTAINERS::Array2D<ElementType>(COLUMN_COUNT, ROW_COUNT);
    };

    // DEFINE COMMON MATRIX4 TYPES.
    /// A 4x4 matrix composed of float components.
    typedef Matrix4x4<float> Matrix4x4f;

    /// Creates an identity matrix.
    /// @return An identity matrix.
    template <typename ElementType>
    Matrix4x4<ElementType> Matrix4x4<ElementType>::Identity()
    {
        // CREATE IDENTITY MATRIX ELEMENTS.
        CONTAINERS::Array2D<ElementType> identity_elements = CONTAINERS::Array2D<ElementType>(
            COLUMN_COUNT, 
            ROW_COUNT,
            {
                1, 0, 0, 0,
                0, 1, 0, 0,
                0, 0, 1, 0,
                0, 0, 0, 1
            });

        // RETURN THE MATRIX.
        Matrix4x4<ElementType> identity_matrix;
        identity_matrix.Elements = identity_elements;
        return identity_matrix;
    }

    /// Creates a translation matrix.
    /// @param[in]  translation_vector - The vector defining the translation amount.
    /// @return The translation matrix for the provided vector.
    template <typename ElementType>
    Matrix4x4<ElementType> Matrix4x4<ElementType>::Translation(const Vector3<ElementType>& translation_vector)
    {
        // CREATE TRANSLATION MATRIX ELEMENTS.
        CONTAINERS::Array2D<ElementType> translation_elements = CONTAINERS::Array2D<ElementType>(
            COLUMN_COUNT,
            ROW_COUNT,
            {
                1, 0, 0, translation_vector.X,
                0, 1, 0, translation_vector.Y,
                0, 0, 1, translation_vector.Z,
                0, 0, 0, 1
            });

        // RETURN THE MATRIX.
        Matrix4x4<ElementType> translation_matrix;
        translation_matrix.Elements = translation_elements;
        return translation_matrix;
    }

    /// Creates a scale matrix.
    /// @param[in]  scale_vector - The vector defining the scaling amount.
    /// @return The scale matrix for the provided vector.
    template <typename ElementType>
    Matrix4x4<ElementType> Matrix4x4<ElementType>::Scale(const Vector3<ElementType>& scale_vector)
    {
        // CREATE SCALE MATRIX ELEMENTS.
        CONTAINERS::Array2D<ElementType> scale_elements = CONTAINERS::Array2D<ElementType>(
            COLUMN_COUNT,
            ROW_COUNT,
            {
                scale_vector.X, 0, 0, 0,
                0, scale_vector.Y, 0, 0,
                0, 0, scale_vector.Z, 0,
                0, 0, 0, 1
            });

        // RETURN THE MATRIX.
        Matrix4x4<ElementType> scale_matrix;
        scale_matrix.Elements = scale_elements;
        return scale_matrix;
    }

    /// Creates a rotation matrix about the X axis.
    /// @param[in]  angle_in_radians - The angle in radians to rotate by.
    /// @return The rotation matrix about the X axis.
    template <typename ElementType>
    Matrix4x4<ElementType> Matrix4x4<ElementType>::RotateX(const typename Angle<ElementType>::Radians angle_in_radians)
    {
        // CREATE ROTATION MATRIX ELEMENTS.
        CONTAINERS::Array2D<ElementType> rotation_elements = CONTAINERS::Array2D<ElementType>(
            COLUMN_COUNT,
            ROW_COUNT,
            {
                1, 0, 0, 0,
                0, cos(angle_in_radians.Value), -sin(angle_in_radians.Value), 0,
                0, sin(angle_in_radians.Value), cos(angle_in_radians.Value), 0,
                0, 0, 0, 1
            });

        // RETURN THE MATRIX.
        Matrix4x4<ElementType> rotation_matrix;
        rotation_matrix.Elements = rotation_elements;
        return rotation_matrix;
    }

    /// Creates a rotation matrix about the Y axis.
    /// @param[in]  angle_in_radians - The angle in radians to rotate by.
    /// @return The rotation matrix about the Y axis.
    template <typename ElementType>
    Matrix4x4<ElementType> Matrix4x4<ElementType>::RotateY(const typename Angle<ElementType>::Radians angle_in_radians)
    {
        // CREATE ROTATION MATRIX ELEMENTS.
        CONTAINERS::Array2D<ElementType> rotation_elements = CONTAINERS::Array2D<ElementType>(
            COLUMN_COUNT,
            ROW_COUNT,
            {
                cos(angle_in_radians.Value), 0, sin(angle_in_radians.Value), 0,
                0, 1, 0, 0,
                -sin(angle_in_radians.Value), 0, cos(angle_in_radians.Value), 0,
                0, 0, 0, 1
            });

        // RETURN THE MATRIX.
        Matrix4x4<ElementType> rotation_matrix;
        rotation_matrix.Elements = rotation_elements;
        return rotation_matrix;
    }

    /// Creates a rotation matrix about the Z axis.
    /// @param[in]  angle_in_radians - The angle in radians to rotate by.
    /// @return The rotation matrix about the Z axis.
    template <typename ElementType>
    Matrix4x4<ElementType> Matrix4x4<ElementType>::RotateZ(const typename Angle<ElementType>::Radians angle_in_radians)
    {
        // CREATE ROTATION MATRIX ELEMENTS.
        CONTAINERS::Array2D<ElementType> rotation_elements = CONTAINERS::Array2D<ElementType>(
            COLUMN_COUNT,
            ROW_COUNT,
            {
                cos(angle_in_radians.Value), -sin(angle_in_radians.Value), 0, 0,
                sin(angle_in_radians.Value), cos(angle_in_radians.Value), 0, 0,
                0, 0, 1, 0,
                0, 0, 0, 1
            });

        // RETURN THE MATRIX.
        Matrix4x4<ElementType> rotation_matrix;
        rotation_matrix.Elements = rotation_elements;
        return rotation_matrix;
    }

    /// Creates a rotation matrix about the 3 primary axes.
    /// @param[in]  angles_in_radians - The rotation angles across the 3 primary axes.
    /// @return The specified rotation matrix about the primary axes.
    template <typename ElementType>
    Matrix4x4<ElementType> Matrix4x4<ElementType>::Rotation(const typename Vector3< typename Angle<ElementType>::Radians >& angles_in_radians)
    {
        MATH::Matrix4x4<ElementType> x_rotation_matrix = RotateX(angles_in_radians.X);
        MATH::Matrix4x4<ElementType> y_rotation_matrix = RotateY(angles_in_radians.Y);
        MATH::Matrix4x4<ElementType> z_rotation_matrix = RotateZ(angles_in_radians.Z);

        MATH::Matrix4x4<ElementType> rotation_matrix = x_rotation_matrix * y_rotation_matrix * z_rotation_matrix;
        return rotation_matrix;
    }

    /// Multiples this matrix by the provided matrix.
    /// @param[in]  rhs - The matrix to multiply on the right-hand side.
    /// @return The product of the matrix multiplication.
    template <typename ElementType>
    Matrix4x4<ElementType> Matrix4x4<ElementType>::operator* (const Matrix4x4<ElementType>& rhs) const
    {
        Matrix4x4<ElementType> matrix_product;

        // COMPUTE PRODUCT ELEMENT VALUES FOR EACH ROW.
        for (unsigned int row_index = 0; row_index < ROW_COUNT; ++row_index)
        {
            // GET THE CURRENT ROW VALUES FOR THE LEFT-HAND SIDE OF THE PRODUCT ELEMENT BEING COMPUTED.
            std::array<ElementType, 4> lhs_row =
            { 
                this->Elements(0, row_index),
                this->Elements(1, row_index),
                this->Elements(2, row_index),
                this->Elements(3, row_index)
            };

            // COMPUTE PRODUCT ELEMENT VALUES FOR EACH COLUMN.
            for (unsigned int column_index = 0; column_index < COLUMN_COUNT; ++column_index)
            {
                // GET THE CURRENT COLUMN VALUES FOR THE RIGHT-HAND SIDE OF THE PRODUCT ELEMENT BEING COMPUTED.
                std::array<ElementType, 4> rhs_column =
                {
                    rhs.Elements(column_index, 0),
                    rhs.Elements(column_index, 1),
                    rhs.Elements(column_index, 2),
                    rhs.Elements(column_index, 3)
                };

                // COMPUTE THE PRODUCT VALUE AT THE CURRENT ROW/COLUMN.
                // The matrix elements are arranged by x, y (column, row),
                // which makes this a bit unintuitive for matrix math.
                matrix_product.Elements(column_index, row_index) =
                    (lhs_row[0] * rhs_column[0]) +
                    (lhs_row[1] * rhs_column[1]) +
                    (lhs_row[2] * rhs_column[2]) +
                    (lhs_row[3] * rhs_column[3]);
            }
        }

        return matrix_product;
    }

    /// Multiples a vector by this matrix.
    /// @param[in]  vector - The vector to multiply on the right-hand side.
    /// @return The vector transformed by this matrix.
    template <typename ElementType>
    Vector4<ElementType> Matrix4x4<ElementType>::operator* (const Vector4<ElementType>& vector) const
    {
        Vector4<ElementType> transformed_vector;

        // CALCULATE THE X COMPONENT OF THE VECTOR.
        const unsigned int ROW_1 = 0;
        const unsigned int COLUMN_1 = 0;
        const unsigned int COLUMN_2 = 1;
        const unsigned int COLUMN_3 = 2;
        const unsigned int COLUMN_4 = 3;
        transformed_vector.X =
            (this->Elements(COLUMN_1, ROW_1) * vector.X) +
            (this->Elements(COLUMN_2, ROW_1) * vector.Y) +
            (this->Elements(COLUMN_3, ROW_1) * vector.Z) +
            (this->Elements(COLUMN_4, ROW_1) * vector.W);

        // CALCULATE THE Y COMPONENT OF THE VECTOR.
        const unsigned int ROW_2 = 1;
        transformed_vector.Y =
            (this->Elements(COLUMN_1, ROW_2) * vector.X) +
            (this->Elements(COLUMN_2, ROW_2) * vector.Y) +
            (this->Elements(COLUMN_3, ROW_2) * vector.Z) +
            (this->Elements(COLUMN_4, ROW_2) * vector.W);

        // CALCULATE THE Z COMPONENT OF THE VECTOR.
        const unsigned int ROW_3 = 2;
        transformed_vector.Z =
            (this->Elements(COLUMN_1, ROW_3) * vector.X) +
            (this->Elements(COLUMN_2, ROW_3) * vector.Y) +
            (this->Elements(COLUMN_3, ROW_3) * vector.Z) +
            (this->Elements(COLUMN_4, ROW_3) * vector.W);

        // CALCULATE THE W COMPONENT OF THE VECTOR.
        const unsigned int ROW_4 = 3;
        transformed_vector.W =
            (this->Elements(COLUMN_1, ROW_4) * vector.X) +
            (this->Elements(COLUMN_2, ROW_4) * vector.Y) +
            (this->Elements(COLUMN_3, ROW_4) * vector.Z) +
            (this->Elements(COLUMN_4, ROW_4) * vector.W);

        return transformed_vector;
    }

    /// Gets the element values in row-major order
    /// (each row's values before the next row).
    /// @return The element values in row-major order.
    template <typename ElementType>
    const ElementType* Matrix4x4<ElementType>::ElementsInRowMajorOrder() const
    {
        return Elements.ValuesInRowMajorOrder();
    }

    /// Sets the first 3 elements in the row to the provided vector.
    /// The 4th element is left unchanged.
    /// @param[in]  vector - The values for the 1st 3 elements in the row.
    template <typename ElementType>
    void Matrix4x4<ElementType>::SetRow(const unsigned int row_index, const Vector3<ElementType>& vector)
    {
        // SET THE FIRST 3 ELEMENTS IN THE ROW.
        // X, Y, and Z ordering is based on intuitive understanding.
        Elements(0, row_index) = vector.X;
        Elements(1, row_index) = vector.Y;
        Elements(2, row_index) = vector.Z;
    }
}