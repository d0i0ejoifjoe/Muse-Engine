#include "graphics/public/texture_manager.h"

#include "log/public/logger.h"
#include "utils/public/utils.h"

#include <SDL2/SDL_image.h>
#include <algorithm>
#include <array>
#include <cassert>
#include <filesystem>
#include <fstream>
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

    auto *s = IMG_Load(filename);
    check_sdl_error();

    SDL_LockSurface(s);
    check_sdl_error();

    if (flip)
    {
        flip_image(s->pixels, s->w, s->h, s->format->BytesPerPixel);
    }

    if (!s->pixels || !s->w || !s->h || !s->format->BytesPerPixel)
    {
        LOG_WARN(ImageLoading, "Failed to read image file!\nImage path: {}", filename);
        std::abort();
    }

    data = reinterpret_cast<std::byte *>(s->pixels);
    width = s->w;
    height = s->h;
    color_channels = s->format->BytesPerPixel;

    SDL_UnlockSurface(s);
    check_sdl_error();

    return {data, width, height, color_channels};
}

namespace muse
{

TextureManager::TextureManager()
    : textures_()
    , cube_maps_()
    , samplers_()
    , texture_counter_(-1)
    , cubemap_counter_(-1)
{
    IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
    check_sdl_error();

    add(SamplerSpecification{});
    add(SamplerSpecification{.use_mipmaps = false});
}

Texture *TextureManager::load(std::string_view filename,
                              std::int32_t sampler_index,
                              TextureFormat format,
                              bool flip_image)
{
    auto [data, width, height, color_channels] = load_image(filename.data(), flip_image);
    return add(data, width, height, sampler_index, color_channels, format);
}

Texture *TextureManager::add(std::byte *data,
                             std::uint32_t width,
                             std::uint32_t height,
                             std::int32_t sampler_index,
                             std::uint32_t color_channels,
                             TextureFormat format)
{
    auto sampler = sampler_index == -1 ? default_texture_sampler() : this->sampler(sampler_index + 2);
    texture_counter_++;

    // clang-format off
    return textures_.emplace_back(std::make_unique<Texture>(width,
                                                            height,
                                                            format,
                                                            data,
                                                            sampler,
                                                            color_channels,
                                                            texture_counter_)).get();
    // clang-format on
}

void TextureManager::remove_texture(std::uint32_t index)
{
    textures_.erase(std::begin(textures_) + index);
}

Sampler *TextureManager::add(const SamplerSpecification &spec)
{
    return samplers_.emplace_back(std::make_unique<Sampler>(spec)).get();
}

void TextureManager::remove_sampler(std::uint32_t index)
{
    samplers_.erase(std::begin(samplers_) + (index + 2));
}

// FIXME: Something is wrong here i think with cubemaps
CubeMap *TextureManager::load(std::string_view left_filename,
                              std::string_view right_filename,
                              std::string_view up_filename,
                              std::string_view down_filename,
                              std::string_view front_filename,
                              std::string_view back_filename,
                              std::int32_t sampler_index,
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

    assert(match && "dimensions of all six images need to match");

    return add(data,
               std::get<0>(images[0]),
               std::get<0>(images[1]),
               std::get<0>(images[2]),
               std::get<0>(images[3]),
               std::get<0>(images[4]),
               width,
               height,
               sampler_index,
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
                             std::int32_t sampler_index,
                             std::uint32_t color_channels,
                             TextureFormat format)
{
    auto sampler = sampler_index == -1 ? default_cubemap_sampler() : this->sampler(sampler_index + 2);

    // clang-format off
    cubemap_counter_++;
    return cube_maps_.emplace_back(std::make_unique<CubeMap>(left_data,
                                                             right_data,
                                                             up_data,
                                                             down_data,
                                                             front_data,
                                                             back_data,
                                                             width,
                                                             height,
                                                             format,
                                                             sampler,
                                                             color_channels,
                                                             cubemap_counter_)).get();
    // clang-format on
}

void TextureManager::remove_cubemap(std::uint32_t index)
{
    cube_maps_.erase(std::begin(cube_maps_) + index);
}

Sampler *TextureManager::default_texture_sampler()
{
    return samplers_.at(0).get();
}

Sampler *TextureManager::default_cubemap_sampler()
{
    return samplers_.at(1).get();
}

Sampler *TextureManager::sampler(std::uint32_t index)
{
    return samplers_.at(index + 2).get();
}

Texture *TextureManager::texture(std::uint32_t index)
{
    return textures_.at(index).get();
}

CubeMap *TextureManager::cubemap(std::uint32_t index)
{
    return cube_maps_.at(index).get();
}
}