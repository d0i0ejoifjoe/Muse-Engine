#include "graphics/public/shader_system.h"

#include "log/public/logger.h"

#include <cassert>
#include <cstdlib>

/** Maximum buffer size. */
static constexpr auto max_buf_size = 32000u;

/**
 *
 *  Check compile status of shader.
 *
 *  @param handle OpenGL handle to shader.
 *
 */
void check_compile_status(GLuint handle)
{
    if (handle == 0)
    {
        return;
    }

    GLint status = GL_NONE;
    glGetShaderiv(handle, GL_COMPILE_STATUS, &status);

    if (!status)
    {
        char buf[max_buf_size];
        glGetShaderInfoLog(handle, max_buf_size, nullptr, buf);

        GLint shader_type = GL_NONE;
        glGetShaderiv(handle, GL_SHADER_TYPE, &shader_type);

        std::string shader_str{""};
        if (shader_type == GL_VERTEX_SHADER)
        {
            shader_str = "vertex shader";
        }
        else if (shader_type == GL_FRAGMENT_SHADER)
        {
            shader_str = "fragment shader";
        }
        else if (shader_type == GL_GEOMETRY_SHADER)
        {
            shader_str = "geometry shader";
        }

        LOG_ERROR(OpenGLShaderSystem, "\nCompilation failure of {}\nOpenGL Error log: {}", shader_str, buf);
        std::abort();
    }
}

/**
 *
 *  Check link status of opengl program.
 *
 *  @param handle Handle to program.
 *
 */
void check_link_status(GLuint handle)
{
    GLint status = GL_NONE;
    glGetProgramiv(handle, GL_LINK_STATUS, &status);

    if (!status)
    {
        char buf[max_buf_size];
        glGetProgramInfoLog(handle, max_buf_size, nullptr, buf);

        LOG_ERROR(OpenGLShaderSystem, "\nLinkage failure\nOpenGL Error log: {}", buf);
        std::abort();
    }
}

/**
 *
 *  Check validation status of opengl program.
 *
 *  @param handle Handle to program.
 *
 */
void check_validation_status(GLuint handle)
{
    GLint status = GL_NONE;
    glGetProgramiv(handle, GL_VALIDATE_STATUS, &status);

    if (!status)
    {
        char buf[max_buf_size];
        glGetProgramInfoLog(handle, max_buf_size, nullptr, buf);

        LOG_ERROR(OpenGLShaderSystem, "\nOpenGL Program is invalid\nOpenGL Error log: {}", buf);
        std::abort();
    }
}

namespace muse
{

ShaderSystem::ShaderSystem(std::string_view vertex_shader_source, std::string_view fragment_shader_source,
                           std::optional<std::string_view> geometry_shader_source)
    : handle_(0)
    , uniform_map_()
{
    auto vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    auto fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    auto geometry_shader = GL_NONE;

    const char *c_vertex_source = vertex_shader_source.data();
    const char *c_fragment_source = fragment_shader_source.data();

    glShaderSource(vertex_shader, 1, &c_vertex_source, nullptr);
    glShaderSource(fragment_shader, 1, &c_fragment_source, nullptr);

    glCompileShader(vertex_shader);
    glCompileShader(fragment_shader);

    if (geometry_shader_source)
    {
        auto temp = glCreateShader(GL_GEOMETRY_SHADER);

        const char *c_geometry_source = geometry_shader_source->data();

        glShaderSource(temp, 1, &c_geometry_source, nullptr);
        glCompileShader(temp);

        geometry_shader = temp;
    }

    check_compile_status(vertex_shader);
    check_compile_status(fragment_shader);
    check_compile_status(geometry_shader);

    handle_ = glCreateProgram();

    glAttachShader(handle_, vertex_shader);
    glAttachShader(handle_, fragment_shader);

    if (geometry_shader != GL_NONE)
    {
        glAttachShader(handle_, geometry_shader);
    }

    glLinkProgram(handle_);
    check_link_status(handle_);

    glValidateProgram(handle_);
    check_validation_status(handle_);
}

ShaderSystem::~ShaderSystem()
{
    glDeleteProgram(handle_);
}

void ShaderSystem::bind()
{
    glUseProgram(handle_);
}

GLuint ShaderSystem::handle() const
{
    return handle_;
}

void ShaderSystem::add_uniform(const std::string &uniform_name)
{
    uniform_map_[uniform_name] = glGetUniformLocation(handle_, uniform_name.data());
}

void ShaderSystem::remove_uniform(const std::string &uniform_name)
{
    uniform_map_.erase(uniform_name);
}

void ShaderSystem::set_value(const std::string &uniform_name, std::uint32_t value)
{
    glProgramUniform1ui(handle_, uniform_map_[uniform_name], value);
}

void ShaderSystem::set_value(const std::string &uniform_name, const glm::mat4 &value)
{
    glProgramUniformMatrix4fv(handle_, uniform_map_[uniform_name], 1, GL_FALSE, &value[0][0]);
}

void ShaderSystem::set_value(const std::string &uniform_name, std::uint64_t value)
{
    glProgramUniformHandleui64ARB(handle_, uniform_map_[uniform_name], value);
}
}