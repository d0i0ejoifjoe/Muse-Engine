#pragma once

#include "glad.h"

#include <glm/glm.hpp>

namespace muse
{
    /** Enum for different address modes */
    enum class SamplerAddressMode
    {
        MIRRORED_REPEAT,
        REPEAT,
        CLAMP_TO_EDGE,
        CLAMP_TO_BORDER
    };

    /** Enum for sampler */
    enum class SamplerFilter
    {
        NEAREST,
        LINEAR
    };

    /**
     * 
     *  Sampler specification.
     * 
    */
    struct SamplerSpecification
    {
        SamplerAddressMode s_mode = SamplerAddressMode::REPEAT;
        SamplerAddressMode t_mode = SamplerAddressMode::REPEAT;
        SamplerAddressMode r_mode = SamplerAddressMode::REPEAT;

        SamplerFilter mag_filter = SamplerFilter::LINEAR;
        SamplerFilter min_filter = SamplerFilter::LINEAR;
        SamplerFilter mipmap_filter = SamplerFilter::LINEAR;
        
        bool use_mipmaps = true;

        glm::vec4 border_color = {1.0f, 1.0f, 1.0f, 1.0f};
    };

    /**
     * 
     *  Class that represents a sampler.
     * 
    */
    class Sampler
    {
    public:
        /**
         * 
         *  Create sampler.
         * 
         *  @param spec Specification.
         * 
        */
        Sampler(const SamplerSpecification& spec);

        /**
         * 
         *  Destroy sampler.
         * 
        */
        ~Sampler();

        /**
         * 
         *  Get sampler specification.
         * 
         *  @return Reference to specification.
         * 
        */
        const SamplerSpecification& specification() const;

        /**
         * 
         *  Get handle.
         * 
         *  @return Handle.
         * 
        */
        GLuint handle() const;

    private:
        /** Handle */
        GLuint handle_;

        /** Specification */
        SamplerSpecification specification_;
    };
}