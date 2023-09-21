#pragma once

#include "utils/public/include_glm.h"
#include "weight.h"

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
         *  Create a vertex.
         * 
         *  @param position Position of vertex.
         *  @param normal Normal of vertex.
         *  @param color Color of vertex.
         *  @param tex_coord Texture coordinate of vertex.
         *  @param tangent Tangent of vertex.
         *  @param bitangent Bitangent of vertex.
         *  @param weights Weights of bones for vertex.
         * 
        */
        Vertex(const glm::vec3& position,
               const glm::vec3& normal,
               const glm::vec4& color,
               const glm::vec2& tex_coord,
               const glm::vec3& tangent,
               const glm::vec3& bitangent,
               const std::array<Weight, 4>& weights)
            : position(position)
            , normal(normal)
            , color(color)
            , tex_coord(tex_coord)
            , tangent(tangent)
            , bitangent(bitangent)
            , weights(weights)
        {
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

        /** Weights */
        std::array<Weight, 4> weights;
    };
}