#pragma once

#include <string>
#include <functional>
#include <vector>

namespace muse
{
    class Mesh;
    class Animation;

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
         *  @param animation_callback Callback that called when loaded the animations.
         * 
         *  @return Pointer to new loaded mesh.
         *  
        */
        Mesh* load(const std::string& filename,
                   bool flip_uvs,
                   std::function<void(const std::vector<Animation>&)> animation_callback);

        /**
         * 
         *  Get mesh at given index, if exists.
         * 
         *  @param index Index of mesh.
         * 
        */
        Mesh* mesh(std::uint32_t index);

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