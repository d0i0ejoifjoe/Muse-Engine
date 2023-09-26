#pragma once

#include "transform.h"

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
        float time;

        /** Transformation to apply at this time. */
        Transform transform;
    };
}