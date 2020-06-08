#pragma once

#include <filesystem>
#include <memory>
#include "Graphics/ColorFormat.h"
#include "Graphics/RenderTarget.h"

namespace GRAPHICS
{
    /// An image that defines the texture of a material applied to a surface.
    /// Currently only bitmap (.bmp) texture files are supported.
    class Texture
    {
    public:
        static std::shared_ptr<Texture> Load(const std::filesystem::path& filepath);
        explicit Texture(
            const unsigned int width_in_pixels,
            const unsigned int height_in_pixels,
            const ColorFormat color_format);

        RenderTarget Bitmap;
    };
}
