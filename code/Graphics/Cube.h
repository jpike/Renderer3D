#pragma once

#include <memory>
#include <vector>
#include "Graphics/Material.h"
#include "Graphics/Object3D.h"

namespace GRAPHICS
{
    /// A cube that can be rendered.
    class Cube
    {
    public:
        // CONSTRUCTION.
        static Object3D Create(const std::shared_ptr<GRAPHICS::Material>& material);
    };
}
