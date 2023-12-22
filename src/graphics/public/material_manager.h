#pragma once

#include <limits>
#include <memory>
#include <string>
#include <vector>

namespace muse
{

class Material;
class TextureManager;
class MaterialMaps;

/**
 *
 *  Structure that stores paths to all material map files.
 *
 */
struct MaterialPaths
{
    std::string albedo = "";
    std::string normal = "";
    std::string metallic = "";
    std::string roughness = "";
    std::string ao = "";
};

/**
 *
 *  Class that helps us to manage all user created materials.
 *
 */
class MaterialManager
{
  public:
    /**
     *
     *  Create a material manager.
     *
     */
    MaterialManager(TextureManager *tmanager);

    /** Default destructor. */
    ~MaterialManager() = default;

    /**
     *
     *  Add new material.
     *
     *  @param maps Pointers to all material's maps.
     *  @param name Name of material.
     *
     *  @return Pointer to newly created material.
     *
     */
    Material *add(const MaterialMaps &maps, std::string_view name);

    /**
     *
     *  Load all maps and add new material.
     *
     *  @param paths Paths for all textures.
     *  @param name Name of material.
     *
     *  @return Pointer to newly created material.
     *
     */
    Material *load(const MaterialPaths &paths, std::string_view name);

    /**
     *
     *  Remove material at given index.
     *
     *  @param index Index of material to remove.
     *
     */
    void remove(std::uint32_t index);

    /**
     *
     *  Get material at given index.
     *
     *  @param index Index of material to get.
     *
     */
    Material *material(std::uint32_t index) const;

    /**
     *
     *  Get material counter.
     *
     *  @return Material counter.
     *
     */
    std::int32_t counter() const;

  private:
    /** Materials. */
    std::vector<std::unique_ptr<Material>> materials_;

    /** Texture manager. */
    TextureManager *tmanager_;

    /** Material counter. */
    std::int32_t material_counter_;
};
}