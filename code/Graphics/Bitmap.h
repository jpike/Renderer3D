#pragma once

#include <cstdint>
#include <filesystem>
#include <memory>
#include "Containers/Array2D.h"
#include "Graphics/Color.h"
#include "Graphics/ColorFormat.h"

/// Holds computer graphics code.
namespace GRAPHICS
{
    /// A 2D rectangular bitmap.
    /// The general concept of a bitmap image (https://en.wikipedia.org/wiki/Bitmap)
    /// is used over alternative terms (frame, screen, surface, canvas, render target, etc.)
    /// to allow this class to be re-used in more contexts.
    ///
    /// Specific features include:
    /// - (0,0) is the top-left corner.
    /// - 32 bits per pixel.
    /// - Each pixel stores colors in the following format
    ///   (assumes a little-endian architecture): 0xRRGGBBAA.
    class Bitmap
    {
    public:
        // CONSTRUCTION/DESTRUCTION.
        static std::shared_ptr<Bitmap> Load(const std::filesystem::path& filepath);
        explicit Bitmap(
            const unsigned int width_in_pixels,
            const unsigned int height_in_pixels,
            const GRAPHICS::ColorFormat color_format);

        // DIMENSIONS.
        unsigned int GetWidthInPixels() const;
        unsigned int GetHeightInPixels() const;

        // OTHER ACCESSORS.
        const uint32_t* GetRawData() const;
        GRAPHICS::Color GetPixel(const unsigned int x, const unsigned int y) const;

        // DRAWING.
        void WritePixel(const unsigned int x, const unsigned int y, const uint32_t& color);
        void WritePixel(const unsigned int x, const unsigned int y, const Color& color);
        void FillPixels(const Color& color);

    private:
        // MEMBER VARIABLES.
        /// The width of the bitmap in pixels.
        unsigned int WidthInPixels;
        /// The height of the bitmap in pixels.
        unsigned int HeightInPixels;
        /// The color format of pixels in the bitmap.
        GRAPHICS::ColorFormat ColorFormat;
        /// The underlying pixel memory to which graphics are rendered.
        /// The top-left corner pixel is at (0,0), and 
        /// the bottom-right corner pixel is at (width-1, height-1). 
        CONTAINERS::Array2D<uint32_t> Pixels;
    };
}
