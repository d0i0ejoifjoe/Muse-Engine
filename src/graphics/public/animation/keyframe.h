#pragma once

#include "graphics/public/transform.h"

#include <chrono>

namespace muse
{

/**
 *
 *  Structure that encapsulates all of data for keyframe.
 *  Keyframe it's just a transformation that is applied at given time.
 *
 */
struct Keyframe
{
    /** Time of keyframe. */
    std::chrono::milliseconds time;

    /** Transformation to apply at this time. */
    Transform transform;
};

}