#pragma once

#include <optional>
#include <vector>
#include "Graphics/Color.h"
#include "Graphics/Light.h"
#include "Graphics/Object3D.h"

namespace GRAPHICS
{
    /// A scene consisting of objects within a 3D space.
    class Scene
    {
    public:
        // PUBLIC MEMBER VARIABLES FOR EASY ACCESS.
        /// The background color of the scene.
        GRAPHICS::Color BackgroundColor = GRAPHICS::Color::BLACK;
        /// All objects in the scene.
        std::vector<Object3D> Objects = {};
        /// All point lights in the scene.
        /// An empty optional means lighting should not be computed for the scene
        /// (shading comes directly from vertex colors).
        /// A populated optional with an empty list means that lighting should be
        /// computed for the scene as if no lights existed.
        std::optional<std::vector<Light>> PointLights = std::nullopt;
    };
}
