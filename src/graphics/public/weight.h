#pragma once

#include <cstdint>

namespace muse
{
    /**
     * 
     *  This structure encapsulates all of data for a weight that helps us to determine how much influence bone
     *  has on vertex. 
     * 
    */
    struct Weight
    {
        /** Bone index. */
        std::uint32_t index = 0u;

        /** Weight */
        float weight = 0.0f;
    };
}