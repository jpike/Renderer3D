#include "Graphics/FrameTimer.h"

namespace GRAPHICS
{
    /// Resets the timer to start timing a frame.
    void FrameTimer::StartTimingFrame()
    {
        FrameStartTime = ClockType::now();
    }

    /// Sets the current time as the ending timing measurement for the frame.
    void FrameTimer::EndTimingFrame()
    {
        FrameEndTime = ClockType::now();
    }

    /// Gets some display text regarding frame timing measurements that can be useful for debugging.
    /// @return Text for the frame timing measurements.
    std::string FrameTimer::GetFrameTimingText() const
    {
        // COMPUTE THE ELAPSED TIME.
        auto frame_elapsed_time = FrameEndTime - FrameStartTime;

        // CREATE MILLISECONDS DISPLAY TEXT.
        auto frame_elapsed_time_milliseconds = std::chrono::duration_cast<std::chrono::duration<float, std::chrono::milliseconds::period>>(frame_elapsed_time);
        std::string frame_elapsed_time_milliseconds_string = std::to_string(frame_elapsed_time_milliseconds.count()) + "ms";

        // CREATE SECONDS DISPLAY TEXT.
        auto frame_elapsed_time_seconds = std::chrono::duration_cast<std::chrono::duration<float, std::chrono::seconds::period>>(frame_elapsed_time);
        std::string frame_elapsed_time_seconds_string = std::to_string(frame_elapsed_time_seconds.count()) + "s";

        // CREATE FRAME RATE DISPLAY TEXT.
        float frames_per_second = 1.0f / frame_elapsed_time_seconds.count();
        std::string frames_per_second_string = std::to_string(frames_per_second) + "fps";

        // RETURN THE FINAL DISPLAY TEXT.
        std::string frame_timing_text = frames_per_second_string + " " + frame_elapsed_time_seconds_string + " " + frame_elapsed_time_milliseconds_string;
        return frame_timing_text;
    }
}
