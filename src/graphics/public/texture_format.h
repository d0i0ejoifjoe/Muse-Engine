#pragma once

#include <cstdint>

namespace muse
{

/** Enum for texture format */
enum class TextureFormat : std::uint8_t
{
    RED,
    RG,
    RGB,
    RGBA,
    SRGB,
    SRGBA,
    REDF16,
    RGF16,
    RGBF16,
    RGBAF16,
    DEPTH_COMPONENT
};

}