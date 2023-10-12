#pragma once

#include <cstdint>

namespace muse
{

struct MaterialIndices
{
    std::uint32_t albedo;
    std::uint32_t normal;
    std::uint32_t roughness;
    std::uint32_t metalness;
    std::uint32_t specular;
    std::uint32_t height;
    std::uint32_t ao; // Ambient occulusion map
};

/**
 *
 *  Class that stores all indices to all different kinds of maps.
 *  Then these indices get submitted to shader to find the maps
 *  and do all of the calculations needed.
 *
 */
class Material
{
  public:
    /**
     *
     *  Create a material.
     *
     *  @param indices All of map indices.
     *
     */
    Material(const MaterialIndices &indices);

    /**
     *
     *  Get albedo map index.
     *
     *  @return Albedo map index.
     *
     */
    std::uint32_t albedo_index() const;

    /**
     *
     *  Get normal map index.
     *
     *  @return Normal map index.
     *
     */
    std::uint32_t normal_index() const;

    /**
     *
     *  Get roughness map index.
     *
     *  @return roughness map index.
     *
     */
    std::uint32_t roughness_index() const;

    /**
     *
     *  Get metalness map index.
     *
     *  @return Metalness map index.
     *
     */
    std::uint32_t metalness_index() const;

    /**
     *
     *  Get specular map index.
     *
     *  @return Specular map index.
     *
     */
    std::uint32_t specular_index() const;

    /**
     *
     *  Get height map index.
     *
     *  @return Height map index.
     *
     */
    std::uint32_t height_index() const;

    /**
     *
     *  Get ao (ambient occulusion) map index.
     *
     *  @return AO map index.
     *
     */
    std::uint32_t ao_index() const;

  private:
    /** Indices. */
    MaterialIndices indices_;
};
}