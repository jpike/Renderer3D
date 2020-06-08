#include "Graphics/Cube.h"

namespace GRAPHICS
{
    /// Creates a cube (with side lengths of 1) of the specified color.
    /// @param[in]  The material of the cube.
    /// @return The requested cube.
    Object3D Cube::Create(const std::shared_ptr<GRAPHICS::Material>& material)
    {
        // DEFINE THE VERTICES OF THE CUBE.
        std::vector<MATH::Vector3f> vertices =
        {
            MATH::Vector3f(0.5f, 0.5f, 0.5f),
            MATH::Vector3f(-0.5f, 0.5f, 0.5f),
            MATH::Vector3f(-0.5f, 0.5f, -0.5f),
            MATH::Vector3f(0.5f, 0.5f, -0.5f),
            MATH::Vector3f(0.5f, -0.5f, 0.5f),
            MATH::Vector3f(-0.5f, -0.5f, 0.5),
            MATH::Vector3f(-0.5f, -0.5f, -0.5f),
            MATH::Vector3f(0.5f, -0.5, -0.5f),
        };

        // DEFINE THE TRIANGLES FOR THE FACES.
        std::vector<Triangle> triangles =
        {
            Triangle(material, {vertices[2], vertices[1], vertices[0]}),
            Triangle(material, {vertices[3], vertices[2], vertices[0]}),
            Triangle(material, {vertices[4], vertices[7], vertices[0]}),
            Triangle(material, {vertices[7], vertices[3], vertices[0]}),
            Triangle(material, {vertices[6], vertices[7], vertices[4]}),
            Triangle(material, {vertices[5], vertices[6], vertices[4]}),
            Triangle(material, {vertices[2], vertices[6], vertices[1]}),
            Triangle(material, {vertices[5], vertices[6], vertices[1]}),
            Triangle(material, {vertices[7], vertices[6], vertices[3]}),
            Triangle(material, {vertices[5], vertices[4], vertices[0]}),
            Triangle(material, {vertices[1], vertices[5], vertices[0]}),
        };

        // RETURN THE 3D OBJECT.
        Object3D cube;
        cube.Triangles = triangles;
        return cube;
    }
}
