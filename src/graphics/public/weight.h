#pragma once

#include <cstdint>
#include <string>

namespace muse
{
    /**
     * 
     *  Structure that encapsulates all of need data for a weight.
     * 
    */
    struct Weight
    {
        /** ID of vertex. */
        std::uint32_t vertex = 0;

        /** Influence on vertex. */
        float weight = 0.0f;

        /** Name of bone */
        std::string name = "";
    };
}