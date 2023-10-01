#include "graphics/public/texture.h"
#include "graphics/public/sampler.h"

namespace muse
{
    Texture::Texture(std::uint32_t width,
                     std::uint32_t height,
                     TextureFormat format,
                     std::byte* data,
                     Sampler* sampler)
        : width_(width)
        , height_(height)
        , handle_(0)
        , bindless_handle_(0)
        , format_(format)
    {
        glGenBuffers(1, &handle_);
        glBindTexture(GL_TEXTURE_2D, handle_);

        switch(format_)
        {
        case TextureFormat::RGBA: glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, width_, height_, 0, GL_RGBA, GL_UNSIGNED_BYTE, data); break;
        case TextureFormat::SRGB_ALPHA: glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width_, height_, 0, GL_RGBA, GL_UNSIGNED_BYTE, data); break;
        case TextureFormat::DEPTH_COMPONENT: glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width_, height_, 0, GL_DEPTH_COMPONENT, GL_FLOAT, data); break;
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
}