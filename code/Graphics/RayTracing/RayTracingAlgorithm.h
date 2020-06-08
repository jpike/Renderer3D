#pragma once

#include <optional>
#include "Graphics/Camera.h"
#include "Graphics/Color.h"
#include "Graphics/RayTracing/IObject3D.h"
#include "Graphics/RayTracing/Ray.h"
#include "Graphics/RayTracing/RayObjectIntersection.h"
#include "Graphics/RayTracing/Scene.h"
#include "Graphics/RenderTarget.h"

namespace GRAPHICS
{
/// Holds code related to ray tracing.
namespace RAY_TRACING
{
    /// A basic ray tracing algorithm.
    class RayTracingAlgorithm
    {
    public:
        // PUBLIC METHODS.
        void Render(const Scene& scene, GRAPHICS::RenderTarget& render_target);

        // PUBLIC MEMBER VARIABLES FOR EASY ACCESS.
        /// The camera used for rendering.
        GRAPHICS::Camera Camera = GRAPHICS::Camera();
        /// True if ambient lighting should be calculated; false otherwise.
        bool Ambient = true;
        /// True if shadows should be calculated; false otherwise.
        bool Shadows = true;
        /// True if diffuse shading should be calculated; false otherwise.
        bool Diffuse = true;
        /// True if specular shading should be calculated; false otherwise.
        bool Specular = true;
        /// True if reflections should be calculated; false otherwise.
        bool Reflections = true;
        /// The maximum number of reflections to computer (if reflections are enabled).
        /// More reflections will take longer to render an image.
        unsigned int ReflectionCount = 5;

    private:
        // PRIVATE HELPER METHODS.
        GRAPHICS::Color ComputeColor(
            const Scene& scene,
            const RayObjectIntersection& intersection,
            const unsigned int remaining_reflection_count) const;
        std::optional<RayObjectIntersection> ComputeClosestIntersection(
            const Scene& scene,
            const Ray& ray,
            const IObject3D* const ignored_object = nullptr) const;
    };
}
}
