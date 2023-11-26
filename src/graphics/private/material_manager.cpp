#include "graphics/public/material_manager.h"

#include "graphics/public/material.h"
#include "graphics/public/texture_manager.h"

muse::MaterialMaps get_maps(const muse::MaterialIndices &indices, const muse::TextureManager *tmanager)
{
    muse::MaterialMaps maps{};

    if (indices.albedo == std::numeric_limits<std::uint32_t>::max())
    {
        LOG_WARN(Material, "No albedo texture");
        maps.albedo = nullptr;
    }
    else
    {
        maps.albedo = tmanager->texture(indices.albedo);
    }

    if (indices.normal == std::numeric_limits<std::uint32_t>::max())
    {
        LOG_WARN(Material, "No normal texture");
        maps.normal = nullptr;
    }
    else
    {
        maps.normal = tmanager->texture(indices.normal);
    }

    if (indices.metallic == std::numeric_limits<std::uint32_t>::max())
    {
        LOG_WARN(Material, "No metallic texture");
        maps.metallic = nullptr;
    }
    else
    {
        maps.metallic = tmanager->texture(indices.metallic);
    }

    if (indices.roughness == std::numeric_limits<std::uint32_t>::max())
    {
        LOG_WARN(Material, "No roughness texture");
        maps.roughness = nullptr;
    }
    else
    {
        maps.roughness = tmanager->texture(indices.roughness);
    }

    if (indices.ao == std::numeric_limits<std::uint32_t>::max())
    {
        LOG_WARN(Material, "No AO (ambient occulusion) texture");
        maps.ao = nullptr;
    }
    else
    {
        maps.ao = tmanager->texture(indices.ao);
    }

    if (indices.specular == std::numeric_limits<std::uint32_t>::max())
    {
        LOG_WARN(Material, "No specular texture");
        maps.specular = nullptr;
    }
    else
    {
        maps.specular = tmanager->texture(indices.specular);
    }

    if (indices.height == std::numeric_limits<std::uint32_t>::max())
    {
        LOG_WARN(Material, "No height texture");
        maps.height = nullptr;
    }
    else
    {
        maps.height = tmanager->texture(indices.height);
    }

    return maps;
}

namespace muse
{

MaterialManager::MaterialManager(TextureManager *tmanager)
    : materials_()
    , tmanager_(tmanager)
    , material_counter_(-1)
{
}

Material *MaterialManager::add(const MaterialIndices &indices, std::string_view name)
{
    auto maps = get_maps(indices, tmanager_);

    material_counter_++;

    return materials_.emplace_back(std::make_unique<Material>(maps, name)).get();
}

void MaterialManager::remove(std::uint32_t index)
{
    materials_.erase(std::begin(materials_) + index);
}

Material *MaterialManager::material(std::uint32_t index) const
{
    return materials_.at(index).get();
}

std::int32_t MaterialManager::counter() const
{
    return material_counter_;
}
}