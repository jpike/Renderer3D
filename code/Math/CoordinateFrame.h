#pragma once

#include "Math/Vector3.h"

namespace MATH
{
    /// The frame for a coordinate system.
    /// It is defined by 3 orthogonal basis vectors.
    /// Assumed to be a left-handed coordinate system.
    class CoordinateFrame
    {
    public:
        // CONSTRUCTION.
        static CoordinateFrame FromUpAndForward(const Vector3f& up, const Vector3f& forward);

        // PUBLIC MEMBER VARIABLES FOR EASY ACCESS.
        /// The up vector.  Corresponds to the positive y-axis in a traditional coordinate system.
        Vector3f Up = Vector3f(0.0f, 1.0f, 0.0f);
        /// The right vector.  Corresponds to the positive x-axis in a traditional coordinate system.
        Vector3f Right = Vector3f(1.0f, 0.0f, 0.0f);
        /// The forward vector.  Corresponds to the positive z-axis in a traditional coordinate system.
        /// For a left-handed coordinate system, this comes out from the origin towards the viewer.
        Vector3f Forward = Vector3f(0.0f, 0.0f, 1.0f);
    };
}
