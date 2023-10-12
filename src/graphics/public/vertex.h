#pragma once

#include "utils/public/include_glm.h"
#include "weight.h"

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
    Vertex(const glm::vec3 &position,
           const glm::vec3 &normal,
           const glm::vec4 &color,
           const glm::vec2 &tex_coord,
           const glm::vec3 &tangent,
           const glm::vec3 &bitangent)
        : position(position)
        , normal(normal)
        , color(color)
        , tex_coord(tex_coord)
        , tangent(tangent)
        , bitangent(bitangent)
        , bone_ids(0)
        , weights(0.0f)
    {
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
    Vertex(const glm::vec3 &position,
           const glm::vec3 &normal,
           const glm::vec4 &color,
           const glm::vec2 &tex_coord,
           const glm::vec3 &tangent,
           const glm::vec3 &bitangent,
           const std::array<Weight, max_weights> &weight_array)
        : position(position)
        , normal(normal)
        , color(color)
        , tex_coord(tex_coord)
        , tangent(tangent)
        , bitangent(bitangent)
        , bone_ids(0)
        , weights(0.0f)
    {
        for (auto i = 0u; i < max_weights; i++)
        {
            bone_ids[i] = weight_array[i].index;
            weights[i] = weight_array[i].weight;
        }
    }

    /** Position */
    glm::vec3 position;

    /** Normal */
    glm::vec3 normal;

    /** Color */
    glm::vec4 color;

    /** Texture coordinate */
    glm::vec2 tex_coord;

    /** Tangent */
    glm::vec3 tangent;

    /** Bitangent */
    glm::vec3 bitangent;

    /** Bone IDs */
    glm::ivec4 bone_ids;

    /** Weights */
    glm::vec4 weights;
};

}