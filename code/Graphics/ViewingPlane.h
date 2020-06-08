#pragma once

namespace GRAPHICS
{
    /// A plane onto which an image is projected when viewed by a camera.
    /// The viewing plane is always centerd directly in front of the camera.
    class ViewingPlane
    {
    public:
        /// The distance between the camera and this viewing plane.
        /// Set to 1 to give some minimum distance between the camera and
        /// viewing plane (which is needed for perspective projection).
        float FocalLength = 1.0f;
        /// The width of the viewing plane.
        /// Defaults to 2 for simple viewing plane with min/max dimensions of -1 and 1.
        float Width = 2.0f;
        /// The height of the viewing plane.
        /// Defaults to 2 for simple viewing plane with min/max dimensions of -1 and 1.
        float Height = 2.0f;
    };
}
