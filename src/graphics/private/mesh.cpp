#include "graphics/public/mesh.h"
#include "graphics/private/vertex_descriptor.h"

namespace muse
{
    Mesh::Mesh(const std::vector<Vertex>& vertex_data,
               const std::vector<std::uint32_t>& index_data)
        : vbo_(sizeof(Vertex) * vertex_data.size())
        , ibo_(sizeof(std::uint32_t) * index_data.size())
        , handle_(0)
    {
        glGenVertexArrays(1, &handle_);

        write_data(vertex_data);
        write_data(index_data);
    }

    void Mesh::write_data(const std::vector<Vertex>& vertex_data)
    {
        vbo_.write(vertex_data);
        setup();
    }
    
    void Mesh::write_data(const std::vector<std::uint32_t>& index_data)
    {
        ibo_.write(index_data);
        element_count_ += index_data.size();
        
        setup();
    }

    void Mesh::bind()
    {
        glBindVertexArray(handle_);
    }

    void Mesh::setup()
    {
        glBindVertexArray(handle_);

        glBindBuffer(GL_ARRAY_BUFFER, vbo_.handle());
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_.handle());

        auto index = 0u;
        auto size = DefaultVertexDescriptor.size();

        for(const auto&[_, __, count, offset] : DefaultVertexDescriptor)
        {
            glEnableVertexAttribArray(index);
            glVertexAttribPointer(index, static_cast<GLint>(count), GL_FLOAT, GL_FALSE, static_cast<GLsizei>(size), reinterpret_cast<void*>(offset));
            index++;
        }

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        glBindVertexArray(0);
    }

    void Mesh::set_receive_shadows(bool receive_shadows)
    {
        receive_shadows_ = receive_shadows;
    }

    bool Mesh::receive_shadows() const
    {
        return receive_shadows_;
    }

    Transform Mesh::transform() const
    {
        return transform_;
    }

    void Mesh::set_transform(const Transform& transform)
    {
        transform_ = transform;
    }

    void Mesh::set_translation(const glm::vec3& translation)
    {
        transform_.set_translation(translation);
    }

    void Mesh::set_rotation(const glm::quat& rotation)
    {
        transform_.set_rotation(rotation);
    }

    void Mesh::set_scale(const glm::vec3& scale)
    {
        transform_.set_scale(scale);
    }

    std::size_t Mesh::element_count() const
    {
        return element_count_;
    }
}