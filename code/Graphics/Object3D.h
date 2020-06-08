#pragma once

#include <vector>
#include "Graphics/Triangle.h"
#include "Math/Angle.h"
#include "Math/Matrix4x4.h"
#include "Math/Vector3.h"

namespace GRAPHICS
{
    /// A generic object that exists in a 3D space.
    class Object3D
    {
    public:
        // METHODS.
        MATH::Matrix4x4f WorldTransform() const;

        // PUBLIC MEMBER VARIABLES FOR EASY ACCESS.
        /// The triangles of the object, in the local coordinate space of the object.
        std::vector<Triangle> Triangles = {};
        /// The world position of the object.
        MATH::Vector3f WorldPosition = MATH::Vector3f();
        /// The rotation of the object along the 3 primary axes, expressed in radians per axis.
        MATH::Vector3< MATH::Angle<float>::Radians > RotationInRadians = MATH::Vector3< MATH::Angle<float>::Radians >();
        /// The scaling of the object.  Defaults to no scaling (using the size of the triangles exactly).
        MATH::Vector3f Scale = MATH::Vector3f(1.0f, 1.0f, 1.0f);
    };
}
