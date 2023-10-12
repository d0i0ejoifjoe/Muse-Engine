#pragma once

#include "graphics/public/cube_map.h"
#include "graphics/public/sampler.h"
#include "graphics/public/texture.h"

#include <cstdint>
#include <memory>
#include <string_view>
#include <vector>

namespace muse
{

class TextureManager
{
  public:
    /**
     *
     *  Create a texture manager.
     *
     */
    TextureManager();

    /** Default destructor. */
    ~TextureManager() = default;

    /**
     *
     *  Load image and create a texture.
     *
     *  @param filename Path to image.
     *  @param sampler_index Index of sampler to use on texture (-1 if you want to use default sampler).
     *  @param format Format of texture.
     *  @param flip_image Whether to flip image when loaded.
     *
     *  @return Pointer to newly created texture.
     *
     */
    Texture *load(std::string_view filename,
                  std::int32_t sampler_index,
                  TextureFormat format,
                  bool flip_image);

    /**
     *
     *  Add new texture.
     *
     *  @param data Texels of texture.
     *  @param width Width of texture.
     *  @param height Height of texture.
     *  @param sampler_index Index of sampler to use on texture (-1 if you want to use default sampler).
     *  @param format Format of texture.
     *
     *  @return Pointer to newly created texture.
     *
     */
    Texture *add(std::byte *data,
                 std::uint32_t width,
                 std::uint32_t height,
                 std::int32_t sampler_index,
                 std::uint32_t color_channels,
                 TextureFormat format);

    /**
     *
     *  Remove a texture.
     *
     *  @param index Index of texture.
     *
     */
    void remove_texture(std::uint32_t index);

    /**
     *
     *  Add new sampler.
     *
     *  @param spec Sampler specification.
     *
     *  @return Pointer to newly created sampler.
     *
     */
    Sampler *add(const SamplerSpecification &spec);

    /**
     *
     *  Remove a sampler.
     *
     *  @param index Index of sampler.
     *
     */
    void remove_sampler(std::uint32_t index);

    /**
     *
     *  Load all six images and create a cubemap.
     *
     *  @param left_filename Path to left side image.
     *  @param right_filename Path to right side image.
     *  @param up_filename Path to up side image.
     *  @param down_filename Path to down side image.
     *  @param front_filename Path to front side image.
     *  @param back_filename Path to back side image.
     *  @param sampler_index Index of sampler to use on cubemap (can't have mipmaps, -1 if you want to use default sampler).
     *  @param format Format of all images.
     *  @param flip_images Whether to flip images when loaded.
     *
     *  @return Pointer to newly created cubemap.
     *
     */
    CubeMap *load(std::string_view left_filename,
                  std::string_view right_filename,
                  std::string_view up_filename,
                  std::string_view down_filename,
                  std::string_view front_filename,
                  std::string_view back_filename,
                  std::int32_t sampler_index,
                  TextureFormat format,
                  bool flip_images);

    /**
     *
     *  Create a cubemap.
     *
     *  @param left_data Data of left side image.
     *  @param right_data Data of right side image.
     *  @param up_data Data of up side image.
     *  @param down_data Data of down side image.
     *  @param front_data Data of front side image.
     *  @param back_data Data of back side image.
     *  @param width Width of all six images (needs to match for all images).
     *  @param height Height of all six images (needs to match for all images).
     *  @param sampler_index Index of sampler to use on cubemap (can't have mipmaps, -1 if you want to use default sampler).
     *  @param format Format of all images.
     *
     *  @return Pointer to newly created cubemap.
     *
     */
    CubeMap *add(std::byte *left_data,
                 std::byte *right_data,
                 std::byte *up_data,
                 std::byte *down_data,
                 std::byte *front_data,
                 std::byte *back_data,
                 std::uint32_t width,
                 std::uint32_t height,
                 std::int32_t sampler_index,
                 std::uint32_t color_channels,
                 TextureFormat format);

    /**
     *
     *  Remove a cubemap.
     *
     *  @param index Index of cubemap.
     *
     */
    void remove_cubemap(std::uint32_t index);

    /**
     *
     *  Get sampler.
     *
     *  @param index Index of sampler.
     *
     *  @return Pointer to sampler.
     *
     */
    Sampler *sampler(std::uint32_t index);

    /**
     *
     *  Get texture.
     *
     *  @param index Index of texture.
     *
     *  @return Pointer to texture.
     *
     */
    Texture *texture(std::uint32_t index);

    /**
     *
     *  Get cubemap.
     *
     *  @param index Index of cubemap.
     *
     *  @return Pointer to cubemap.
     *
     */
    CubeMap *cubemap(std::uint32_t index);

    /**
     *
     *  Get default texture sampler.
     *
     *  @return Pointer to default sampler.
     *
     */
    Sampler *default_texture_sampler();

    /**
     *
     *  Get default cubemap sampler.
     *
     *  @return Pointer to default sampler.
     *
     */
    Sampler *default_cubemap_sampler();

  private:
    /** Textures */
    std::vector<std::unique_ptr<Texture>> textures_;

    /** Cubemaps */
    std::vector<std::unique_ptr<CubeMap>> cube_maps_;

    /** Samplers */
    std::vector<std::unique_ptr<Sampler>> samplers_;

    /** Counters */
    std::int32_t texture_counter_;
    std::int32_t cubemap_counter_;
};
}