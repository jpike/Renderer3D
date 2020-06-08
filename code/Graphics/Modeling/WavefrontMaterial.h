#pragma once

#include <filesystem>
#include <memory>
#include "Graphics/Material.h"

namespace GRAPHICS::MODELING
{
    /// A material in the .mtl (Material Template Library) format.
    /// See https://en.wikipedia.org/wiki/Wavefront_.obj_file#Material_template_library.
    class WavefrontMaterial
    {
    public:
        static std::shared_ptr<Material> Load(const std::filesystem::path& mtl_filepath);
    };
}
