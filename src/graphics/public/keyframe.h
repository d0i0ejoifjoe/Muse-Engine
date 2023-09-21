#pragma once

#include <chrono>

#include "transform.h"

namespace muse
{
    /**
     * 
     *  Structure that encapsulates all of data needed for key frame.
     *  Keyframe is just a transformation that is applied at given time in animation.
     * 
    */
    struct Keyframe
    {
        /** Time in animation */
        std::chrono::milliseconds time;

        /** Keyframe transformation */
        Transform transform;
    };
}