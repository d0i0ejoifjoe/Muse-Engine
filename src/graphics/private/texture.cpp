#include "graphics/public/texture.h"

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

Texture::Texture(std::uint32_t width,
                 std::uint32_t height,
                 TextureFormat format,
                 std::byte *data,
                 Sampler *sampler,
                 std::uint32_t color_channels,
                 std::int32_t index)
    : width_(width)
    , height_(height)
    , handle_(0)
    , bindless_handle_(0)
    , format_(format)
    , index_(index)
{
    glGenTextures(1, &handle_);
    glBindTexture(GL_TEXTURE_2D, handle_);

    auto [internal_format, gl_format, gl_type] = to_opengl(color_channels,
                                                           format_ != TextureFormat::RGBA && format_ != TextureFormat::DEPTH_COMPONENT,
                                                           format_ == TextureFormat::DEPTH_COMPONENT);

    glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width_, height_, 0, gl_format, gl_type, data);

    if (sampler->specification().use_mipmaps)
    {
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    bindless_handle_ = glGetTextureSamplerHandleARB(handle_, sampler->handle());
    glMakeTextureHandleResidentARB(bindless_handle_);

    glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::~Texture()
{
    glMakeTextureHandleNonResidentARB(bindless_handle_);
    glDeleteTextures(1, &handle_);
}

GLuint Texture::handle() const
{
    return handle_;
}

GLuint64 Texture::bindless_handle() const
{
    return bindless_handle_;
}

std::uint32_t Texture::width() const
{
    return width_;
}

std::uint32_t Texture::height() const
{
    return height_;
}

TextureFormat Texture::format() const
{
    return format_;
}

std::int32_t Texture::index() const
{
    return index_;
}

}