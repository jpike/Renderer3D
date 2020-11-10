#pragma once

#include <limits>
#include "Containers/Array2D.h"

namespace GRAPHICS
{
    /// A depth buffer for keeping track of depth values during rendering.
    /// Also known as z-buffering: https://en.wikipedia.org/wiki/Z-buffering.
    class DepthBuffer
    {
    public:
        // STATIC CONSTANTS.
        /// The default minimum depth value for the depth buffer.
        static constexpr float MIN_DEPTH = std::numeric_limits<float>::max();
        /// The default maximum depth value for the depth buffer.
        static constexpr float MAX_DEPTH = std::numeric_limits<float>::lowest();

        // CONSTRUCTION/DESTRUCTION.
        explicit DepthBuffer(const unsigned int width_in_pixels, const unsigned int height_in_pixels);

        // OTHER METHODS.
        void ClearToDepth(const float depth);
        float GetDepth(const unsigned int x, const unsigned int y) const;
        void WriteDepth(const unsigned int x, const unsigned int y, const float depth);

    private:
        // MEMBER VARIABLES.
        /// The width of the depth buffer in pixels.
        unsigned int WidthInPixels;
        /// The height of the depth buffer in pixels.
        unsigned int HeightInPixels;
        /// The underlying depth buffer memory to which graphics are rendered.
        /// The top-left corner pixel is at (0,0), and 
        /// the bottom-right corner pixel is at (width-1, height-1). 
        CONTAINERS::Array2D<float> DepthValues;
    };
}
