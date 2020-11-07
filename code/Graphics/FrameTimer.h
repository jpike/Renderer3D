#pragma once

#include <chrono>
#include <string>

namespace GRAPHICS
{
    /// A timer to assist in measuring frame rate.
    class FrameTimer
    {
    public:
        // PUBLIC METHODS.
        void StartTimingFrame();
        void EndTimingFrame();

        std::string GetFrameTimingText() const;

    private:
        // TYPE ALIASES.
        /// The type of clock used for the frame timer.
        using ClockType = std::chrono::high_resolution_clock;

        // MEMBER VARIABLES.
        /// The start time for the current frame being tracked.
        ClockType::time_point FrameStartTime = {};
        /// The end time for the current frame being tracked.
        ClockType::time_point FrameEndTime = {};
    };
}
