#pragma once

#include "log/public/logger.h"

#include <SDL.h>
#include <cmath>
#include <concepts>
#include <cstdlib>
#include <limits>
#include <numbers>
#include <random>
#include <tuple>
#include <utility>

namespace muse
{

/** Alias for a data buffer. */
using Data = std::vector<std::byte>;

/** Alias for a pair. */
template <class T>
using Pair = std::pair<T, T>;

/** PI */
inline constexpr auto pi = std::numbers::pi_v<float>;

/**
 *
 *  Convert given degrees to radians.
 *
 *  @param x Degrees.
 *
 *  @return Radians.
 *
 */
inline constexpr float to_radians(float x)
{
    return x * (pi / 180.0f);
}

/**
 *
 *  Swap two objects.
 *
 *  @param a Object a.
 *  @param b Object b.
 *
 *
 */
template <class T>
inline constexpr void swap(T &a, T &b) noexcept
{
    T tmp = a;
    a = b;
    b = tmp;
}

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
inline constexpr bool
compare(float a, float b)
{
    constexpr auto epsilon = std::numeric_limits<float>::epsilon();
    return std::fabs(a - b) < epsilon;
}

// Nodiscard
#define MUSE_NODISCARD [[nodiscard]]

/**
 *
 *  Get SDL error.
 *
 *  @param line Line where this function was called.
 *  @param filename File name where this function was called.
 *
 */
inline void __check_sdl_error(std::size_t line, const char *filename)
{
    if (auto str = SDL_GetError(); *str != '\0')
    {
        LOG_ERROR(SDL, "\nError string: {}\nFile: {}:{}", str, filename, line);
        std::abort();
    }
}

// Check SDL error macro
#define check_sdl_error() muse::__check_sdl_error(__LINE__, __FILE__)

/** Get whether type is an integer type */
template <class T>
concept IsInteger = std::is_integral_v<T>;

/** Get whether type is an floating point type */
template <class T>
concept IsFloat = std::is_floating_point_v<T>;

////////////////////////////////////////////////////////
///                    RNG Utils                     ///
////////////////////////////////////////////////////////

// ensure that every thread gets it's own random device
inline thread_local std::random_device device;

/**
 *
 *  Generate a random value with given distribution.
 *
 *  @param distribution Distribution of random value.
 *
 *  @return Random value generated with given distribution.
 *
 */
template <class T>
inline typename T::result_type gen(T distribution)
{
    static std::mt19937 engine(device());
    return distribution(engine);
}

/**
 *
 *  Get random integer of given integer type in certain range.
 *
 *  @param min Minimum value.
 *  @param max Maximum value.
 *
 *  @return Random int.
 *
 */
template <IsInteger T>
inline T random_int(T min = std::numeric_limits<T>::min(),
                    T max = std::numeric_limits<T>::max())
{
    return gen(std::uniform_int_distribution<T>(min, max));
}

/**
 *
 *  Get random float in given floating point type in certain range.
 *
 *  @param min Minimum value.
 *  @param max Maximum value.
 *
 *  @return Random float.
 *
 */
template <IsFloat T>
inline T random_float(T min, T max)
{
    return gen(std::uniform_real_distribution<T>(min, max));
}

/**
 *
 *  Flip a coin.
 *
 *  @param bias Chance for heads in range [0, 1].
 *
 *  @return True if heads, false if tails.
 *
 */
inline bool flip_coin(float bias = 0.5f)
{
    return gen(std::bernoulli_distribution(bias));
}

}