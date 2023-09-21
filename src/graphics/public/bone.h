#pragma once

#include <cstdint>
#include "utils/public/include_glm.h"

namespace muse
{
    /**
     * 
     *  Structure that encapsulates bone data like, name, index, offset.
     *  Index is used to find this bone transformation in vertex shader and will be set by skeleton class while
     *  the offset matrix is used to transform vertex from local space to bone space
     * 
    */
    struct Bone
    {
        /**
         * 
         *  Create an empty (invalid) bone.
         * 
        */
        Bone()
            : Bone("", glm::mat4{1.0f})
        {
        }

        /**
         * 
         *  Create a bone.
         * 
         *  @param name Name of bone.
         *  @param offset Offset matrix.
         * 
        */
        Bone(std::string_view name, const glm::mat4& offset)
            : name(name)
            , index(-1)
            , offset(offset)
        {
        }

        /** Name of bone */
        std::string name;

        /** Index of bone transformation */
        std::int32_t index;
    
        /** Offset matrix.*/
        glm::mat4 offset;
    };
}