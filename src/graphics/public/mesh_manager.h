#pragma once

#include "graphics/public/mesh.h"
#include "utils/public/utils.h"

#include <string>
#include <functional>
#include <memory>
#include <vector>

namespace muse
{
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
        MeshManager() = default;

        /** Default destructor. */
        ~MeshManager() = default;
    
        /**
         * 
         *  Load mesh.
         * 
         *  @param filename Path to model file.
         *  @param flip_uvs Whether flip UVs.
         * 
         *  @return Pointer to new loaded mesh.
         *  
        */
        MUSE_NODISCARD Mesh* load(const std::string& filename,
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
        MUSE_NODISCARD Mesh* load(const std::string& animation_filename,
                                  const std::string& mesh_filename,
                                  bool flip_uvs,
                                  std::function<void(const std::vector<Animation>&)> animation_callback);

        /**
         * 
         *  Get mesh at given index, if exists.
         * 
         *  @param index Index of mesh.
         * 
        */
        MUSE_NODISCARD Mesh* mesh(std::uint32_t index);

        /**
         * 
         *  Remove mesh at given index.
         * 
         *  @param index Index of mesh.
         * 
        */
        void remove(std::uint32_t index);

    private:
        /** Meshes. */
        std::vector<std::unique_ptr<Mesh>> meshes_;
    };
}