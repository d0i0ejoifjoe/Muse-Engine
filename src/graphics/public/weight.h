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
    /**
     *
     *  Create an empty weight.
     *
     */
    Weight() = default;

    /**
     *
     *  Create a weight.
     *
     *  @param index Index of bone.
     *  @param weight Influence of bone.
     *
     */
    Weight(std::uint32_t index, float weight)
        : index(index)
        , weight(weight)
    {
    }

    /** Bone index. */
    std::uint32_t index = 0u;

    /** Weight */
    float weight = 0.0f;
};

}