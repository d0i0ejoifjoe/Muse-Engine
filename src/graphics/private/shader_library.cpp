#include "graphics/public/shader_library.h"

#include "graphics/public/shader_system.h"

#include <fstream>

std::string load_file(const std::string &filename)
{
    std::fstream file{filename, std::ios::in};

    std::string string{""};
    std::string line{""};

    if (file.is_open() && file)
    {
        while (std::getline(file, line))
        {
            string += line + '\n'; // Adding that new line because when we're going to read the string we're going read it until the \n symbol with std::getline.
        }
    }

    return string;
}

std::string process_directives(const std::string &shader, const muse::ShaderLibrary *lib)
{
    std::filesystem::path root = lib->root_directory();
    std::string copy{shader};
    std::size_t pos = copy.find("#include");
    if (pos == std::string::npos) // No #include directives
    {
        return shader;
    }

    do
    {
        auto line_length = copy.find_first_of("\n", pos) - pos;
        auto line = copy.substr(pos, line_length);

        // the length of #include " is 10 characters so and length of filename is the length of line
        // minus the #include " length which is 10 character and minus the one quote at the end #include "<name>" <- this one
        // so in total 11
        auto file_name = line.substr(10, line_length - 11);
        auto file = load_file(root / file_name);

        copy.replace(pos, line_length, file, 0); // just replace the directive with file contents

        if (file.empty())
        {
            LOG_ERROR(ShaderLibrary, R"(Failed to load file {})", root / file_name);
            std::abort();

            return shader;
        }

        pos = copy.find("#include");
    } while (pos != std::string::npos);

    return copy;
}

enum class ShaderType : std::uint8_t
{
    INVALID = 0,
    VERTEX,
    FRAGMENT,
    GEOMETRY
};

ShaderType check_type(const std::string &line)
{
    if (line.find("#shader") != std::string::npos) // if there even a #shader directive
    {
        if (line.find(R"("vertex")") != std::string::npos) // directive specifies that from this point until the next directive of #shader
                                                           // we're gonna treat the contents as given type of shader in this case vertex
        {
            return ShaderType::VERTEX;
        }
        else if (line.find(R"("fragment")") != std::string::npos)
        {
            return ShaderType::FRAGMENT;
        }
        else if (line.find(R"("geometry")") != std::string::npos)
        {
            return ShaderType::GEOMETRY;
        }
        else // string is invalid or there's nothing there we're gonna return INVALID enum
        {
            return ShaderType::INVALID;
        }
    }

    return ShaderType::INVALID;
}

namespace muse
{

ShaderLibrary::ShaderLibrary()
    : shaders_()
    , shader_systems_()
    , root_(".")
{
}

void ShaderLibrary::set_root_directory(std::filesystem::path root)
{
    root_ = root;
}

std::filesystem::path ShaderLibrary::root_directory() const
{
    return root_;
}

const std::string &ShaderLibrary::load_shader(std::string_view filename, const std::string &name)
{
    auto shader = load_file(root_ / filename);
    return shaders_[name] = shader;
}

const std::string &ShaderLibrary::shader(const std::string &name)
{
    if (!shaders_.contains(name))
    {
        const static std::string invalid{""};
        return invalid;
    }

    return shaders_[name];
}

const ShaderSystem *ShaderLibrary::shader_system(const std::string &name)
{
    if (!shader_systems_.contains(name))
    {
        return nullptr;
    }

    return shader_systems_[name].get();
}

const std::unordered_map<std::string, std::string> &ShaderLibrary::shaders() const
{
    return shaders_;
}

const std::unordered_map<std::string, std::unique_ptr<ShaderSystem>> &ShaderLibrary::shader_systems() const
{
    return shader_systems_;
}

ShaderSystem *ShaderLibrary::compile_shader(const std::string &shader_name, const std::string &system_name)
{
    std::string shader;
    if (!shaders_.contains(shader_name))
    {
        LOG_INFO(ShaderLibrary, "Shader isn`t found in library!\nName: {}", shader_name);
        return nullptr;
    }

    std::istringstream stream(shaders_[shader_name]);
    std::string line;
    ShaderType current_type;

    std::stringstream vertex_shader;
    std::stringstream fragment_shader;
    std::stringstream geometry_shader;

    std::stringstream *current_shader = std::addressof(vertex_shader);

    while (std::getline(stream, line))
    {
        auto checked = false;
        if (auto type = check_type(line); type != ShaderType::INVALID)
        {
            checked = true;
            current_type = type;
            switch (current_type)
            {
                case ShaderType::VERTEX: current_shader = std::addressof(vertex_shader); break;
                case ShaderType::FRAGMENT: current_shader = std::addressof(fragment_shader); break;
                case ShaderType::GEOMETRY: current_shader = std::addressof(geometry_shader); break;
                case ShaderType::INVALID: break;
                default: break;
            }

            *current_shader << "#version 450 core" << '\n'
                            << "#extension GL_ARB_bindless_texture : require" << '\n'
                            << "precision mediump float;" << '\n';
        }

        if (checked)
        {
            checked = false;
            continue;
        }
        else
        {
            *current_shader << (line + '\n');
        }
    }

    auto vertex_shader_source = process_directives(vertex_shader.str(), this);
    auto fragment_shader_source = process_directives(fragment_shader.str(), this);
    std::optional<std::string> geometry_shader_source;
    if (geometry_shader.str().empty())
    {
        geometry_shader_source = std::nullopt;
    }
    else
    {
        geometry_shader_source = process_directives(geometry_shader.str(), this);
    }

    shader_systems_[system_name] = std::make_unique<ShaderSystem>(vertex_shader_source, fragment_shader_source, geometry_shader_source);
    return shader_systems_[system_name].get();
}

}