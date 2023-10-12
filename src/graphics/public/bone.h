#pragma once

#include "utils/public/include_glm.h"

#include <cstdint>
#include <string>
#include <vector>

namespace muse
{

/**
 *
 *  Structure that encapsulates information about
 *  that bone like it's offset matrix, transform matrix, name, index, etc.
 *
 */
struct Bone
{
    /**
     *
     *  Create an empty bone.
     *
     */
    Bone() = default;

    /**
     *
     *  Create a bone.
     *
     *  @param name Name of bone.
     *  @param offset Offset matrix.
     *  @param transform Transform matrix.
     *  @param index Index of bone.
     *  @param children All of it's children.
     *
     */
    Bone(std::string_view name,
         const glm::mat4 &offset,
         const glm::mat4 &transform,
         std::int32_t index,
         const std::vector<Bone> &children)
        : name(name)
        , offset(offset)
        , transform(transform)
        , index(index)
        , children(children)
    {
    }

    /** Name of bone. */
    std::string name;

    /** Offset matrix. */
    glm::mat4 offset;

    /** Transform matrix. */
    glm::mat4 transform;

    /** Index of bone. */
    std::int32_t index;

    /** Children of this bone. */
    std::vector<Bone> children;
};

}