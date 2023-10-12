#include "graphics/public/cube_map.h"

#include "graphics/public/sampler.h"

#include <tuple>

/**
 *
 *  Convert color channels to opengl data format.
 *
 *  @param color_channels How much color channels has every texel in data buffer.
 *  @param gamma_correction Does image have gamma correction.
 *  @param is_depth Whether image contains depth data.
 *
 *  @return Tuple with [internal format, format, type].
 *
 */
inline std::tuple<GLenum, GLenum, GLenum> to_opengl(std::uint32_t color_channels, bool gamma_correction, bool is_depth)
{
    GLenum format = GL_NONE;
    GLenum internal_format = GL_NONE;

    if (is_depth)
    {
        return {GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT};
    }

    switch (color_channels)
    {
        case 1:
            internal_format = GL_RED;
            format = GL_RED;
            break;
        case 2:
            internal_format = GL_RG;
            format = GL_RG;
            break;
        case 3:
            internal_format = gamma_correction ? GL_SRGB : GL_RGB;
            format = GL_RGB;
            break;
        case 4:
            internal_format = gamma_correction ? GL_SRGB_ALPHA : GL_RGBA;
            format = GL_RGBA;
            break;
    }

    return {internal_format, format, GL_UNSIGNED_BYTE};
}

namespace muse
{

CubeMap::CubeMap(std::byte *left_data, std::byte *right_data, std::byte *up_data, std::byte *down_data,
                 std::byte *front_data, std::byte *back_data, std::uint32_t width, std::uint32_t height,
                 TextureFormat format, Sampler *sampler, std::uint32_t color_channels, std::int32_t index)
    : width_(width)
    , height_(height)
    , handle_(0)
    , bindless_handle_(0)
    , index_(index)
{
    glGenTextures(1, &handle_);
    glBindTexture(GL_TEXTURE_CUBE_MAP, handle_);

    std::array<std::byte *, 6> data = {right_data, left_data, up_data, down_data, back_data, front_data};

    auto [internal_format, gl_format, gl_type] = to_opengl(color_channels,
                                                           format != TextureFormat::RGBA && format != TextureFormat::DEPTH_COMPONENT,
                                                           format == TextureFormat::DEPTH_COMPONENT);

    for (auto i = 0; i < 6; i++)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internal_format, width_, height_, 0, gl_format,
                     gl_type, data[i]);
        i++;
    }

    bindless_handle_ = glGetTextureSamplerHandleARB(handle_, sampler->handle());
    glMakeTextureHandleResidentARB(bindless_handle_);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

CubeMap::~CubeMap()
{
    glMakeTextureHandleNonResidentARB(bindless_handle_);
    glDeleteTextures(1, &handle_);
}

GLuint CubeMap::handle() const
{
    return handle_;
}

GLuint64 CubeMap::bindless_handle() const
{
    return bindless_handle_;
}

std::uint32_t CubeMap::width() const
{
    return width_;
}

std::uint32_t CubeMap::height() const
{
    return height_;
}

std::int32_t CubeMap::index() const
{
    return index_;
}
}