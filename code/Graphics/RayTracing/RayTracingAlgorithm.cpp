#include <algorithm>
#include <cmath>
#include "Graphics/RayTracing/RayTracingAlgorithm.h"
#include "Math/Angle.h"

namespace GRAPHICS
{
namespace RAY_TRACING
{
    /// Renders a scene to the specified render target.
    /// @param[in]  scene - The scene to render.
    /// @param[in,out]  render_target - The target to render to.
    void RayTracingAlgorithm::Render(const Scene& scene, GRAPHICS::RenderTarget& render_target)
    {
        // RENDER EACH ROW OF PIXELS.
        unsigned int render_target_height_in_pixels = render_target.GetHeightInPixels();
        for (unsigned int y = 0; y < render_target_height_in_pixels; ++y)
        {
            // RENDER EACH COLUMN IN THE CURRENT ROW.
            unsigned int render_target_width_in_pixels = render_target.GetWidthInPixels();
            for (unsigned int x = 0; x < render_target_width_in_pixels; ++x)
            {
                // COMPUTE THE VIEWING RAY.
                MATH::Vector2ui pixel_coordinates(x, y);
                Ray ray = Camera.ViewingRay(pixel_coordinates, render_target);

                // FIND THE CLOSEST OBJECT IN THE SCENE THAT THE RAY INTERSECTS.
                std::optional<RayObjectIntersection> closest_intersection = ComputeClosestIntersection(scene, ray);

                // COLOR THE CURRENT PIXEL.
                if (closest_intersection)
                {
                    // COMPUTE THE CURRENT PIXEL'S COLOR.
                    Color color = ComputeColor(scene, *closest_intersection, ReflectionCount);
                    render_target.WritePixel(x, y, color);
                }
                else
                {
                    // FILL THE PIXEL WITH THE BACKGROUND COLOR.
                    render_target.WritePixel(x, y, scene.BackgroundColor);
                }
            }
        }
    }

