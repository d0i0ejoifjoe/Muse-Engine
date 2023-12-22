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

/** Global state */
static std::int32_t texture_counter = -1;
static std::int32_t cubemap_counter = -1;

/**
 *
 *  Get format from number of color channels.
 *
 *  @param color_channels Number of channels.
 *  @param gamma_correct Whether gamma correct or not (only works for more than 2 channels).
 *  @param is_float Whether the image was stored as floating-point as opposed to bytes.
 *
 */
muse::TextureFormat get_format(std::uint32_t color_channels, bool gamma_correct, bool is_float)
{
    muse::TextureFormat format = muse::TextureFormat::RGBA;

    assert(!(gamma_correct && is_float) && "incompatable parameters");

    switch (color_channels)
    {
        case 1: format = is_float ? muse::TextureFormat::REDF16 : muse::TextureFormat::RED; break;
        case 2: format = is_float ? muse::TextureFormat::RGF16 : muse::TextureFormat::RG; break;
        case 3: format = !is_float ? gamma_correct ? muse::TextureFormat::SRGB : muse::TextureFormat::RGB : muse::TextureFormat::RGBF16; break;
        case 4: format = !is_float ? gamma_correct ? muse::TextureFormat::SRGBA : muse::TextureFormat::RGBA : muse::TextureFormat::RGBAF16; break;
    }

    return format;
}

/**
 *
 *  Load image.
 *
 *  @param filename Path to image.
 *  @param flip Whether to flip it on load.
 *  @param is_float Whether the image that we want to load is gonna be in .hdr format.
 *  @param fmanager File manager.
 *
 *  @return Tuple with image data [texels, width, height, color_channels].
 *
 */
std::tuple<muse::Data, std::int32_t, std::int32_t, std::int32_t> load_image(const char *filename, bool flip, bool is_float, muse::FileManager *fmanager)
{
    auto width = 0;
    auto height = 0;
    auto color_channels = 0;
    std::byte *data_ptr = nullptr;

    static std::int32_t texture_index = -1;
    texture_index++;

    const auto &data_buffer = fmanager->load(filename, "texture_" + std::to_string(texture_index));

    stbi_set_flip_vertically_on_load(flip);

    if (is_float)
    {
        data_ptr = reinterpret_cast<std::byte *>(stbi_loadf_from_memory(reinterpret_cast<const stbi_uc *>(data_buffer.data()),
                                                                        data_buffer.size(),
                                                                        &width,
                                                                        &height,
                                                                        &color_channels,
                                                                        0));
    }
    else
    {

        data_ptr = reinterpret_cast<std::byte *>(stbi_load_from_memory(reinterpret_cast<const stbi_uc *>(data_buffer.data()),
                                                                       data_buffer.size(),
                                                                       &width,
                                                                       &height,
                                                                       &color_channels,
                                                                       0));
    }

    muse::Data data{data_ptr, data_ptr + width * height * color_channels * (is_float ? sizeof(float) : 1)};

    LOG_INFO(TextureManager, "Image path: {}\nWidth: {}\nHeight: {}\nBytes per color: {}", filename, width, height, color_channels);

    return {data, width, height, color_channels};
}

