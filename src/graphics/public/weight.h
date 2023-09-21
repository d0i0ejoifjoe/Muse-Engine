#pragma once

#include <cstdint>

namespace muse
{
    /**
     * 
     *  Structure that encapsulates all of needed data for a weight.
     *  Weight is used in vertex shader to help us determine how much influence bone has on vertex.
     * 
    */
    struct Weight
    {
        /** Influence of bone. */
        float influence = 0.0f;

        /** ID of vertex. */
        std::int32_t id = 0;
    };
}