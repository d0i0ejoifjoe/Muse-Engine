#include "graphics/public/material_manager.h"

#include "graphics/public/material.h"
#include "graphics/public/texture_manager.h"

namespace muse
{

MaterialManager::MaterialManager(TextureManager *tmanager)
    : materials_()
    , tmanager_(tmanager)
    , material_counter_(-1)
{
    LOG_INFO(MaterialManager, "Material manager created!");
}

Material *MaterialManager::add(const MaterialMaps &maps, std::string_view name)
{
    if (!maps.albedo)
    {
        LOG_WARN(Material, "No albedo map");
    }

    if (!maps.normal)
    {
        LOG_WARN(Material, "No normal map");
    }

    if (!maps.ao)
    {
        LOG_WARN(Material, "No ambient occulusion map");
    }

    if (!maps.roughness)
    {
        LOG_WARN(Material, "No roughness map");
    }

    if (!maps.metallic)
    {
        LOG_WARN(Material, "No metallic map");
    }

    material_counter_++;

    return materials_.emplace_back(std::make_unique<Material>(maps, name)).get();
}

Material *MaterialManager::load(const MaterialPaths &paths, std::string_view name)
{
    MaterialMaps maps{};
    maps.albedo = tmanager_->load_texture(paths.albedo, nullptr, true, false);
    maps.ao = tmanager_->load_texture(paths.ao, nullptr, false, false);
    maps.metallic = tmanager_->load_texture(paths.metallic, nullptr, false, false);
    maps.normal = tmanager_->load_texture(paths.normal, nullptr, false, false);
    maps.roughness = tmanager_->load_texture(paths.roughness, nullptr, false, false);

    return this->add(maps, name);
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