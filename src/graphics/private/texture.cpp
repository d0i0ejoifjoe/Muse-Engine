#include "graphics/public/texture.h"

#include "graphics/public/sampler.h"
#include "log/public/logger.h"

#include <tuple>

/**
 *
 *  Get internal format opengl enum.
 *
 *  @param engine_format Engine format.
 *
 *  @return GL format.
 *
 */
GLenum to_opengl(muse::TextureFormat engine_format)
{
    GLenum format = GL_NONE;

    switch (engine_format)
    {
        case muse::TextureFormat::RED: format = GL_RED; break;
        case muse::TextureFormat::RG: format = GL_RG; break;
        case muse::TextureFormat::RGB: format = GL_RGB; break;
        case muse::TextureFormat::RGBA: format = GL_RGBA; break;
        case muse::TextureFormat::SRGB: format = GL_SRGB; break;
        case muse::TextureFormat::SRGBA: format = GL_SRGB_ALPHA; break;
        case muse::TextureFormat::REDF16: format = GL_R16F; break;
        case muse::TextureFormat::RGF16: format = GL_RG16F; break;
        case muse::TextureFormat::RGBF16: format = GL_RGB16F; break;
        case muse::TextureFormat::RGBAF16: format = GL_RGBA16F; break;
        case muse::TextureFormat::DEPTH_COMPONENT: format = GL_DEPTH_COMPONENT; break;
    }

    return format;
}

/**
 *
 *  Get the texture format based on internal format.
 *
 *  @param internal_format Internal format.
 *
 *  @return GL format.
 *
 */
inline GLenum texture_format(GLenum internal_format)
{
    GLenum format = GL_NONE;

    switch (internal_format)
    {
        case GL_RED:
        case GL_RG:
        case GL_RGB:
        case GL_RGBA: return internal_format;
        case GL_SRGB: format = GL_RGB; break;
        case GL_SRGB_ALPHA: format = GL_RGBA; break;
        case GL_R16F: format = GL_RED; break;
        case GL_RG16F: format = GL_RG; break;
        case GL_RGB16F: format = GL_RGB; break;
        case GL_RGBA16F: format = GL_RGBA; break;
        case GL_DEPTH_COMPONENT: format = GL_DEPTH_COMPONENT; break;
    }

    return format;
}

/**
 *
 *  Get texture data type.
 *
 *  @param internal_format Internal format.
 *
 *  @return GLEnum for data type.
 *
 */
GLenum to_opengl(GLenum internal_format)
{
    switch (internal_format)
    {
        case GL_RED:
        case GL_RG:
        case GL_RGB:
        case GL_RGBA:
        case GL_SRGB:
        case GL_SRGB_ALPHA: return GL_UNSIGNED_BYTE;
        case GL_R16F:
        case GL_RG16F:
        case GL_RGB16F:
        case GL_RGBA16F:
        case GL_DEPTH_COMPONENT: return GL_FLOAT;
    }

    return GL_NONE;
}

namespace muse
{

Texture::Texture(std::uint32_t width,
                 std::uint32_t height,
                 TextureFormat format,
                 Sampler *sampler,
                 std::int32_t index)
    : Texture(Data(), width, height, format, sampler, index)
{
}

Texture::Texture(const Data &data,
                 std::uint32_t width,
                 std::uint32_t height,
                 TextureFormat format,
                 Sampler *sampler,
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

    auto gl_internal = to_opengl(format_);
    auto gl_format = texture_format(gl_internal);
    auto gl_type = to_opengl(gl_internal);

    const auto &spec = sampler->specification();
    if (spec.use_mipmaps)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, static_cast<GLint>(std::floor(std::log2(std::max(width_, height_)))) + 1);
    }

    glTexImage2D(GL_TEXTURE_2D, 0, gl_internal, width_, height_, 0, gl_format, gl_type, data.empty() ? nullptr : reinterpret_cast<const float *>(data.data()));

    if (spec.use_mipmaps)
    {
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    bindless_handle_ = glGetTextureSamplerHandleARB(handle_, sampler->handle());
    glMakeTextureHandleResidentARB(bindless_handle_);

    glBindTexture(GL_TEXTURE_2D, 0);

    LOG_INFO(Texture, "Texture created!");
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