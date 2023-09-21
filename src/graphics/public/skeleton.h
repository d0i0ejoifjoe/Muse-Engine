#pragma once

#include "bone.h"
#include "keyframe.h"

#include <chrono>
#include <unordered_map>
#include <vector>

struct AssimpNode;

namespace muse
{
    /**
     * 
     *  Class that represents a skeleton.
     *  Skeleton is responsible for updating bones at given time,
     *  storing transforms that need to be submit to vertex shader and
     *  setting up indices.
     * 
    */
    class Skeleton
    {
    public:
        /**
         * 
         *  Create skeleton.
         * 
         *  @param bones Bones of skeleton.
         *  @param root Root node of node hierarchy.
         *  @param frames Map with key as bone name and value as series of keyframes that sorted by time.
         * 
        */ 
        explicit Skeleton(const std::vector<Bone>& bones,
                          const AssimpNode& root,
                          const std::unordered_map<std::string, std::vector<Keyframe>>& frames);

        /**
         * 
         *  Update all of bone transformations.
         * 
         *  @param time Current time in animation.
         * 
        */
        void update(std::chrono::milliseconds time);

        /**
         * 
         *  Get bone transforms.
         * 
         *  @return Const reference to bone transforms.
         * 
        */
        const std::vector<glm::mat4>& transforms() const;

        /**
         * 
         *  Get bones.
         * 
         *  @return Const reference to bones array.
         * 
        */
        const std::vector<Bone>& bones() const;

        /**
         * 
         *  Find bone.
         *  
         *  @param name Name of bone.
         * 
        */
        Bone* find_bone(std::string_view name);

        /**
         * 
         *  Get transform from frames map at given time.
         * 
         *  @param name Name of bone to find calculate transform of.
         *  @param time Current time in animation (provided by update() in which time is provided by animation).
         * 
        */
        Transform transform(const std::string& name, std::chrono::milliseconds time);

    private:
        /** Bones. */
        std::vector<Bone> bones_;

        /** Root node. */
        const ::AssimpNode* root_;

        /** Transforms. */
        std::vector<glm::mat4> transforms_;
        
        /** Frame map. */
        std::unordered_map<std::string, std::vector<Keyframe>> frames_;
    };
}