#include "graphics/public/cube_map.h"

#include "graphics/public/sampler.h"

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
inline GLenum to_opengl(muse::TextureFormat engine_format)
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
inline GLenum to_opengl(GLenum internal_format)
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

CubeMap::CubeMap(std::uint32_t width,
                 std::uint32_t height,
                 TextureFormat format,
                 Sampler *sampler,
                 std::int32_t index)
    : CubeMap(Data(), Data(), Data(), Data(), Data(), Data(), width, height, format, sampler, index)
{
}

CubeMap::CubeMap(const Data &left_data,
                 const Data &right_data,
                 const Data &up_data,
                 const Data &down_data,
                 const Data &front_data,
                 const Data &back_data,
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
    glBindTexture(GL_TEXTURE_CUBE_MAP, handle_);

    auto gl_internal = to_opengl(format_);
    auto gl_format = texture_format(gl_internal);
    auto gl_type = to_opengl(gl_internal);

    const auto &spec = sampler->specification();
    if (spec.use_mipmaps)
    {
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, static_cast<GLint>(std::floor(std::log2(std::max(width_, height_)))) + 1);
    }

    std::array<const std::byte *, 6>
        data = {right_data.empty() ? nullptr : right_data.data(),
                left_data.empty() ? nullptr : left_data.data(),
                up_data.empty() ? nullptr : up_data.data(),
                down_data.empty() ? nullptr : down_data.data(),
                front_data.empty() ? nullptr : front_data.data(),
                back_data.empty() ? nullptr : back_data.data()};

    for (auto i = 0; i < 6; i++)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, gl_internal, width_, height_, 0, gl_format,
                     gl_type, data[i] ? data[i] : nullptr);
    }

    if (spec.use_mipmaps)
    {
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    }

    bindless_handle_ = glGetTextureSamplerHandleARB(handle_, sampler->handle());
    glMakeTextureHandleResidentARB(bindless_handle_);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    LOG_INFO(CubeMap, "Cube map created!");
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

TextureFormat CubeMap::format() const
{
    return format_;
}
}