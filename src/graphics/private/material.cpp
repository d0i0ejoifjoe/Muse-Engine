#include "graphics/public/material.h"

namespace muse
{

Material::Material(const MaterialIndices &indices)
    : indices_(indices)
{
}

std::uint32_t Material::albedo_index() const
{
    return indices_.albedo;
}

}