#pragma once

#include <memory>
#include <string>
#include <vector>

namespace muse
{

class Material;
class TextureManager;

/**
 *
 *  Structure that stores indices of material textures.
 *
 */
struct MaterialIndices
{
    std::uint32_t albedo;
    std::uint32_t normal;
    std::uint32_t metallic;
    std::uint32_t roughness;
    std::uint32_t ao;
    std::uint32_t specular;
    std::uint32_t height;
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
     *  @param indices Indices of all material's maps.
     *  @param name Name of material.
     *
     *  @return Pointer to newly created material.
     *
     */
    Material *add(const MaterialIndices &indices, std::string_view name);

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
     *  Get counter.
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