namespace muse
{

TextureManager::TextureManager(FileManager *fmanager)
    : fmanager_(fmanager)
    , textures_()
    , cube_maps_()
    , samplers_()
{
    LOG_INFO(TextureManager, "Texture manager created!");
}

Texture *TextureManager::load_texture(std::string_view filename,
                                      Sampler *sampler,
                                      bool gamma_correct,
                                      bool flip_image)
{
    bool is_hdr = filename.substr(filename.find_last_of('.') + 1) == "hdr";

    auto [data, width, height, color_channels] = load_image(filename.data(), flip_image, is_hdr, fmanager_);
    auto format = get_format(color_channels, gamma_correct, is_hdr);
    return add_texture(data, width, height, sampler, format);
}

Texture *TextureManager::add_texture(const Data &data,
                                     std::uint32_t width,
                                     std::uint32_t height,
                                     Sampler *sampler,
                                     TextureFormat format)
{
    auto sampler_to_use = sampler ? sampler : default_texture_sampler();

    texture_counter++;

    // clang-format off
    return textures_.emplace_back(std::make_unique<Texture>(data,
                                                            width,
                                                            height,
                                                            format,
                                                            sampler_to_use,
                                                            texture_counter)).get();
    // clang-format on
}

Texture *TextureManager::add_texture(std::uint32_t width,
                                     std::uint32_t height,
                                     Sampler *sampler,
                                     TextureFormat format)
{
    auto sampler_to_use = sampler ? sampler : default_texture_sampler();

    texture_counter++;

    // clang-format off
    return textures_.emplace_back(std::make_unique<Texture>(width,
                                                            height,
                                                            format,
                                                            sampler_to_use,
                                                            texture_counter)).get();
    // clang-format on
}

Sampler *TextureManager::add(const SamplerSpecification &spec)
{
    return samplers_.emplace_back(std::make_unique<Sampler>(spec)).get();
}

// FIXME: Something is wrong here i think with cubemaps
CubeMap *
TextureManager::load_cubemap(std::string_view left_filename,
                             std::string_view right_filename,
                             std::string_view up_filename,
                             std::string_view down_filename,
                             std::string_view front_filename,
                             std::string_view back_filename,
                             Sampler *sampler,
                             bool gamma_correct,
                             bool flip_images)
{
    auto is_hdr = left_filename.substr(left_filename.find_last_of('.') + 1) == "hdr";

    auto [data, width, height, color_channels] = load_image(left_filename.data(), flip_images, is_hdr, fmanager_);
    auto format = get_format(color_channels, gamma_correct, is_hdr);

    // clang-format off
    std::array<std::tuple<Data, std::uint32_t, std::uint32_t, std::uint32_t>, 5> images{
        load_image(right_filename.data(), flip_images, right_filename.substr(right_filename.find_last_of('.') + 1) == "hdr", fmanager_),
        load_image(up_filename.data(), flip_images, up_filename.substr(up_filename.find_last_of('.') + 1) == "hdr", fmanager_),
        load_image(down_filename.data(), flip_images, down_filename.substr(down_filename.find_last_of('.') + 1) == "hdr", fmanager_),
        load_image(front_filename.data(), flip_images, front_filename.substr(front_filename.find_last_of('.') + 1) == "hdr", fmanager_),
        load_image(back_filename.data(), flip_images, back_filename.substr(back_filename.find_last_of('.') + 1) == "hdr", fmanager_),
    };
    // clang-format on

    auto match = std::all_of(std::begin(images), std::end(images), [&](const auto &image)
                             {
                                 return std::get<1>(image) == static_cast<const std::uint32_t>(width) &&
                                        std::get<2>(image) == static_cast<const std::uint32_t>(height) &&
                                        std::get<3>(image) == static_cast<const std::uint32_t>(color_channels);
                             });

    assert(match && "dimensions and bytes per color of all six images need to match");

    return add_cubemap(data,
                       std::get<0>(images[0]),
                       std::get<0>(images[1]),
                       std::get<0>(images[2]),
                       std::get<0>(images[3]),
                       std::get<0>(images[4]),
                       width,
                       height,
                       sampler,
                       format);
}

CubeMap *TextureManager::add_cubemap(const Data &left_data,
                                     const Data &right_data,
                                     const Data &up_data,
                                     const Data &down_data,
                                     const Data &front_data,
                                     const Data &back_data,
                                     std::uint32_t width,
                                     std::uint32_t height,
                                     Sampler *sampler,
                                     TextureFormat format)
{
    auto sampler_to_use = sampler ? sampler : default_cubemap_sampler();

    // clang-format off
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
                                                             cubemap_counter)).get();
    // clang-format on
}

CubeMap *TextureManager::add_cubemap(std::uint32_t width,
                                     std::uint32_t height,
                                     Sampler *sampler,
                                     TextureFormat format)
{
    auto sampler_to_use = sampler ? sampler : default_cubemap_sampler();

    // clang-format off
    cubemap_counter++;

    return cube_maps_.emplace_back(std::make_unique<CubeMap>(width,
                                                             height,
                                                             format,
                                                             sampler_to_use,
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
    static Data data = {std::byte{0xFF},
                        std::byte{0xFF},
                        std::byte{0xFF},
                        std::byte{0xFF}};

    static Texture blank(data, 1, 1, TextureFormat::SRGBA, default_texture_sampler(), -1);

    return &blank;
}

CubeMap *TextureManager::blank_cubemap() const
{
    static Data data = {std::byte{0xFF},
                        std::byte{0xFF},
                        std::byte{0xFF},
                        std::byte{0xFF}};

    static CubeMap blank(data, data, data, data, data, data, 1, 1, muse::TextureFormat::SRGBA, default_cubemap_sampler(), -1);

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