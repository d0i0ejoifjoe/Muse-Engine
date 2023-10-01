#pragma once

#include <limits>
#include <numbers>
#include <cmath>
#include <tuple>

namespace muse
{
    /** Alias for a pair. */
    template<class T>
    using Pair = std::pair<T, T>;

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

    #define MUSE_NODISCARD [[nodiscard]]
}