#pragma once

#include <cstdint>

namespace muse
{

/** Enum for texture format */
enum class TextureFormat : std::uint8_t
{
    RGBA,
    SRGB_ALPHA,
    DEPTH_COMPONENT
};

}