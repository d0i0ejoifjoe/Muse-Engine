#pragma once

#include <filesystem>
#include <string>
#include <unordered_map>

namespace muse
{

class ShaderSystem;

class ShaderLibrary
{
  public:
    ShaderLibrary();

    void set_root_directory(std::filesystem::path root);

    std::filesystem::path root_directory() const;

    const std::string &load_shader(std::string_view filename, const std::string &name);

    const std::string &shader(const std::string &name);

    const ShaderSystem *shader_system(const std::string &name);

    const std::unordered_map<std::string, std::string> &shaders() const;

    const std::unordered_map<std::string, std::unique_ptr<ShaderSystem>> &shader_systems() const;

    // Compile shader means process all directives, for now just two, these are #include {name} and #shader {type}. The #shader should make that when we create a shader system
    // we read one file and first expression in file has to be #shader {type} if not throw error, then we threat the rest of file until the second #shader directive as vertex or what shader
    // was specified in {type} argument, if there's no minimum of two #shader {type} directives of {type} vertex and fragment throw error additional shader types gonna be geometry
    // and maybe tesselation. Now about include, #include directive should be written like this #include {name} where name is a function from array of this library if it isn't loaded
    // throw error, it needs to be written with quotes "".
    ShaderSystem *compile_shader(const std::string &shader_name, const std::string &system_name);

  private:
    std::unordered_map<std::string, std::string> shaders_;

    std::unordered_map<std::string, std::unique_ptr<ShaderSystem>> shader_systems_;

    std::filesystem::path root_;
};

}