#pragma once

#include "graphics/public/mesh.h"
#include "utils/public/utils.h"

#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace muse
{

class TextureManager;
class Material;

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
    MeshManager(TextureManager *tmanager);

    /** Default destructor. */
    ~MeshManager() = default;

    /**
     *
     *  Load mesh.
     *
     *  @param filename Path to model file.
     *  @param material_callback Called when mesh's material is loaded.
     *  @param flip_uvs Whether flip UVs.
     *
     *  @return Pointer to new loaded mesh.
     *
     */
    MUSE_NODISCARD std::vector<Mesh *> load(const std::string &filename,
                                            const std::function<void(const std::vector<Material> &)> &material_callback,
                                            bool flip_uvs);

    /**
     *
     *  Load mesh file and file that has all of animations.
     *
     *  @param animation_filename Animation filename.
     *  @param mesh_filename Mesh filename.
     *  @param flip_uvs Whether to flip UVs.
     *  @param animation_callback Function that called when loaded the animations.
     *
     *  @return Pointer to newly loaded mesh.
     *
     */
    MUSE_NODISCARD std::vector<Mesh *> load(const std::string &animation_filename,
                                            const std::string &mesh_filename,
                                            const std::function<void(const std::vector<Material> &)> &material_callback,
                                            bool flip_uvs,
                                            const std::function<void(const std::vector<Animation> &)> &animation_callback);

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
     *  @param skeleton Skeleton.
     *
     */
    MUSE_NODISCARD Mesh *create(const std::vector<Vertex> &vertices,
                                const std::vector<std::uint32_t> &indices,
                                const Skeleton &skeleton);

  private:
    /** Meshes. */
    std::vector<std::unique_ptr<Mesh>> meshes_;

    /** Texture manager. */
    TextureManager *tmanager_;
};

}