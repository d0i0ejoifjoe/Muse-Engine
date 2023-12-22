#pragma once

#include "graphics/public/cube_map.h"
#include "graphics/public/sampler.h"
#include "graphics/public/texture.h"
#include "utils/public/file_manager.h"

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
    TextureManager(FileManager *fmanager);

    /** Default destructor. */
    ~TextureManager() = default;

    /**
     *
     *  Load image and create a texture.
     *
     *  @param filename Path to image.
     *  @param sampler Sampler to use (nullptr for default).
     *  @param gamma_correct Whether to do gamma correction on image.
     *  @param flip_image Whether to flip image when loaded.
     *
     *  @return Pointer to newly created texture.
     *
     */
    Texture *load_texture(std::string_view filename,
                          Sampler *sampler,
                          bool gamma_correct,
                          bool flip_image);

    /**
     *
     *  Add new texture.
     *
     *  @param data Texels of texture.
     *  @param width Width of texture.
     *  @param height Height of texture.
     *  @param sampler Sampler to use (nullptr for default).
     *  @param format Format of texture.
     *
     *  @return Pointer to newly created texture.
     *
     */
    Texture *add_texture(const Data &data,
                         std::uint32_t width,
                         std::uint32_t height,
                         Sampler *sampler,
                         TextureFormat format);

    /**
     *
     *  Add new texture.
     *
     *  @param width Width of texture.
     *  @param height Height of texture.
     *  @param sampler Sampler to use (nullptr for default).
     *  @param format Format of texture.
     *
     *  @return Pointer to newly created texture.
     *
     */
    Texture *add_texture(std::uint32_t width,
                         std::uint32_t height,
                         Sampler *sampler,
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
     *  Get a blank texture.
     *
     *  @return Pointer to blank texture (just white).
     *
     */
    Texture *blank_texture() const;

    /**
     *
     *  Get a blank cubemap.
     *
     *  @return Pointer to blank texture (just white).
     *
     */
    CubeMap *blank_cubemap() const;

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
     *  @param sampler Sampler to use (nullptr for default).
     *  @param format Format of all images.
     *  @param gamma_correct Whether to load all images with gamma correction.
     *  @param flip_images Whether to flip images when loaded.
     *
     *  @return Pointer to newly created cubemap.
     *
     */
    CubeMap *load_cubemap(std::string_view left_filename,
                          std::string_view right_filename,
                          std::string_view up_filename,
                          std::string_view down_filename,
                          std::string_view front_filename,
                          std::string_view back_filename,
                          Sampler *sampler,
                          bool gamma_correct,
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
     *  @param sampler Sampler to use (nullptr for default).
     *  @param format Format of all images.
     *
     *  @return Pointer to newly created cubemap.
     *
     */
    CubeMap *add_cubemap(const Data &left_data,
                         const Data &right_data,
                         const Data &up_data,
                         const Data &down_data,
                         const Data &front_data,
                         const Data &back_data,
                         std::uint32_t width,
                         std::uint32_t height,
                         Sampler *sampler,
                         TextureFormat format);
    /**
     *
     *  Create a cubemap.
     *
     *  @param width Width of all six images (needs to match for all images).
     *  @param height Height of all six images (needs to match for all images).
     *  @param sampler Sampler to use (nullptr for default).
     *  @param format Format of all images.
     *
     *  @return Pointer to newly created cubemap.
     *
     */
    CubeMap *add_cubemap(std::uint32_t width,
                         std::uint32_t height,
                         Sampler *sampler,
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
    Sampler *sampler(std::uint32_t index) const;

    /**
     *
     *  Get texture.
     *
     *  @param index Index of texture.
     *
     *  @return Pointer to texture.
     *
     */
    Texture *texture(std::uint32_t index) const;

    /**
     *
     *  Get cubemap.
     *
     *  @param index Index of cubemap.
     *
     *  @return Pointer to cubemap.
     *
     */
    CubeMap *cubemap(std::uint32_t index) const;

    /**
     *
     *  Get default texture sampler.
     *
     *  @return Pointer to default sampler.
     *
     */
    Sampler *default_texture_sampler() const;

    /**
     *
     *  Get default cubemap sampler.
     *
     *  @return Pointer to default sampler.
     *
     */
    Sampler *default_cubemap_sampler() const;

    /**
     *
     *  Get texture count.
     *
     *  @return Texture count.
     *
     */
    std::size_t texture_count() const;

    /**
     *
     *  Get cubemap count.
     *
     *  @return Cubemap count.
     *
     */
    std::size_t cubemap_count() const;

    /**
     *
     *  Get sampler count.
     *
     *  @return Sampler count.
     *
     */
    std::size_t sampler_count() const;

  private:
    /** File manager. */
    FileManager *fmanager_;

    /** Textures */
    std::vector<std::unique_ptr<Texture>> textures_;

    /** Cubemaps */
    std::vector<std::unique_ptr<CubeMap>> cube_maps_;

    /** Samplers */
    std::vector<std::unique_ptr<Sampler>> samplers_;
};

}