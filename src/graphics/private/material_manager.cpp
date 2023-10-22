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
}

Material *MaterialManager::add(const MaterialIndices &indices, std::string_view name)
{
    MaterialMaps maps{.albedo = tmanager_->texture(indices.albedo),
                      .normal = tmanager_->texture(indices.normal),
                      .metallic = tmanager_->texture(indices.metallic),
                      .roughness = tmanager_->texture(indices.roughness),
                      .ao = tmanager_->texture(indices.ao),
                      .specular = tmanager_->texture(indices.specular),
                      .height = tmanager_->texture(indices.height)};

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