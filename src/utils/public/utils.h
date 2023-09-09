#pragma once

#include <limits>
#include <numbers>
#include <cmath>

namespace muse
{
    /** PI */
    inline constexpr auto pi = std::numbers::pi_v<float>;
    
    /**
     * 
     *  Compare two floats.
     * 
     *  @param a A float.
     *  @param b B float.
     * 
     *  @return True if they're the same otherwise false.
     * 
    */
    constexpr bool compare(float a, float b)
    {
        constexpr auto epsilon = std::numeric_limits<float>::epsilon();
        return std::fabs(a - b) < epsilon;
    }
}