    /// Computes color based on the specified intersection in the scene.
    /// @param[in]  scene - The scene in which the color is being computed.
    /// @param[in]  intersection - The intersection for which to compute the color.
    /// @param[in]  remaining_reflection_count - The remaining reflection depth for color computation.
    ///     To compute more accurate light, rays need to be reflected, but we don't want this to go on forever.
    ///     Furthermore, more rays can be computationally expensive for little more gain, which is why 
    ///     the amount of reflection is capped.
    /// @return The computed color.
    GRAPHICS::Color RayTracingAlgorithm::ComputeColor(
        const Scene& scene, 
        const RayObjectIntersection& intersection,
        const unsigned int remaining_reflection_count) const
    {
        // INITIALIZE THE COLOR TO HAVE NO CONTRIBUTION FROM ANY SOURCES.
        Color final_color = Color::BLACK;

        // ADD IN THE AMBIENT COLOR IF ENABLED.
        const Material* intersected_material = intersection.Object->GetMaterial();
        if (Ambient)
        {
            final_color += intersected_material->AmbientColor;
        }

        // COMPUTE SHADOWS.
        std::vector<float> shadow_factors_by_light_index;
        MATH::Vector3f intersection_point = intersection.IntersectionPoint();
        for (const Light& light : scene.PointLights)
        {
            // CAST A RAY OUT TO COMPUTE SHADOWS IF ENABLED.
            // To simplify later parts of the algorithm, a shadow factor of 1 (no shadowing)
            // should always be computed.
            constexpr float NO_SHADOWING = 1.0f;
            float shadow_factor = NO_SHADOWING;
            if (Shadows)
            {
                // SHOOT A SHADOW RAY OUT FROM THE INTERSECTION POINT TO THE LIGHT.
                MATH::Vector3f direction_from_point_to_light = light.PointLightDirectionFrom(intersection_point);
                Ray shadow_ray(intersection_point, direction_from_point_to_light);
                std::optional<RayObjectIntersection> shadow_intersection = ComputeClosestIntersection(scene, shadow_ray, intersection.Object);
                if (shadow_intersection)
                {
                    // DETERMINE THE SHADOW FACTOR BASED ON THE INTERSECTION.
                    // For a shadow to occur, the intersection with another object must occur in front of the shadow ray.
                    // Similarly, the intersection must occur before the ray hits the light (hence why the shadow ray
                    // is computed with a direction that is not unit length but the full length from the intersection
                    // point to the light - it makes checking for the distance to the light easier).
                    constexpr float NO_DISTANCE_IN_FRONT_OF_SHADOW_RAY = 0.0f;
                    constexpr float DISTANCE_AT_LIGHT = 1.0f;
                    bool shadow_intersection_in_range = (
                        (NO_DISTANCE_IN_FRONT_OF_SHADOW_RAY < shadow_intersection->DistanceFromRayToObject) &&
                        (shadow_intersection->DistanceFromRayToObject < DISTANCE_AT_LIGHT));
                    if (shadow_intersection_in_range)
                    {
                        constexpr float FULL_SHADOWING = 0.0f;
                        shadow_factor = FULL_SHADOWING;
                    }
                    else
                    {
                        shadow_factor = NO_SHADOWING;
                    }
                }
            }

            // STORE THE SHADOW FACTOR FOR THE LIGHT.
            shadow_factors_by_light_index.push_back(shadow_factor);
        }

        // ADD IN DIFFUSE COLOR FROM LIGHTS IF ENABLED.
        MATH::Vector3f unit_surface_normal = intersection.Object->SurfaceNormal(intersection_point);
        if (Diffuse)
        {
            // ADD DIFFUSE CONTRIBUTIONS FROM ALL LIGHT SOURCES.
            Color light_total_color = Color::BLACK;
            for (std::size_t light_index = 0; light_index < scene.PointLights.size(); ++light_index)
            {
                // ADD COLOR FROM THE CURRENT LIGHT.
                // This is based on the Lambertian shading model.
                // An object is maximally illuminated when facing toward the light.
                // An object tangent to the light direction or facing away receives no illumination.
                // In-between, the amount of illumination is proportional to the cosine of the angle between
                // the light and surface normal (where the cosine can be computed via the dot product).
                const Light& light = scene.PointLights.at(light_index);
                MATH::Vector3f direction_from_point_to_light = light.PointLightDirectionFrom(intersection_point);
                MATH::Vector3f unit_direction_from_point_to_light = MATH::Vector3f::Normalize(direction_from_point_to_light);
                constexpr float NO_ILLUMINATION = 0.0f;
                float illumination_proportion = MATH::Vector3f::DotProduct(unit_surface_normal, unit_direction_from_point_to_light);
                illumination_proportion = std::max(NO_ILLUMINATION, illumination_proportion);

                // ADD THE CURRENT LIGHT'S COLOR.
                float shadow_factor = shadow_factors_by_light_index.at(light_index);
                Color current_light_color = Color::ScaleRedGreenBlue(illumination_proportion, light.Color);
                current_light_color = Color::ScaleRedGreenBlue(shadow_factor, current_light_color);
                light_total_color += current_light_color;
            }

            // The diffuse color is multiplied component-wise by the amount of light.
            Color diffuse_color = Color::ComponentMultiplyRedGreenBlue(
                intersected_material->DiffuseColor,
                light_total_color);
            final_color += diffuse_color;
        }

        // ADD IN SPECULAR COLOR IF ENABLED.
        // This is based on the Blinn-Phong model.
        if (Specular)
        {
            // ADD SPECULAR CONTRIBUTIONS FROM ALL LIGHT SOURCES.
            Color specular_light_total_color = Color::BLACK;
            MATH::Vector3f ray_from_intersection_to_eye = intersection.Ray->Origin - intersection_point;
            MATH::Vector3f normalized_ray_from_intersection_to_eye = MATH::Vector3f::Normalize(ray_from_intersection_to_eye);
            for (std::size_t light_index = 0; light_index < scene.PointLights.size(); ++light_index)
            {
                // COMPUTE THE AMOUNT OF ILLUMINATION FROM THE CURRENT LIGHT.
                const Light& light = scene.PointLights.at(light_index);
                MATH::Vector3f direction_from_point_to_light = light.PointLightDirectionFrom(intersection_point);
                MATH::Vector3f unit_direction_from_point_to_light = MATH::Vector3f::Normalize(direction_from_point_to_light);
                constexpr float NO_ILLUMINATION = 0.0f;
                float illumination_proportion = MATH::Vector3f::DotProduct(unit_surface_normal, unit_direction_from_point_to_light);
                illumination_proportion = std::max(NO_ILLUMINATION, illumination_proportion);

                // COMPUTE THE REFLECTED LIGHT DIRECTION.
                MATH::Vector3f reflected_light_along_surface_normal = MATH::Vector3f::Scale(2.0f * illumination_proportion, unit_surface_normal);
                MATH::Vector3f reflected_light_direction = reflected_light_along_surface_normal - unit_direction_from_point_to_light;
                MATH::Vector3f unit_reflected_light_direction = MATH::Vector3f::Normalize(reflected_light_direction);

                // COMPUTE THE SPECULAR AMOUNT.
                float specular_proportion = MATH::Vector3f::DotProduct(normalized_ray_from_intersection_to_eye, unit_reflected_light_direction);
                specular_proportion = std::max(NO_ILLUMINATION, specular_proportion);
                specular_proportion = std::pow(specular_proportion, intersected_material->SpecularPower);

                // ADD THE CURRENT LIGHT'S SPECULAR COLOR.
                float shadow_factor = shadow_factors_by_light_index.at(light_index);
                float light_proportion = shadow_factor * specular_proportion;
                Color current_light_specular_color = Color::ScaleRedGreenBlue(light_proportion, light.Color);
                specular_light_total_color += current_light_specular_color;
            }

            // The specular color is multiplied component-wise by the amount of light.
            Color specular_color = Color::ComponentMultiplyRedGreenBlue(
                intersected_material->SpecularColor,
                specular_light_total_color);
            final_color += specular_color;
        }

        // COMPUTE REFLECTED LIGHT IF POSSIBLE.
        if (Reflections)
        {
            // CHECK IF THE RAY CAN BE REFLECTED.
            // In addition to the remaining reflections, there's no need to compute
            // color from reflected light in the material isn't reflective.
            bool ray_can_be_reflected = (remaining_reflection_count > 0) && (intersected_material->ReflectivityProportion > 0.0f);
            if (!ray_can_be_reflected)
            {
                // RETURN THE COLOR AS-IS.
                return final_color;
            }

            // COMPUTE THE REFLECTED RAY.
            MATH::Vector3f direction_from_ray_origin_to_intersection = intersection_point - intersection.Ray->Origin;
            MATH::Vector3f normalized_direction_from_ray_origin_to_intersection = MATH::Vector3f::Normalize(direction_from_ray_origin_to_intersection);
            float length_of_ray_along_surface_normal = MATH::Vector3f::DotProduct(normalized_direction_from_ray_origin_to_intersection, unit_surface_normal);
            float twice_length_of_ray_along_surface_normal = 2.0f * length_of_ray_along_surface_normal;
            MATH::Vector3f twice_reflected_ray_along_surface_normal = MATH::Vector3f::Scale(twice_length_of_ray_along_surface_normal, unit_surface_normal);
            MATH::Vector3f reflected_ray_direction = normalized_direction_from_ray_origin_to_intersection - twice_reflected_ray_along_surface_normal;
            MATH::Vector3f normalized_reflected_ray_direction = MATH::Vector3f::Normalize(reflected_ray_direction);
            Ray reflected_ray(intersection_point, normalized_reflected_ray_direction);

            // CHECK FOR ANY INTERSECTIONS FROM THE REFLECTED RAY.
            std::optional<RayObjectIntersection> reflected_intersection = ComputeClosestIntersection(scene, reflected_ray, intersection.Object);
            if (reflected_intersection)
            {
                // COMPUTE THE REFLECTED COLOR.
                const unsigned int child_reflection_count = remaining_reflection_count - 1;
                Color raw_reflected_color = ComputeColor(scene, *reflected_intersection, child_reflection_count);
                Color reflected_color = Color::ScaleRedGreenBlue(intersected_material->ReflectivityProportion, raw_reflected_color);
                final_color += reflected_color;
            }
            else
            {
                // ADD REFLECTED LIGHT CONTRIBUTED FROM THE BACKGROUND.
                Color reflected_color = Color::ScaleRedGreenBlue(intersected_material->ReflectivityProportion, scene.BackgroundColor);
                final_color += reflected_color;
            }
        }

        return final_color;
    }

