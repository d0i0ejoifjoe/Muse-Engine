#include "graphics/public/cube_map.h"

#include "graphics/public/sampler.h"

#include <array>

/**
 *
 *  Convert engine format to opengl format.
 *
 *  @param format Engine format.
 *
 *  @return Array with opengl types [internal_format, gl_format, gl_type].
 *
 */
std::array<GLenum, 3> to_opengl_format(muse::TextureFormat format)
{
    GLenum internal_format = GL_NONE;
    GLenum gl_format = GL_NONE;
    GLenum gl_type = GL_NONE;

    switch (format)
    {
        case muse::TextureFormat::RGBA:
            internal_format = GL_RGBA;
            gl_format = GL_RGBA;
            gl_type = GL_UNSIGNED_BYTE;
            break;
        case muse::TextureFormat::SRGB_ALPHA:
            internal_format = GL_SRGB_ALPHA;
            gl_format = GL_RGBA;
            gl_type = GL_UNSIGNED_BYTE;
            break;
        case muse::TextureFormat::DEPTH_COMPONENT:
            internal_format = GL_DEPTH_COMPONENT;
            gl_format = GL_DEPTH_COMPONENT;
            gl_type = GL_FLOAT;
            break;
    }

    return {internal_format, gl_format, gl_type};
}

namespace muse
{

CubeMap::CubeMap(std::byte *left_data, std::byte *right_data, std::byte *up_data, std::byte *down_data,
                 std::byte *front_data, std::byte *back_data, std::uint32_t width, std::uint32_t height,
                 TextureFormat format, Sampler *sampler, std::int32_t index)
    : width_(width)
    , height_(height)
    , handle_(0)
    , bindless_handle_(0)
    , index_(index)
{
    glGenTextures(1, &handle_);
    glBindTexture(GL_TEXTURE_CUBE_MAP, handle_);

    std::array<std::byte *, 6> data = {right_data, left_data, up_data, down_data, back_data, front_data};

    auto gl_formats = to_opengl_format(format);

    for (auto i = 0; i < 6; i++)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, gl_formats[0], width_, height_, 0, gl_formats[1],
                     gl_formats[2], data[i]);
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