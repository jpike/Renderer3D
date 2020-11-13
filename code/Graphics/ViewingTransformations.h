#pragma once

#include <optional>
#include "Graphics/Bitmap.h"
#include "Graphics/Camera.h"
#include "Graphics/ScreenSpaceTriangle.h"
#include "Graphics/Triangle.h"
#include "Math/Matrix4x4.h"

namespace GRAPHICS
{
    /// The transformations needed to view geometry in 3D space on a 2D plane.
    /// This class encapsulates the domain algorithms for these transformations.
    /// It may also be knowing as a "pipeline" in some contexts and is very similar
    /// to a vertex processing stage in a larger graphics rendering pipeline.
    /// However, this class specifically does not include world transformations
    /// or shading - only the concept of "viewing" some geometry in 3D space
    /// with a camera in order to produce a 2D image.
    class ViewingTransformations
    {
    public:
        explicit ViewingTransformations(const Camera& camera);
        explicit ViewingTransformations(const Camera& camera, const Bitmap& output_plane);

        std::optional<ScreenSpaceTriangle> Apply(const Triangle& world_triangle) const;

        /// The transform to transform a vertex from world to camera view space.
        MATH::Matrix4x4f CameraViewTransform = {};
        /// The projection transform used by the camera.
        MATH::Matrix4x4f CameraProjectionTransform = {};
        /// The near clip plane viewing distance for the camera.
        float CameraNearClipPlaneViewDistance = 0.0f;
        /// The far clip plane viewing distance for the camera.
        float CameraFarClipPlaneViewDistance = 0.0f;
        /// The transform to transform a vertex from projected view space to screen space.
        MATH::Matrix4x4f ScreenTransform = {};
    };
}
