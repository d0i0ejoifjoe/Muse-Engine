#include "graphics/public/texture_manager.h"

#include "graphics/public/stb_image.h"
#include "log/public/logger.h"
#include "utils/public/utils.h"

#include <algorithm>
#include <array>
#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <tuple>

void flip_image(void *image, std::int32_t w, std::int32_t h, std::int32_t bytes_per_pixel)
{
    std::size_t bytes_per_row = w * bytes_per_pixel;
    std::uint8_t temp[2048];
    std::uint8_t *bytes = reinterpret_cast<std::uint8_t *>(image);

    for (auto row = 0; row < (h >> 1); row++)
    {
        std::uint8_t *row0 = bytes + row * bytes_per_row;
        std::uint8_t *row1 = bytes + (h - row - 1) * bytes_per_row;

        std::size_t bytes_left = bytes_per_row;
        while (bytes_left)
        {
            std::size_t bytes_copy = bytes_left < sizeof(temp) ? bytes_left : sizeof(temp);
            std::memcpy(temp, row0, bytes_copy);
            std::memcpy(row0, row1, bytes_copy);
            std::memcpy(row1, temp, bytes_copy);
            row0 += bytes_copy;
            row1 += bytes_copy;
            bytes_left -= bytes_copy;
        }
    }
}

/**
 *
 *  Load image.
 *
 *  @param filename Path to image.
 *  @param flip Whether to flip it on load.
 *
 *  @return Tuple with image data [texels, width, height, color_channels].
 *
 */
std::tuple<std::byte *, std::int32_t, std::int32_t, std::int32_t> load_image(const char *filename, bool flip)
{
    auto width = 0;
    auto height = 0;
    auto color_channels = 0;
    std::byte *data = nullptr;

    stbi_set_flip_vertically_on_load(flip);

    data = reinterpret_cast<std::byte *>(stbi_load(filename, &width, &height, &color_channels, 0));

    if (!width || !height || !color_channels || !data)
    {
        LOG_INFO(TextureManager, "Failed to load texture, path: {}\nError string: {}", filename, stbi_failure_reason());
    }

    LOG_INFO(TextureManager, "Image path: {}\nWidth: {}\nHeight: {}\nBytes per color: {}", filename, width, height, color_channels);

    return {data, width, height, color_channels};
}

