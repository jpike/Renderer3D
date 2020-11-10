#pragma once

#include <memory>
#include <vector>
#include "Graphics/Bitmap.h"
#include "Graphics/Color.h"
#include "Math/Vector2.h"

namespace GRAPHICS
{
    /// The different kinds of shading that are supported.
    enum class ShadingType
    {
        /// Objects are rendered as wireframes.
        /// Colors will be interpolated across vertices.
        /// Defaults to wireframe to ensure at least something gets rendered.
        WIREFRAME = 0,
        /// Objects are rendered with a solid, flat color.
        FLAT,
        /// Objects are rendered with colors interpolated across vertices across the entire face.
        FACE_VERTEX_COLOR_INTERPOLATION,
        /// Typical Gouraud shading, with diffuse and specular components computer per vertex.
        GOURAUD,
        /// Shading that incorporates a texture.
        /// @todo   Should this be incorporated into something else?
        TEXTURED,
        /// Using material colors rather than vertex colors.
        /// @todo   How to handle differences here?
        MATERIAL,
        /// An extra enum to indicate the number of different shading types.
        COUNT
    };

    /// A material defining properties of a surface and how it's shaded.
    class Material
    {
    public:
        /// The type of shading for the material.
        ShadingType Shading = ShadingType::WIREFRAME;

        /// The vertex colors for shading.
        /// Expected to be in counter-clockwise vertex order, and the number of vertex colors
        /// can vary to handle different kinds of polygonal shapes.
        std::vector<Color> VertexColors = {};

        /// The ambient color of the material.
        Color AmbientColor = Color::BLACK;
        /// The diffuse color of the material.
        Color DiffuseColor = Color::BLACK;
        /// The specular color of the material.
        Color SpecularColor = Color::BLACK;
        /// The specular power defining the shininess of specular highlights.
        float SpecularPower = 0.0f;
        /// How reflective the material is as a proportion from [0, 1].
        float ReflectivityProportion = 0.0f;
        /// The emissive color if the material emits light.
        Color EmissiveColor = Color::BLACK;

        /// Any texture defining the look of the material.
        std::shared_ptr<Bitmap> Texture = nullptr;

        /// Any texture coordinates [0,1] for the vertices.
        std::vector<MATH::Vector2f> VertexTextureCoordinates = {};
    };
}
