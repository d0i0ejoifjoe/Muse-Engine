#pragma once

#include "graphics/public/animation/weight.h"
#include "utils/public/matrix4.h"
#include "utils/public/vector2.h"

#include <array>

namespace muse
{

/** Maximum weights for vertex */
inline constexpr auto max_weights = 4u;

/**
 *
 *  Structure that encapsulates all of vertex components.
 *  Position, normal, color, texture coordinates, tangent, bitangent and lastly weights
 *  to determine how much influence has a bone on vertex.
 *
 */
struct Vertex
{
  public:
    /**
     *
     *  Create a vertex without weights.
     *
     *  @param position Position of vertex.
     *  @param normal Normal of vertex.
     *  @param color Color of vertex.
     *  @param tex_coord Texture coordinate of vertex.
     *  @param tangent Tangent of vertex.
     *  @param bitangent Bitangent of vertex.
     *
     */
    Vertex(const Vector3 &position,
           const Vector3 &normal,
           const Vector4 &color,
           const Vector2 &tex_coord,
           const Vector3 &tangent,
           const Vector3 &bitangent)
        : position(position)
        , normal(normal)
        , color(color)
        , tex_coord(tex_coord)
        , tangent(tangent)
        , bitangent(bitangent)
        , bone_ids()
        , weights()
    {
        for (auto i = 0u; i < max_weights; i++)
        {
            bone_ids[i] = -1;
            weights[i] = 0.0f;
        }
    }

    /**
     *
     *  Create a vertex.
     *
     *  @param position Position of vertex.
     *  @param normal Normal of vertex.
     *  @param color Color of vertex.
     *  @param tex_coord Texture coordinate of vertex.
     *  @param tangent Tangent of vertex.
     *  @param bitangent Bitangent of vertex.
     *  @param weight_array Weights of vertex.
     *
     */
    Vertex(const Vector3 &position,
           const Vector3 &normal,
           const Vector4 &color,
           const Vector2 &tex_coord,
           const Vector3 &tangent,
           const Vector3 &bitangent,
           const std::array<Weight, max_weights> &weight_array)
        : position(position)
        , normal(normal)
        , color(color)
        , tex_coord(tex_coord)
        , tangent(tangent)
        , bitangent(bitangent)
        , bone_ids()
        , weights()
    {
        for (auto i = 0u; i < max_weights; i++)
        {
            bone_ids[i] = weight_array[i].index;
            weights[i] = weight_array[i].weight;
        }
    }

    /** Position */
    Vector3 position;

    /** Normal */
    Vector3 normal;

    /** Color */
    Vector4 color;

    /** Texture coordinate */
    Vector2 tex_coord;

    /** Tangent */
    Vector3 tangent;

    /** Bitangent */
    Vector3 bitangent;

    /** Bone IDs */
    std::array<std::int32_t, 4> bone_ids;

    /** Weights */
    Vector4 weights;
};

}