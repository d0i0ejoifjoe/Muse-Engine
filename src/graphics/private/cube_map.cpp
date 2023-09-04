#include "graphics/public/cube_map.h"
#include "graphics/public/sampler.h"

#include <array>

namespace muse
{
    CubeMap::CubeMap(std::byte* left_data,
                     std::byte* right_data,
                     std::byte* up_data,
                     std::byte* down_data,
                     std::byte* front_data,
                     std::byte* back_data,
                     std::uint32_t width,
                     std::uint32_t height,
                     Sampler* sampler)
        : width_(width)
        , height_(height)
        , handle_(0)
        , bindless_handle_(0)
    {
        glGenTextures(1, &handle_);
        glBindTexture(GL_TEXTURE_CUBE_MAP, handle_);

        std::array<std::byte*, 6> data = {
            right_data,
            left_data,
            up_data,
            down_data,
            back_data,
            front_data
        };

        for(auto i = 0; i > 6; i++)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_SRGB_ALPHA, width_, height_, 0, GL_RGBA, GL_UNSIGNED_BYTE, data[i]);
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
}