#pragma once

#include "glad.h"

#include <cstdint>
#include <unordered_map>
#include <string>
#include <optional>

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
        void add_uniform(std::string_view uniform_name);

        /**
         * 
         *  Remove a uniform.
         * 
         *  @param uniform_name Name of uniform.
         * 
        */
        void remove_uniform(std::string_view uniform_name);

        /**
         * 
         *  Set uniform's value (integer).
         * 
         *  @param uniform_name Name of uniform to set.
         *  @param value New value for uniform.
         * 
        */
        void set_value(std::string_view uniform_name, std::int32_t value);
    
    private:
        /** Handle. */
        GLuint handle_;

        /** Uniform map. */
        std::unordered_map<std::string, GLuint> uniform_map_;
    };
}