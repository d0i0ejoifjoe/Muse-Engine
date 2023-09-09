#pragma once

#include "utils/public/include_glm.h"

namespace muse
{
    /**
     * 
     *  Structure that encapsulates all of vertex components.
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
         *  @param tex_coord Texture coordinate of vertex.
         *  @param tangent Tangent of vertex.
         *  @param bitangent Bitangent of vertex.
         * 
        */
        Vertex(const glm::vec3& position,
               const glm::vec3& normal,
               const glm::vec2& tex_coord,
               const glm::vec3& tangent,
               const glm::vec3& bitangent)
            : position(position)
            , normal(normal)
            , tex_coord(tex_coord)
            , tangent(tangent)
            , bitangent(bitangent)
        {
        }

        /** Position */
        glm::vec3 position;

        /** Normal */
        glm::vec3 normal;

        /** Texture coordinate */
        glm::vec2 tex_coord;

        /** Tangent */
        glm::vec3 tangent;

        /** Bitangent */
        glm::vec3 bitangent;
    };
}