#pragma once

#include "transform.h"

#include <string>

namespace muse
{
    /**
     * 
     *  Class that represents a bone.
     * 
    */
    class Bone
    {
    public:
        /**
         * 
         *  Create a root bone.  
         * 
         *  @param name Name of bone.
         *  @param offset Offset matrix.
         *  @param transform Bone transformation.
         * 
        */
        Bone(std::string_view name, const glm::mat4& offset, const Transform& transform);

        /**
         * 
         *  Create bone.
         * 
         *  @param parent Parent bone.
         *  @param name Name of this bone.
         *  @param offset Offset matrix.
         *  @param transform Bone transformation.
         * 
        */
        Bone(Bone* parent, std::string_view name, const glm::mat4& offset, const Transform& transform);

        /**
         * 
         *  Get name of bone.
         * 
         *  @return Bone name.
         * 
        */
        std::string name() const;

        /**
         * 
         *  Get bone parent.
         * 
         *  @return Pointer to bone's parent.
         * 
         *  @note If pointer in nullptr means that's root bone.
         * 
        */
        const Bone* parent() const;

        /**
         * 
         *  Get offset matrix.
         * 
         *  @return Offset matrix.
         * 
        */
        glm::mat4 offset() const;

        /**
         * 
         *  Get transform of bone.
         * 
         *  @return Transform.
         * 
        */
        Transform transform() const;

        /**
         * 
         *  Set transform of bone.
         * 
         *  @param transform New transform.
         * 
        */
        void set_transform(const Transform& transform);
        
    private:
        /** Parent. */
        Bone* parent_;

        /** Name of bone. */
        std::string name_;

        /** Offset. */
        glm::mat4 offset_;

        /** Transformation of a bone. */
        Transform transform_;
    };
}