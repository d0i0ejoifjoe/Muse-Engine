#pragma once

#include "buffer.h"
#include "skeleton.h"
#include "transform.h"
#include "vertex.h"

#include <cstdint>
#include <vector>

namespace muse
{

/**
 *
 *  Class that represents a mesh.
 *  A Mesh it's just a set of vertices and indices but in this case
 *  it's has it's own model matrix which is called "transform"
 *  it's has a boolean that helps us determine whether
 *  mesh should receive shadow from light
 *  and lastly it's own skeleton that we use in render function
 *  to submit bone transformations.
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
     *  @param skeleton Skeleton of mesh.
     *
     */
    Mesh(const std::vector<Vertex> &vertex_data,
         const std::vector<std::uint32_t> &index_data,
         const Skeleton &skeleton);

    /**
     *
     *  Write vertices to vertex buffer.
     *
     *  @param vertex_data Data to write.
     *
     */
    void write_data(const std::vector<Vertex> &vertex_data);

    /**
     *
     *  Write indics to element buffer.
     *
     *  @param index_data Data to write.
     *
     */
    void write_data(const std::vector<std::uint32_t> &index_data);

    /**
     *
     *  Bind VAO.
     *
     */
    void bind();

    /**
     *
     *  Get whether mesh receives shadows.
     *
     *  @return True if it is otherwise false.
     *
     */
    bool receive_shadows() const;

    /**
     *
     *  Get mesh transformation.
     *
     *  @return Transform.
     *
     */
    Transform transform() const;

    /**
     *
     *  Set whether mesh will cast shadows.
     *
     *  @param receive_shadows Cast shadows.
     *
     */
    void set_receive_shadows(bool receive_shadows);

    /**
     *
     *  Set mesh transform.
     *
     *  @param transform Transform.
     *
     */
    void set_transform(const Transform &transform);

    /**
     *
     *  Set translation of mesh.
     *
     *  @param translation Translation.
     *
     *
     */
    void set_translation(const glm::vec3 &translation);

    /**
     *
     *  Set rotation of mesh.
     *
     *  @param rotation Rotation.
     *
     */
    void set_rotation(const glm::quat &rotation);

    /**
     *
     *  Set scale of mesh.
     *
     *  @param scale Scale.
     *
     */
    void set_scale(const glm::vec3 &scale);

    /**
     *
     *  Get element count.
     *
     *  @return Count.
     *
     */
    std::size_t element_count() const;

    /**
     *
     *  Get skeleton of this mesh.
     *
     *  @return Skeleton pointer.
     *
     */
    const Skeleton *skeleton() const;

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

    /** Whether the mesh will receive shadows from light */
    bool receive_shadows_;

    /** Mesh transform */
    Transform transform_;

    /** Element count */
    std::size_t element_count_;

    /** Skeleton. */
    Skeleton skeleton_;
};

}