#include "graphics/public/texture_manager.h"

#define STB_IMAGE_IMPLEMENTATION 1
#include "graphics/public/stb_image.h"

#include <algorithm>
#include <array>
#include <cassert>
#include <tuple>

/**
 *
 *  Load image.
 *
 *  @param filename Path to image.
 *  @param flip Whether to flip it on load.
 *
 *  @return Tuple with image data [texels, width, height].
 *
 */
std::tuple<std::byte *, std::int32_t, std::int32_t> load_image(const char *filename, bool flip)
{
    std::int32_t width = 0;
    std::int32_t height = 0;
    std::byte *data = nullptr;

    std::int32_t dummy = 0;

    stbi_set_flip_vertically_on_load(flip);

    data = reinterpret_cast<std::byte *>(stbi_load(filename, &width, &height, &dummy, 0));

    assert(data != nullptr && "failed to load image");

    return {data, width, height};
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

    add(SamplerSpecification{});
    add(SamplerSpecification{.use_mipmaps = false});
}

Texture *TextureManager::load(std::string_view filename,
                              std::int32_t sampler_index,
                              TextureFormat format,
                              bool flip_image)
{
    auto [data, width, height] = load_image(filename.data(), flip_image);
    return add(data, width, height, sampler_index, format);
}

Texture *TextureManager::add(std::byte *data,
                             std::uint32_t width,
                             std::uint32_t height,
                             std::int32_t sampler_index,
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
    auto [data, width, height] = load_image(left_filename.data(), flip_images);

    // clang-format off
    std::array<std::tuple<std::byte *, std::uint32_t, std::uint32_t>, 5> images{
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
                                        std::get<2>(image) == static_cast<const std::uint32_t>(height);
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