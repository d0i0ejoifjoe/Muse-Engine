#pragma once

#include "glad.h"
#include "graphics/public/texture_format.h"
#include "utils/public/utils.h"

namespace muse
{

class Sampler;

/**
 *
 *  Class that represents a cubemap.
 *  This class represents cube of which every side has it's own texture
 *  the texels of each side are stored on GPU and this class helps us to manage the OpenGL handle and bindless handle that we get when creating cubemap.
 *  Handle it's a way to control the cubemap while bindless handle it's just id of sampler on GPU.
 *
 */
class CubeMap
{
  public:
    /**
     *
     *  Create cubemap for rendering.
     *
     *  @param width Width of all sides.
     *  @param height Height of all sides.
     *  @param format Format of all sides.
     *  @param sampler Sampler to sample cubemap with.
     *  @param index Index of cubemap.
     *
     */
    CubeMap(std::uint32_t width,
            std::uint32_t height,
            TextureFormat format,
            Sampler *sampler,
            std::int32_t index);

    /**
     *
     *  Create cubemap.
     *
     *  @param left_data Data of left side.
     *  @param right_data Data of right side.
     *  @param up_data Data of up side.
     *  @param down_data Data of down side.
     *  @param front_data Data of front side.
     *  @param back_data Data of back side.
     *  @param width Width of all sides.
     *  @param height Height of all sides.
     *  @param format Format of all sides.
     *  @param sampler Sampler to sample cubemap with.
     *  @param index Index of cubemap.
     *
     */
    CubeMap(const Data &left_data,
            const Data &right_data,
            const Data &up_data,
            const Data &down_data,
            const Data &front_data,
            const Data &back_data,
            std::uint32_t width,
            std::uint32_t height,
            TextureFormat format,
            Sampler *sampler,
            std::int32_t index);

    /**
     *
     *  Destroy cubemap.
     *
     */
    ~CubeMap();

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
     *  Get bindless handle.
     *
     *  @return Bindless handle.
     *
     */
    GLuint64 bindless_handle() const;

    /**
     *
     *  Get width of all sides.
     *
     *  @return Width.
     *
     */
    std::uint32_t width() const;

    /**
     *
     *  Get height of all sides.
     *
     *  @return Height.
     *
     */
    std::uint32_t height() const;

    /**
     *
     *  Get index of cubemap.
     *
     *  @return Index.
     *
     */
    std::int32_t index() const;

    /**
     *
     *  Get texture format of cubemap.
     *
     *  @return Texture format.
     *
     */
    TextureFormat format() const;

  private:
    /** Width */
    std::uint32_t width_;

    /** Height */
    std::uint32_t height_;

    /** Handle */
    GLuint handle_;

    /** Bindless handle */
    GLuint64 bindless_handle_;

    /** Texture format. */
    TextureFormat format_;

    /** Index */
    std::int32_t index_;
};

}