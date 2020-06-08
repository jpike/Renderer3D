#pragma once

#include <memory>
#include <vector>
#include "Graphics/Color.h"
#include "Graphics/RayTracing/IObject3D.h"
#include "Graphics/Light.h"

namespace GRAPHICS
{
namespace RAY_TRACING
{
    /// A scene consisting of objects within a 3D space.
    class Scene
    {
    public:

        // PUBLIC MEMBER VARIABLES FOR EASY ACCESS.
        /// The background color of the scene.
        GRAPHICS::Color BackgroundColor = GRAPHICS::Color::BLACK;
        /// All objects in the scene.
        std::vector< std::unique_ptr<IObject3D> > Objects = {};
        /// All point lights in the scene.
        std::vector<Light> PointLights = {};
    };
}
}
