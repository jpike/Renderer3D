#include "Graphics/DepthBuffer.h"

namespace GRAPHICS
{
    /// Constructor.
    /// @param[in]  width_in_pixels - The width of the buffer.
    /// @param[in]  height_in_pixels - The height of the buffer.
    DepthBuffer::DepthBuffer(const unsigned int width_in_pixels, const unsigned int height_in_pixels):
        WidthInPixels(width_in_pixels),
        HeightInPixels(height_in_pixels),
        DepthValues(width_in_pixels, height_in_pixels)
    {
        ClearToDepth(MAX_DEPTH);
    }

    /// Clears the depth buffer to the specified depth.
    /// @param[in]  depth - The depth value to clear the buffer too.
    void DepthBuffer::ClearToDepth(const float depth)
    {
        DepthValues.Fill(depth);
    }

    /// Gets the depth at the specified coordinates.
    /// @param[in]  x - The horizontal coordinate of the pixel.
    /// @param[in]  y - The vertical coorindate of the pixel.
    /// @return The depth for the specified pixel.
    float DepthBuffer::GetDepth(const unsigned int x, const unsigned int y) const
    {
        // RETURN A DEFAULT DEPTH VALUE IF THE PIXEL COORDINATES AREN'T VALID.
        bool pixel_coordinates_valid = DepthValues.IndicesInRange(x, y);
        if (!pixel_coordinates_valid)
        {
            return MIN_DEPTH;
        }

        // RETURN THE DEPTH.
        float depth = DepthValues(x, y);
        return depth;
    }

    /// Writes the depth at the specified coordinates.
    /// @param[in]  x - The horizontal coordinate of the pixel.
    /// @param[in]  y - The vertical coorindate of the pixel.
    /// @param[in]  depth - The depth for the specified pixel.
    void DepthBuffer::WriteDepth(const unsigned int x, const unsigned int y, const float depth)
    {
        // MAKE SURE THE PIXEL COORDINATES ARE VALID.
        bool pixel_coordinates_valid = DepthValues.IndicesInRange(x, y);
        if (!pixel_coordinates_valid)
        {
            // The depth can't be written.
            return;
        }

        // FILL IN THE DEPTH OF THE PIXEL.
        DepthValues(x, y) = depth;
    }
}
