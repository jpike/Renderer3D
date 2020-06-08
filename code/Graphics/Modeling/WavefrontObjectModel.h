#pragma once

#include <filesystem>
#include <optional>
#include "Graphics/Object3D.h"

/// Holds code related to 3D models in computer graphics.
namespace GRAPHICS::MODELING
{
    /// A model in Wavefront .obj format (https://en.wikipedia.org/wiki/Wavefront_.obj_file).
    /// This is generally the simplest widely-supported 3D model format that is readable as plain text.
    /// This class is named based on the "model" concept rather than a "file" concept since
    /// a 3D model may include additional files such as a .mtl material file.
    /// This class only supports files with triangulated faces (not quads).
    class WavefrontObjectModel
    {
    public:
        static std::optional<Object3D> Load(const std::filesystem::path& obj_filepath);
    };
}
