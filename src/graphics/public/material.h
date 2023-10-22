#pragma once

#include <cstdint>
#include <string>

namespace muse
{

class Texture;

/**
 *
 *  Structure that stores pointer to all needed maps for a material.
 *
 */
struct MaterialMaps
{
    Texture *albedo;
    Texture *normal;
    Texture *metallic;
    Texture *roughness;
    Texture *ao;
    Texture *specular;
    Texture *height; // Ambient occulusion map
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
     *  @param maps All of material maps.
     *  @param name Name of material.
     *
     */
    Material(const MaterialMaps &maps, std::string_view name);

    /**
     *
     *  Get albedo map.
     *
     *  @return Pointer to albedo map.
     *
     */
    Texture *albedo() const;

    /**
     *
     *  Get normal map.
     *
     *  @return Pointer to normal map.
     *
     */
    Texture *normal() const;

    /**
     *
     *  Get roughness map.
     *
     *  @return Pointer to roughness map.
     *
     */
    Texture *roughness() const;

    /**
     *
     *  Get metallic map.
     *
     *  @return Pointer to metallic map.
     *
     */
    Texture *metallic() const;

    /**
     *
     *  Get specular map.
     *
     *  @return Pointer to specular map.
     *
     */
    Texture *specular() const;

    /**
     *
     *  Get height map.
     *
     *  @return Pointer to height map.
     *
     */
    Texture *height() const;

    /**
     *
     *  Get ao (ambient occulusion) map.
     *
     *  @return Pointer to AO map.
     *
     */
    Texture *ao() const;

    /**
     *
     *  Get name of material.
     *
     *  @return Name.
     *
     */
    std::string name() const;

  private:
    /** Indices. */
    MaterialMaps maps_;

    /** Name. */
    std::string name_;
};
}