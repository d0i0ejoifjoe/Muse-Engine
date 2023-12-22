#pragma once

#include "glad.h"
#include "graphics/public/texture_format.h"
#include "utils/public/utils.h"

namespace muse
{

class Sampler;

/**
 *
 *  This class represents a set of pixels or as they called "texels (texture pixels)"
 *  these texels are stored on GPU this class helps us to manage the OpenGL handle and bindless handle that we get when creating texture.
 *  Handle it's a way to control the texture while bindless handle it's just id of sampler on GPU.
 *
 */

class Texture
{
  public:
    /**
     *
     *  Create texture for rendering.
     *
     *  @param width Width of render target.
     *  @param height Height of render target.
     *  @param format Format of render target.
     *  @param sampler Sampler for render target.
     *  @param index Index of texture.
     *
     */
    Texture(std::uint32_t width,
            std::uint32_t height,
            TextureFormat format,
            Sampler *sampler,
            std::int32_t index);

    /**
     *
     *  Create texture.
     *
     *  @param data Data of image.
     *  @param width Width of image.
     *  @param height Height of image.
     *  @param format Format of image.
     *  @param sampler Sampler to sample texture with.
     *  @param index Index of texture.
     *
     */
    Texture(const Data &data,
            std::uint32_t width,
            std::uint32_t height,
            TextureFormat format,
            Sampler *sampler,
            std::int32_t index);

    /**
     *
     *  Destroy texture.
     *
     */
    ~Texture();

    /**
     *
     *  Get handle.
     *
     *  @return Handle.
     *
     */
    GLuint handle() const;

    /**
     *
     *  Get width of texture.
     *
     *  @return Width.
     *
     */
    std::uint32_t width() const;

    /**
     *
     *  Get height of texture.
     *
     *  @return Height.
     *
     */
    std::uint32_t height() const;

    /**
     *
     *  Get texture format.
     *
     *  @return Format.
     *
     */
    TextureFormat format() const;

    /**
     *
     *  Get bindless handle.
     *
     *  @return Bindless handle.
     *
     */
    GLuint64 bindless_handle() const;

    /**
     *
     *  Get index of texture.
     *
     *  @return Index.
     *
     */
    std::int32_t index() const;

  private:
    /** Width */
    std::uint32_t width_;

    /** Height */
    std::uint32_t height_;

    /** Handle */
    GLuint handle_;

    /** Bindless handle */
    GLuint64 bindless_handle_;

    /** Texture format */
    TextureFormat format_;

    /** Index. */
    std::int32_t index_;
};
}