#pragma once

#include <vector>
#include <cstdint>

#include "vertex.h"
#include "buffer.h"

namespace muse
{
    /**
     * 
     *  Class that represents a mesh.
     * 
    */
    class Mesh
    {
    public:
        /**
         * 
         *  Create a mesh.
         * 
         *  @param vertex_data Initial vertices of mesh.
         *  @param index_data Initial indices of mesh.
         * 
        */
        Mesh(const std::vector<Vertex>& vertex_data,
             const std::vector<std::uint32_t>& index_data);

        /**
         * 
         *  Write vertices to vertex buffer.
         * 
         *  @param vertex_data Data to write.
         * 
        */
        void write_data(const std::vector<Vertex>& vertex_data);

        /**
         * 
         *  Write indices to index buffer.
         * 
         *  @param index_data Data to write.
         * 
        */
        void write_data(const std::vector<std::uint32_t>& index_data);

        /**
         * 
         *  Bind VAO.
         * 
        */
        void bind();

    private:
        /**
         * 
         *  Setup VAO.
         * 
        */
        void setup();

        /** Vertex buffer */
        VBO vbo_;

        /** Index buffer */
        IBO ibo_;

        /** VAO handle */
        GLuint handle_;
    };
}