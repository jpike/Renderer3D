#pragma once

#include <vector>
#include "Graphics/Bitmap.h"
#include "Graphics/Camera.h"
#include "Graphics/Light.h"
#include "Graphics/Scene.h"

namespace GRAPHICS
{
    /// A software (non-graphics hardware) rasterization algorithm
    /// (http://en.wikipedia.org/wiki/Rasterisation) for rendering.
    struct SoftwareRasterizationAlgorithm
    {
        void Render(const Scene& scene, const Camera& camera, Bitmap& output_bitmap) const;
        void Render(const Object3D& object_3D, const std::vector<Light>& lights, const Camera& camera, Bitmap& output_bitmap) const;
    };
}