    /// Computes the closest intersection in the scene of a specific ray.
    /// @param[in]  scene - The scene in which to search for intersections.
    /// @param[in]  ray - The ray to use for searching for intersections.
    /// @param[in]  ignored_object - An optional object to be ignored.  If provided,
    ///     this object will be ignored for intersections.  This provides an easy way
    ///     to calculate intersections from reflected rays without having the object
    ///     being reflected off of infinitely intersected with.
    /// @return The closest intersection, if one was found; unpopulated if no intersection
    ///     was found between the ray and an object in the scene.
    std::optional<RayObjectIntersection> RayTracingAlgorithm::ComputeClosestIntersection(
        const Scene& scene,
        const Ray& ray,
        const IObject3D* const ignored_object) const
    {
        // FIND THE CLOSEST OBJECT IN THE SCENE THAT THE RAY INTERSECTS.
        std::optional<RayObjectIntersection> closest_intersection = std::nullopt;
        for (const auto& current_object : scene.Objects)
        {
            // SKIP OVER THE CURRENT OBJECT IF IT SHOULD BE IGNORED.
            bool ignore_current_object = (ignored_object == current_object.get());
            if (ignore_current_object)
            {
                continue;
            }

            // CHECK IF THE RAY INTERSECTS THE CURRENT OBJECT.
            std::optional<RayObjectIntersection> intersection = current_object->Intersect(ray);
            bool ray_hit_object = (std::nullopt != intersection);
            if (!ray_hit_object)
            {
                // CONTINUE SEEING IF OTHER OBJECTS ARE HIT.
                continue;
            }

            // UPDATE THE CLOSEST INTERSECTION APPROPRIATELY.
            if (closest_intersection)
            {
                // ONLY OVERWRITE THE CLOSEST INTERSECTION IF THE NEWEST ONE IS CLOSER.
                bool new_intersection_closer = (intersection->DistanceFromRayToObject < closest_intersection->DistanceFromRayToObject);
                if (new_intersection_closer)
                {
                    closest_intersection = intersection;
                }
            }
            else
            {
                // SET THIS FIRST INTERSECTION AS THE CLOSEST.
                closest_intersection = intersection;
            }
        }

        return closest_intersection;
    }
}
}
