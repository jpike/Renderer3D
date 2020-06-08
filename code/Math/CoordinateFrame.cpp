#include "Math/CoordinateFrame.h"

namespace MATH
{
    /// Computes a coordinate frame from up and forward vectors.
    /// Both vectors are assumed to already be normalized.
    /// @param[in]  uo - \ref Up.
    /// @param[in]  forward - \ref Forward.
    /// @return A coordinate frame computed from the two vectors.
    CoordinateFrame CoordinateFrame::FromUpAndForward(const Vector3f& up, const Vector3f& forward)
    {
        // COMPUTE THE REMAINING BASIS VECTOR.
        MATH::Vector3f right_direction = MATH::Vector3f::CrossProduct(up, forward);
        MATH::Vector3f normalized_right_direction_basis_vector = MATH::Vector3f::Normalize(right_direction);

        // RETURN THE COORDINATE FRAME.
        CoordinateFrame coordinate_frame;
        coordinate_frame.Up = up;
        coordinate_frame.Right = normalized_right_direction_basis_vector;
        coordinate_frame.Forward = forward;
        return coordinate_frame;
    }
}
