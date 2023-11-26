#pragma once

#include "glad.h"
#include "utils/public/matrix4.h"

#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace muse
{

/**
 *
 *  This class represents shader system.
 *  Shader system is just a shader program encapsulated in easy to control class.
 *
 */
class ShaderSystem
{
  public:
    /**
     *
     *  Create shader system.
     *
     *  @param vertex_shader_source Vertex shader source.
     *  @param fragment_shader_source Fragment shader source.
     *  @param geometry_shader_source Getometry shader source (optional).
     *
     */
    ShaderSystem(std::string_view vertex_shader_source,
                 std::string_view fragment_shader_source,
                 std::optional<std::string_view> geometry_shader_source);

    /**
     *
     *  Destroy shader system.
     *
     */
    ~ShaderSystem();

    /**
     *
     *  Bind this system.
     *
     */
    void bind();

    /**
     *
     *  Get opengl handle to this system.
     *
     *  @return Handle.
     *
     */
    GLuint handle() const;

    /**
     *
     *  Add new uniform.
     *
     *  @param uniform_name Name of uniform.
     *
     */
    void add_uniform(const std::string &uniform_name);

    /**
     *
     *  Remove a uniform.
     *
     *  @param uniform_name Name of uniform.
     *
     */
    void remove_uniform(const std::string &uniform_name);

    /**
     *
     *  Set uniform's value (32bit unsigned integer).
     *
     *  @param uniform_name Name of uniform to set.
     *  @param value New value for uniform.
     *
     */
    void set_value(const std::string &uniform_name, std::uint32_t value);

    /**
     *
     *  Set uniform's value (64bit unsigned integer).
     *
     *  @param uniform_name Name of uniform to set.
     *  @param value New value for uniform.
     *
     */
    void set_value(const std::string &uniform_name, std::uint64_t value);

    /**
     *
     *  Set uniform's value (matrix4x4).
     *
     *  @param uniform_name Name of uniform to set.
     *  @param value New value for uniform.
     *
     */
    void set_value(const std::string &uniform_name, const Matrix4 &value);

    /**
     *
     *  Set uniform's value (array of matrix4x4).
     *
     *  @param uniform_name Name of uniform to set.
     *  @param value New value for uniform.
     *
     */
    void set_value(const std::string &uniform_name, const std::vector<Matrix4> &value);

  private:
    /** Handle. */
    GLuint handle_;

    /** Uniform map. */
    std::unordered_map<std::string, GLuint> uniform_map_;
};

}