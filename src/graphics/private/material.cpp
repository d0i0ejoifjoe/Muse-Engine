#include "graphics/public/material.h"

#include "log/public/logger.h"

namespace muse
{

Material::Material(const MaterialMaps &maps, std::string_view name)
    : maps_(maps)
    , name_(name)
{
}

Texture *Material::albedo() const
{
    return maps_.albedo;
}

Texture *Material::normal() const
{
    return maps_.normal;
}

Texture *Material::roughness() const
{
    return maps_.roughness;
}

Texture *Material::metallic() const
{
    return maps_.metallic;
}

Texture *Material::ao() const
{
    return maps_.ao;
}

std::string Material::name() const
{
    return name_;
}

}