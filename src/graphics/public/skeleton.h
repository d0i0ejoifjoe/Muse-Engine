#pragma once

#include "bone.h"

namespace muse
{
    class Animation;

    /**
     * 
     *  Class that helps us update bones transformations
     *  and query for them.
     * 
    */
    class Skeleton
    {
    public:
        /**
         * 
         *  Create a skeleton.
         * 
         *  @param root_bone Root bone that contains all other bones as their children.
         *  @param animation Animation that's responsible for the skeleton.
         * 
        */
        Skeleton(const Bone& root_bone,
                 Animation* animation);

        /**
         * 
         *  Get root bone.
         * 
         *  @return Root bone pointer.
         * 
        */
        const Bone* root_bone() const;

        /**
         * 
         *  Get transforms.
         * 
         *  @return Transforms.
         * 
        */
        const std::vector<glm::mat4>& transforms() const;

        /**
         * 
         *  Get animation.
         *  
         *  @return Pointer to animation.
         * 
        */
        Animation* animation() const;

        /**
         * 
         *  Update all bone transformations.
         * 
        */
        void update();

        /**
         * 
         *  Find bone by name.
         * 
         *  @param name Name of bone.
         * 
         *  @return Nullptr if bone doesn't exist, pointer to bone otherwise.
         * 
        */
        Bone* find_bone(std::string_view name);

        /**
         * 
         *  Set skeleton owning animation.
         * 
         *  @param animation Pointer to animation.
         * 
        */
        void set_animation(Animation* animation);

    private:
        /** Root bone. */
        Bone root_bone_;

        /** Transformations. */
        std::vector<glm::mat4> transforms_;

        /** Animation that's responsible for skeleton. */
        Animation* animation_;
    };
}