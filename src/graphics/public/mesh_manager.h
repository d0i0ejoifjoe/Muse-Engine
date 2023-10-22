#pragma once

#include "graphics/public/mesh.h"
#include "utils/public/utils.h"

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace muse
{

class TextureManager;
class MaterialManager;
class Material;

/** Useful aliases. */
using AnimationCallback = std::function<void(std::vector<Animation> &&,
                                             Skeleton &&,
                                             std::map<std::string, std::uint32_t, std::less<>> &&)>;

using MaterialCallback = std::function<void(std::vector<Material> &&)>;

/**
 *
 *  This class helps us manage meshes and load the from file.
 *
 */
class MeshManager
{
  public:
    /**
     *
     *  Create mesh manager.
     *
     */
    explicit MeshManager(TextureManager *tmanager, MaterialManager *mmanager);

    /** Default destructor. */
    ~MeshManager() = default;

    /**
     *
     *  Load mesh.
     *
     *  @param filename Path to model file.
     *  @param animation_callback Function that called when loaded the animation/s.
     *  @param flip_uvs Whether flip UVs.
     *
     *  @return Pointer to new loaded mesh.
     *
     */
    MUSE_NODISCARD std::vector<Mesh *> load(const std::string &filename,
                                            const AnimationCallback &animation_callback,
                                            bool flip_uvs);

    /**
     *
     *  Get mesh at given index, if exists.
     *
     *  @param index Index of mesh.
     *
     */
    MUSE_NODISCARD Mesh *mesh(std::uint32_t index);

    /**
     *
     *  Remove mesh at given index.
     *
     *  @param index Index of mesh.
     *
     */
    void remove(std::uint32_t index);

    /**
     *
     *  Create a mesh from supplied variables.
     *
     *  @param vertices Vertices.
     *  @param indices Indices.
     *  @param material Mesh's material.
     *
     *  @return Pointer to newly created mesh.
     *
     */
    MUSE_NODISCARD Mesh *create(const std::vector<Vertex> &vertices,
                                const std::vector<std::uint32_t> &indices,
                                std::uint32_t material_index);

  private:
    /** Meshes. */
    std::vector<std::unique_ptr<Mesh>> meshes_;

    /** Texture manager. */
    TextureManager *tmanager_;

    /** Material manager. */
    MaterialManager *mmanager_;
};

}