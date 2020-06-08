#pragma once

#include <memory>
#include <vector>
#include "Graphics/Color.h"
#include "Graphics/Texture.h"
#include "Math/Vector2.h"

namespace GRAPHICS
{
    /// The different kinds of shading that are supported.
    enum class ShadingType
    {
        /// Objects are rendered as wireframes with a single color.
        /// Defaults to wireframe to ensure at least something gets rendered.
        WIREFRAME = 0,
        /// Objects are rendered as wireframes with colors interpolated across vertices.
        WIREFRAME_VERTEX_COLOR_INTERPOLATION,
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

        /// The color of an edge, if wireframe shading is used.
        Color WireframeColor = Color::BLACK;

        /// The colors of each of the vertices for use in wireframe shading.
        /// Expected to be in counter-clockwise vertex order, and the number of vertex colors
        /// can vary to handle different kinds of polygonal shapes.
        std::vector<Color> VertexWireframeColors = {};

        /// The color of a face, if solid-color face (flat) shading is used.
        Color FaceColor = Color::BLACK;

        /// The colors of each of the vertices for use in interpolated face shading.
        /// Expected to be in counter-clockwise vertex order, and the number of vertex colors
        /// can vary to handle different kinds of polygonal shapes.
        std::vector<Color> VertexFaceColors = {};

        /// The vertex colors for Gouraud shading.
        /// Expected to be in counter-clockwise vertex order, and the number of vertex colors
        /// can vary to handle different kinds of polygonal shapes.
        /// @todo   Consolidate these vertex colors with other vertex colors?
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
        std::shared_ptr<Texture> Texture = nullptr;

        /// Any texture coordinates [0,1] for the vertices.
        std::vector<MATH::Vector2f> VertexTextureCoordinates = {};
    };
}
