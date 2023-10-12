#include "graphics/public/mesh.h"

#include "graphics/private/vertex_descriptor.h"
#include "graphics/public/skeleton.h"
#include "log/public/logger.h"

std::pair<GLenum, GLboolean> to_opengl(muse::VertexElementType type)
{
    GLenum gl_type = GL_NONE;
    GLboolean normalize = GL_FALSE;
    switch (type)
    {
        case muse::VertexElementType::FLOAT2: // for all default vectors the type is same
        case muse::VertexElementType::FLOAT3:
        case muse::VertexElementType::FLOAT4: gl_type = GL_FLOAT; break;
        case muse::VertexElementType::UINT4:
            gl_type = GL_UNSIGNED_INT;
            normalize = GL_TRUE;
            break;
    }

    return {gl_type, normalize};
}

namespace muse
{

Mesh::Mesh(const std::vector<Vertex> &vertex_data, const std::vector<std::uint32_t> &index_data,
           const Skeleton &skeleton)
    : vbo_(sizeof(Vertex) * vertex_data.size())
    , ibo_(sizeof(std::uint32_t) * index_data.size())
    , handle_(0)
    , receive_shadows_(true)
    , transform_(Transform{})
    , element_count_(index_data.size())
    , skeleton_(skeleton)
{
    glGenVertexArrays(1, &handle_);

    glBindVertexArray(handle_);

    vbo_.write(vertex_data);
    ibo_.write(index_data);

    setup();
}

Mesh::~Mesh()
{
    glDeleteVertexArrays(1, &handle_);
}

void Mesh::bind()
{
    glBindVertexArray(handle_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_.handle());
}

void Mesh::setup()
{
    glBindVertexArray(handle_);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_.handle());
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_.handle());

    auto index = 0u;
    auto size = DefaultVertexDescriptor.size();

    for (const auto &[type, __, count, offset] : DefaultVertexDescriptor)
    {
        auto [gl_type, normalize] = to_opengl(type);
        glEnableVertexAttribArray(index);
        glVertexAttribPointer(index, static_cast<GLint>(count), gl_type, normalize, static_cast<GLsizei>(size),
                              reinterpret_cast<void *>(offset));

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

void Mesh::set_transform(const Transform &transform)
{
    transform_ = transform;
}

void Mesh::set_translation(const glm::vec3 &translation)
{
    transform_.set_translation(translation);
}

void Mesh::set_rotation(const glm::quat &rotation)
{
    transform_.set_rotation(rotation);
}

void Mesh::set_scale(const glm::vec3 &scale)
{
    transform_.set_scale(scale);
}

std::size_t Mesh::element_count() const
{
    return element_count_;
}

const Skeleton *Mesh::skeleton() const
{
    return std::addressof(skeleton_);
}

}