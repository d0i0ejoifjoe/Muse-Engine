#pragma once

#include "utils/public/matrix4.h"

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
     *  @param parent Name of parent of this bone.
     *
     */
    Bone(std::string_view name,
         const Matrix4 &offset,
         const Matrix4 &transform,
         std::int32_t index,
         std::string_view parent)
        : name(name)
        , offset(offset)
        , transform(transform)
        , index(index)
        , parent(parent)
    {
    }

    /** Name of bone. */
    std::string name;

    /** Offset matrix. */
    Matrix4 offset;

    /** Transform matrix. */
    Matrix4 transform;

    /** Index of bone. */
    std::int32_t index;

    /** Children of this bone. */
    std::string parent;
};
}