namespace muse
{

TextureManager::TextureManager()
    : textures_()
    , cube_maps_()
    , samplers_()
{
}

Texture *TextureManager::load(std::string_view filename,
                              Sampler *sampler,
                              TextureFormat format,
                              bool flip_image)
{
    auto [data, width, height, color_channels] = load_image(filename.data(), flip_image);
    return add(data, width, height, sampler, color_channels, format);
}

Texture *TextureManager::add(std::byte *data,
                             std::uint32_t width,
                             std::uint32_t height,
                             Sampler *sampler,
                             std::uint32_t color_channels,
                             TextureFormat format)
{
    auto sampler_to_use = sampler ? sampler : default_texture_sampler();

    static std::int32_t texture_counter = -1;
    texture_counter++;

    // clang-format off
    return textures_.emplace_back(std::make_unique<Texture>(width,
                                                            height,
                                                            format,
                                                            data,
                                                            sampler_to_use,
                                                            color_channels,
                                                            texture_counter)).get();
    // clang-format on
}

Sampler *TextureManager::add(const SamplerSpecification &spec)
{
    return samplers_.emplace_back(std::make_unique<Sampler>(spec)).get();
}

// FIXME: Something is wrong here i think with cubemaps
CubeMap *
TextureManager::load(std::string_view left_filename,
                     std::string_view right_filename,
                     std::string_view up_filename,
                     std::string_view down_filename,
                     std::string_view front_filename,
                     std::string_view back_filename,
                     Sampler *sampler,
                     TextureFormat format,
                     bool flip_images)
{
    auto [data, width, height, color_channels] = load_image(left_filename.data(), flip_images);

    // clang-format off
    std::array<std::tuple<std::byte *, std::uint32_t, std::uint32_t, std::uint32_t>, 5> images{
        load_image(right_filename.data(), flip_images),
        load_image(up_filename.data(), flip_images),
        load_image(down_filename.data(), flip_images),
        load_image(front_filename.data(), flip_images),
        load_image(back_filename.data(), flip_images),
    };
    // clang-format on

    auto match = std::all_of(std::begin(images), std::end(images), [&](const auto &image)
                             {
                                 return std::get<1>(image) == static_cast<const std::uint32_t>(width) &&
                                        std::get<2>(image) == static_cast<const std::uint32_t>(height) &&
                                        std::get<3>(image) == static_cast<const std::uint32_t>(color_channels);
                             });

    assert(match && "dimensions and bytes per color of all six images need to match");

    return add(data,
               std::get<0>(images[0]),
               std::get<0>(images[1]),
               std::get<0>(images[2]),
               std::get<0>(images[3]),
               std::get<0>(images[4]),
               width,
               height,
               sampler,
               color_channels,
               format);
}

CubeMap *TextureManager::add(std::byte *left_data,
                             std::byte *right_data,
                             std::byte *up_data,
                             std::byte *down_data,
                             std::byte *front_data,
                             std::byte *back_data,
                             std::uint32_t width,
                             std::uint32_t height,
                             Sampler *sampler,
                             std::uint32_t color_channels,
                             TextureFormat format)
{
    auto sampler_to_use = sampler ? sampler : default_cubemap_sampler();

    // clang-format off
    static std::int32_t cubemap_counter = -1;
    cubemap_counter++;

    return cube_maps_.emplace_back(std::make_unique<CubeMap>(left_data,
                                                             right_data,
                                                             up_data,
                                                             down_data,
                                                             front_data,
                                                             back_data,
                                                             width,
                                                             height,
                                                             format,
                                                             sampler_to_use,
                                                             color_channels,
                                                             cubemap_counter)).get();
    // clang-format on
}

void TextureManager::remove_cubemap(std::uint32_t index)
{
    cube_maps_.erase(std::begin(cube_maps_) + index);
}

void TextureManager::remove_texture(std::uint32_t index)
{
    textures_.erase(std::begin(textures_) + index);
}

void TextureManager::remove_sampler(std::uint32_t index)
{
    samplers_.erase(std::begin(samplers_) + index);
}

Sampler *TextureManager::default_texture_sampler() const
{
    static Sampler default_sampler{SamplerSpecification{}};
    return &default_sampler;
}

Sampler *TextureManager::default_cubemap_sampler() const
{
    static Sampler default_cubemap_sampler{SamplerSpecification{.use_mipmaps = false}};
    return &default_cubemap_sampler;
}

Texture *TextureManager::blank_texture() const
{
    static std::array<std::byte, 4> blank_color = {std::byte{0xff},
                                                   std::byte{0xff},
                                                   std::byte{0xff},
                                                   std::byte{0xff}};

    static Texture blank{1,
                         1,
                         TextureFormat::SRGB_ALPHA,
                         blank_color.data(),
                         default_texture_sampler(),
                         4,
                         -1};

    return &blank;
}

CubeMap *TextureManager::blank_cubemap() const
{
    static std::array<std::byte, 4> blank_color = {std::byte{0xff},
                                                   std::byte{0xff},
                                                   std::byte{0xff},
                                                   std::byte{0xff}};

    static CubeMap blank{blank_color.data(),
                         blank_color.data(),
                         blank_color.data(),
                         blank_color.data(),
                         blank_color.data(),
                         blank_color.data(),
                         1,
                         1,
                         TextureFormat::SRGB_ALPHA,
                         default_cubemap_sampler(),
                         4,
                         -1};

    return &blank;
}

Sampler *TextureManager::sampler(std::uint32_t index) const
{
    return samplers_.at(index).get();
}

Texture *TextureManager::texture(std::uint32_t index) const
{
    return textures_.at(index).get();
}

CubeMap *TextureManager::cubemap(std::uint32_t index) const
{
    return cube_maps_.at(index).get();
}

std::size_t TextureManager::texture_count() const
{
    return textures_.size();
}

std::size_t TextureManager::cubemap_count() const
{
    return cube_maps_.size();
}

std::size_t TextureManager::sampler_count() const
{
    return samplers_.size();
}

}